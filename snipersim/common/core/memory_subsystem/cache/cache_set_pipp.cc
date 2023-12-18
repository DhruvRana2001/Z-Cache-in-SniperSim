#include <iostream>
#include <sstream>
#include <algorithm>

#include "log.h"
#include "stats.h"
#include "config.hpp"

#include "cache_set_pipp.h"

CacheSetInfoPIPP::CacheSetInfoPIPP(String name, String cfgname,
         core_id_t core_id, UInt32 associativity, UInt32 cache_block_size, UInt32 num_sets, UInt32 shared_cores,
         CacheBase::hash_t hash_function)
   : CacheSetInfoUMON(name, cfgname, core_id, associativity, cache_block_size, num_sets, shared_cores, hash_function)
   , m_min_insert_pos(Sim()->getCfg()->getInt(cfgname + "/pipp_min_insert"))
{
m_associativity = associativity;
activity_counters = new uint64_t[associativity];
for(uint32_t i=0; i<associativity; i++){
	activity_counters[i] = 0;
}
}

CacheSetInfoPIPP::~CacheSetInfoPIPP(){
	printf("[FINAL STATS]: ");
	for(uint32_t i=0; i<m_associativity; i++){
		printf("%d,", activity_counters[i]);
	}
	printf("\n");
	delete[] activity_counters;
}
CacheSetPIPP::CacheSetPIPP(
      CacheBase::cache_t cache_type,
      UInt32 associativity, UInt32 blocksize, UInt32 set_no, CacheSetInfoPIPP *info) :
   CacheSetUMON(cache_type, associativity, blocksize, info),
   m_set_no(set_no),
   m_info(info)
{
   m_lru_bits = new UInt8[m_associativity];
   for (UInt32 i = 0; i < m_associativity; i++)
      m_lru_bits[i] = i;

   rng = new Random();
}

CacheSetPIPP::~CacheSetPIPP()
{
   delete [] m_lru_bits;
}

UInt32
CacheSetPIPP::getReplacementIndex(CacheCntlr *cntlr, Core::ExtraMemoryRequestInfo *info, UInt32 &setidx, IntPtr addr)
{
   LOG_ASSERT_ERROR(info != NULL, "ExtraMemoryRequestInfo not set");
   CacheSetUMON::getReplacementIndex(cntlr, info, setidx, addr);

   auto requester = info->core_id;

   // UMON may allocate 0, but we need to store so we change that to 1
   auto allocation = getUMON()->getAllocation(requester);
   allocation = allocation > 0 ? allocation : 1;

   int insertpos = m_associativity - allocation;

   if (getUMON()->isStreaming(requester)) {
      if (getUMON()->isAllStreaming()) {
         insertpos = 0;
      } else {
         insertpos = m_associativity - getUMON()->countStreamingCores();
      }
   }

   
   uint32_t eight_above_lru = m_associativity - m_info->m_min_insert_pos;
   // default to LRU pos. (to prevent negatives) if not enough ways.
   if(m_info->m_min_insert_pos >= m_associativity){
	eight_above_lru = 0;
   }
   if(insertpos > eight_above_lru){
	insertpos = eight_above_lru;	
   }

   int index = 0;
   int max_bit = 0;
   m_info->lock.acquire();

   // Locate LRU
   for (UInt32 i = 0; i < m_associativity; i++)
   {
      if (m_lru_bits[i] > max_bit && isValidReplacement(i))
      {
         max_bit = m_lru_bits[i];
         index = i;
      }
   }

   // Shift every block at insertion position and out one down
   for (UInt32 y = 0; y < m_associativity; y++)
   {
      if (m_lru_bits[y] >= insertpos){
	m_info->activity_counters[m_lru_bits[y]]++;
	 m_lru_bits[y] ++;
	}
   }
   m_info->lock.release();
   // Move to insert position and return
   m_lru_bits[index] = insertpos;
   return index;
}

// Promote a used block
void
CacheSetPIPP::updateReplacementIndex(UInt32 accessed_index, Core::ExtraMemoryRequestInfo *info)
{
   CacheSetUMON::updateReplacementIndex(accessed_index, info);
   bool promote = false;

   // TODO Move to config
   if (!getUMON()->isStreaming(info->core_id)) {
      // Promote 3/4 normally
      promote = rng->next(4) < 3;
   } else {
      // promote 1/128 when streaming
      promote = rng->next(128) == 0;
   }

   if (promote) {
      int original = m_lru_bits[accessed_index];

      // Promote unless already at MRU
      if (original > 0) {
         int target = original - 1;
         for(UInt32 i = 0; i < m_associativity; i++) {
            if (m_lru_bits[i] == target) {
		m_info->lock.acquire();
		m_info->activity_counters[target]++;
		m_info->activity_counters[original]++;
		m_info->lock.release();
               m_lru_bits[i] = original;
               m_lru_bits[accessed_index] = target;
               break;
            }
         }
      }
   }
}
