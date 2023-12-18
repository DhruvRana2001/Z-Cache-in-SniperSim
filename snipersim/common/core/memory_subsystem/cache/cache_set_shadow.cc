#include "cache_set_shadow.h"
#include "cache_set_lru.h"
#include "pr_l1_cache_block_info.h"

#include "log.h"

#include <iostream>

// Cache shadow tag implementation.
// This works like the CacheSet base class except it implements a LRU managed shadow tag for each sharing core

CacheSetInfoShadow::CacheSetInfoShadow(
      String name, String cfgname,
      core_id_t core_id, UInt32 associativity, UInt32 cache_block_size, UInt32 num_sets, UInt32 shared_cores,
      CacheBase::hash_t hash_function)
  : m_shared_cores(shared_cores)
  , m_associativity(associativity)
  , m_cache_base(name, num_sets, associativity, cache_block_size, hash_function)
{

   String sampling = Sim()->getCfg()->getStringArray(cfgname + "/st/sampling", core_id);
   m_set_info = CacheSet::createCacheSetInfo(name, cfgname, core_id, "lru", associativity);

   m_accesses = new UInt64[shared_cores];
   m_misses = new UInt64[shared_cores];
   m_access_counters = new UInt64*[shared_cores];
   shadowlocks = new Lock[shared_cores];

   for(UInt32 core = 0; core < shared_cores; core++) {
      m_accesses[core] = m_misses[core] = 0;
      m_access_counters[core] = new UInt64[associativity];
      for(UInt32 i = 0; i < associativity; i++) {
         m_access_counters[core][i] = 0;
      }

      registerStatsMetric(name, core, "shadow-tag-accesses", &m_accesses[core]);
      registerStatsMetric(name, core, "shadow-tag-misses", &m_misses[core]);

      if (sampling == "full")
      {
         for(UInt64 set_index = 0; set_index < num_sets; ++set_index)
         {
            m_sets[getSetIndex(core, set_index)] = CacheSet::createCacheSet(name, core_id, "lru", CacheBase::PR_L1_CACHE, associativity, 0, m_set_info);
         }
      }
      else if (sampling == "count")
      {
         // Use a specified number of sample sets, evenly distributed
         UInt64 offset = num_sets / Sim()->getCfg()->getInt(cfgname + "/st/sample_sets");
         for(UInt64 set_index = 0; set_index < num_sets; set_index += offset)
         {
            m_sets[getSetIndex(core, set_index)] = CacheSet::createCacheSet(name, core_id, "lru", CacheBase::PR_L1_CACHE, associativity, 0, m_set_info);
         }
      }
      else
      {
         LOG_PRINT_ERROR("Invalid Shadow Tag sampling method %s", sampling.c_str());
      }
   }
}

CacheSetInfoShadow::~CacheSetInfoShadow()
{
   if(m_set_info)
      delete m_set_info;
}

void CacheSetInfoShadow::accessCache(core_id_t core, IntPtr address, bool hit)
{
   IntPtr tag; UInt32 set_index;
   m_cache_base.splitAddress(address, tag, set_index);

   if (isSampledSet(core, set_index))
   {
      UInt32 line_index = -1;

      // Get sample set, and attempt to find the requested address
      auto set = m_sets[getSetIndex(core, set_index)];
      bool atd_hit = set->find(tag, &line_index) != NULL;
      shadowlocks[core].acquire();
      m_accesses[core]++;
      if (atd_hit)
      {
         // Get LRU bits and update access counters, then promote to MRU
         CacheSetLRU* lru_set = dynamic_cast<CacheSetLRU*>(set);
         UInt32 lru_bits = lru_set->getLRUBit(line_index);	
         m_access_counters[core][lru_bits]++;

         lru_set->updateReplacementIndex(line_index, 0);
      }
      else
      {
         m_misses[core]++;

         PrL1CacheBlockInfo* cache_block_info = new PrL1CacheBlockInfo(tag, CacheState::MODIFIED);
         bool eviction; PrL1CacheBlockInfo evict_block_info;
         set->insert(cache_block_info, NULL, &eviction, &evict_block_info, NULL);
	 delete cache_block_info;
      }
      shadowlocks[core].release();
   }
}

CacheSetShadow::CacheSetShadow(
   CacheBase::cache_t cache_type, UInt32 associativity, UInt32 blocksize, CacheSetInfoShadow *info) :
   CacheSet(cache_type, associativity, blocksize),
   m_info(info)
{
}

CacheSetShadow::~CacheSetShadow()
{
}

UInt32
CacheSetShadow::getReplacementIndex(CacheCntlr *cntlr, Core::ExtraMemoryRequestInfo *request, UInt32 &setidx, IntPtr addr)
{
   LOG_ASSERT_ERROR(request != NULL, "Request information not sent to getReplacementIndex, possible bug!");

   LOG_ASSERT_ERROR(m_info != NULL, "info == NULL");
   m_info->accessCache(request->core_id, request->address, false);

   return 0;
}

void
CacheSetShadow::updateReplacementIndex(UInt32 accessed_index, Core::ExtraMemoryRequestInfo *request)
{
   LOG_ASSERT_ERROR(request != NULL, "Request information not sent to updateReplacementIndex, possible bug!");

   m_info->accessCache(request->core_id, request->address, true);
}
