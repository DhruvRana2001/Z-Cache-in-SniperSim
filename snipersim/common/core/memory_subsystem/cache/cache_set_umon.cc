#include "cache_set_umon.h"
#include "cache_set_lru.h"
#include "pr_l1_cache_block_info.h"

#include "log.h"
#include "stats.h"

#include <iostream>

// Cache umon implementation
// This is an extension of the shadow tag implementation adding utility calculations

CacheSetInfoUMON::CacheSetInfoUMON(
         String name, String cfgname,
         core_id_t core_id, UInt32 associativity, UInt32 cache_block_size, UInt32 num_sets, UInt32 shared_cores,
         CacheBase::hash_t hash_function)
   : CacheSetInfoShadow(name, cfgname, core_id, associativity, cache_block_size, num_sets, shared_cores, hash_function)
   , m_reallocate_period(Sim()->getCfg()->getIntArray(cfgname + "/umon/reallocate_period", core_id))
   , m_enable_stream_detection(Sim()->getCfg()->getBoolArray(cfgname + "/umon/enable_stream_detection", core_id))
{
   // Set the intial allocations be equal for all cores
   m_current_period = 0;
   int per_core = associativity / shared_cores;
   m_allocations = new UInt64[shared_cores];
   for(UInt32 i = 0; i < shared_cores; i++) {
      m_allocations[i] = per_core;
      registerStatsMetric(name, i, "umon.allocation", &m_allocations[i]);
   }

   if(m_enable_stream_detection) {
      m_stream_miss_count = Sim()->getCfg()->getInt(cfgname + "/umon/stream_miss_count_limit");
      m_stream_miss_fraction = Sim()->getCfg()->getFloat(cfgname + "/umon/stream_miss_fraction_limit");

      m_stream_flags = new UInt64[shared_cores];
      for(UInt32 i = 0; i < shared_cores; i++) {
         m_stream_flags[i] = false;
         registerStatsMetric(name, i, "umon.stream_flag", &m_stream_flags[i]);
      }
   }

   logAllocations();

   // Create a time converter used to convert elapsed time to cycles. ASSUMES CONSTANT FREQ
   const ComponentPeriod* period = Sim()->getDvfsManager()->getCoreDomain(Sim()->getDvfsManager()->getCoreDomainId(0));
   m_time_converter = new SubsecondTimeCycleConverter(period);

}

CacheSetInfoUMON::~CacheSetInfoUMON()
{
   delete[] m_allocations;
   if (m_enable_stream_detection)
      delete[] m_stream_flags;

   delete m_time_converter;
}

void
CacheSetInfoUMON::accessCache(core_id_t core, IntPtr address, bool hit)
{
   // Update shadow tags
   CacheSetInfoShadow::accessCache(core, address, hit);

   auto this_period = getCurrentAllocationPeriod();

   // If we have moved to the next period we must do a reallocation
   if (m_current_period < this_period) {
      m_current_period = this_period;

      // Do streaming detection
      int count_streaming = 0;
      if (m_enable_stream_detection) {
         for(UInt32 core = 0; core < m_shared_cores; core++) {
            // Check if miss count or miss flag indicates a streaming app, if either limit is 0 that check is disabled
            bool count_flag = m_stream_miss_count > 0 && getMisses(core) > m_stream_miss_count;
            bool rate_flag = m_stream_miss_fraction > 0 && ((double)getMisses(core)/getAccesses(core)) > m_stream_miss_fraction;

            m_stream_flags[core] = count_flag || rate_flag;

            if (m_stream_flags[core]) {
               count_streaming++;
            }
         }
      }

      // Initial allocations, as the UCP paper we force atleast one allocation to each core
      for(UInt32 core = 0; core < m_shared_cores; core++) {
         m_allocations[core] = 1;
      }

      // Number of ways to allocate
      int remainding = m_associativity - m_shared_cores;

      // While we have remainding ways to distribute (and we're not in a situation where all cores are streaming)
      while(remainding > 0 && count_streaming < m_shared_cores) {
         // Keep track of winner, maximum mu and required blocks
         int winner = -1, max_mu = -1, required_blocks = -1;

         for(UInt32 core = 0; core < m_shared_cores; core++) {
            // Skip any streaming cores
            if (m_enable_stream_detection && m_stream_flags[core]) continue;


            int allocated = m_allocations[core];
            UInt32 blocks = 0;
            int mu = getMaxMU(core, allocated, remainding, &blocks);
            if (mu > max_mu) {
               winner = core;
               max_mu = mu;
               required_blocks = blocks;
            }
         }

         assert(winner != -1);
         assert(max_mu != -1);
         assert(required_blocks != -1);

         // Add allocations and decrement remainding
         m_allocations[winner] += required_blocks;
         remainding -= required_blocks;
      }


      logAllocations();

      halvAccessCounters();

   }
}

UInt32
CacheSetInfoUMON::getMU(core_id_t core, UInt32 lower, UInt32 upper)
{
   assert(lower < upper);
   UInt32 U = 0;
   for (UInt32 i = lower ; i < upper; i ++) {
      U += getAccessCounter(core, i);
   }
   return U == 0 ? 0 : U / (upper-lower);
}

UInt32
CacheSetInfoUMON::getMaxMU(core_id_t core, UInt32 allocated, int remainding, UInt32* blocks)
{
   assert(remainding > 0);
   assert(remainding < int(m_associativity)+1);

   int max_mu = 0;
   int max_blocks = 1;

   for (int i = 1; i <= remainding; i++) {
      int mu = getMU(core, allocated, allocated+i);
      if (mu > max_mu) {
         max_mu = mu;
         max_blocks = i;
      }
   }

   *blocks = max_blocks;
   return max_mu;
}

void CacheSetInfoUMON::logAllocations()
{

   //return;

   /*std::ostringstream s;
   s << "umon.allocation." << getCurrentAllocationPeriod();
   std::string alloc_tag(s.str());

   s.str("");
   s << "umon.streaming." << getCurrentAllocationPeriod();
   std::string streaming_tag(s.str());

   for(UInt32 core = 0; core < m_shared_cores; core++) {
      UInt64* val = new UInt64();
      *val = m_allocations[core];
      registerStatsMetric(m_name, m_master_core+core, alloc_tag.c_str(), val);

      if (m_enable_stream_detection) {
         UInt64* val = new UInt64();
         *val = isStreaming(core);
         registerStatsMetric(m_name, m_master_core+core, streaming_tag.c_str(), val);
      }
   }*/

   // return;

   std::cout << "[UMON] Access counters:\n";
   for(UInt32 core = 0; core < m_shared_cores; core++) {
      std::cout << "[UMON] Core" << core << ": ";
      for(UInt32 way = 0; way < m_associativity; way++) {
         std::cout << getAccessCounter(core, way) << ", ";
      }
      std::cout << "\n";
   }

   std::cout << "[UMON] Allocations ";
   if (m_enable_stream_detection) {
      std::cout << "(Streaming: " << countStreamingCores() << ")";
   }
   std::cout << ": ";
   for(UInt32 core = 0; core < m_shared_cores; core++) {
      std::cout << m_allocations[core] << (m_enable_stream_detection && m_stream_flags[core] ? "s, " : ",  ");
   }
   std::cout << "\n";
}

CacheSetUMON::CacheSetUMON(
   CacheBase::cache_t cache_type, UInt32 associativity, UInt32 blocksize, CacheSetInfoUMON *info)
   : CacheSetShadow(cache_type, associativity, blocksize, info)
   , m_info(info)
{
}

CacheSetUMON::~CacheSetUMON()
{
}

UInt32
CacheSetUMON::getReplacementIndex(CacheCntlr *cntlr, Core::ExtraMemoryRequestInfo *request, UInt32 &setidx, IntPtr addr)
{
   LOG_ASSERT_ERROR(request != NULL, "Request information not sent to getReplacementIndex, possible bug!");

   LOG_ASSERT_ERROR(m_info != NULL, "info == NULL");
   m_info->accessCache(request->core_id, request->address, false);

   return 0;
}

void
CacheSetUMON::updateReplacementIndex(UInt32 accessed_index, Core::ExtraMemoryRequestInfo *request)
{
   LOG_ASSERT_ERROR(request != NULL, "Request information not sent to updateReplacementIndex, possible bug!");

   m_info->accessCache(request->core_id, request->address, true);
}
