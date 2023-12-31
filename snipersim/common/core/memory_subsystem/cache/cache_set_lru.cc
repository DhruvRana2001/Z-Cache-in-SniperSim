#include "cache_set_lru.h"
#include "log.h"
#include "stats.h"

// Implements LRU replacement, optionally augmented with Query-Based Selection [Jaleel et al., MICRO'10]

CacheSetLRU::CacheSetLRU(
      CacheBase::cache_t cache_type,
      UInt32 associativity, UInt32 blocksize, CacheSetInfoLRU* set_info, UInt8 num_attempts)
   : CacheSet(cache_type, associativity, blocksize)
   , m_num_attempts(num_attempts)
   , m_set_info(set_info)
{
   m_associativity = associativity;
   m_lru_bits = new UInt8[m_associativity];
   core_bits = new uint8_t[m_associativity];
   for (UInt32 i = 0; i < m_associativity; i++){
      m_lru_bits[i] = i;
      core_bits[i] = 255;
   }
}

CacheSetLRU::~CacheSetLRU()
{
   delete [] m_lru_bits;
   delete [] core_bits;
}

UInt32
CacheSetLRU::getReplacementIndex(CacheCntlr *cntlr, Core::ExtraMemoryRequestInfo *info, UInt32 &setidx, IntPtr addr)
{

    auto this_period = m_set_info->getCurrentAllocationPeriod();
   if (m_set_info->m_current_period < this_period && m_set_info->m_associativity >=16) {
        m_set_info->m_current_period = this_period;
   	printf("[PERIOD]:\t %d\n", this_period);
        printf("[ACTUALS]:\t");
	for(uint32_t i=0; i<32; i++){
		printf("%d,", m_set_info->sizes[i]);	
	}
	printf("\n");
   }

   //Tomwi: this is nasty but quick way
   //to print some LLC-stats for LRU.
   if(info && m_set_info->m_associativity>=16){ 
	m_set_info->core_locks[info->core_id].acquire();
	m_set_info->sizes[info->core_id]++;
	m_set_info->core_locks[info->core_id].release();
   }

   // First try to find an invalid block
   /*for (UInt32 i = 0; i < m_associativity; i++)
   {
      if (!m_cache_block_info_array[i]->isValid())
      {
         // Mark our newly-inserted line as most-recently used
         moveToMRU(i);
         return i;
      }
   }*/

   // Make m_num_attemps attempts at evicting the block at LRU position
   for(UInt8 attempt = 0; attempt < m_num_attempts; ++attempt)
   {
      UInt32 index = 0;
      UInt8 max_bits = 0;
      for (UInt32 i = 0; i < m_associativity; i++)
      {
         if (m_lru_bits[i] > max_bits && isValidReplacement(i))
         {
            index = i;
            max_bits = m_lru_bits[i];
         }
      }
      LOG_ASSERT_ERROR(index < m_associativity, "Error Finding LRU bits");
   if(info && m_set_info->m_associativity>=16){
	uint8_t victim_core = core_bits[index];
	if(victim_core < 0xff){
		m_set_info->core_locks[victim_core].acquire();
		m_set_info->sizes[victim_core]--;
		m_set_info->core_locks[victim_core].release();
	}
   }

      // Tomwi: To track usage, will probably trigger UMONS as well.
   if(info && m_set_info->m_associativity>=16){ 
	   core_bits[index] = info->core_id;
   }

      // Runar: Breaking here
      moveToMRU(index);
      return index;

      bool qbs_reject = false;
      if (attempt < m_num_attempts - 1)
      {
         LOG_ASSERT_ERROR(cntlr != NULL, "CacheCntlr == NULL, QBS can only be used when cntlr is passed in");
         qbs_reject = cntlr->isInLowerLevelCache(m_cache_block_info_array[index]);
      }

      if (qbs_reject)
      {
         // Block is contained in lower-level cache, and we have more tries remaining.
         // Move this block to MRU and try again
         moveToMRU(index);
         cntlr->incrementQBSLookupCost();
         continue;
      }
      else
      {
         // Mark our newly-inserted line as most-recently used
         moveToMRU(index);
         m_set_info->incrementAttempt(attempt);
         return index;
      }
   }

   LOG_PRINT_ERROR("Should not reach here");
}

UInt8
CacheSetLRU::getLRUBit(UInt32 accessed_index)
{
   return m_lru_bits[accessed_index];
}


void
CacheSetLRU::updateReplacementIndex(UInt32 accessed_index, Core::ExtraMemoryRequestInfo *info)
{
   //printf("updating %d\n", accessed_index);
   m_set_info->increment(m_lru_bits[accessed_index]);
   moveToMRU(accessed_index);
}

void
CacheSetLRU::moveToMRU(UInt32 accessed_index)
{
   for (UInt32 i = 0; i < m_associativity; i++)
   {
      if (m_lru_bits[i] < m_lru_bits[accessed_index]){
	// Increase recency counter
         m_lru_bits[i] ++;
      }
   }
   m_lru_bits[accessed_index] = 0;
}

CacheSetInfoLRU::CacheSetInfoLRU(String name, String cfgname, core_id_t core_id, UInt32 associativity, UInt8 num_attempts)
   : m_associativity(associativity)
   , m_attempts(NULL)
{
   m_access = new UInt64[m_associativity];
   for(UInt32 i = 0; i < m_associativity; ++i)
   {
      m_access[i] = 0;
      registerStatsMetric(name, core_id, String("access-mru-")+itostr(i), &m_access[i]);
   }

   if (num_attempts > 1)
   {
      m_attempts = new UInt64[num_attempts];
      for(UInt32 i = 0; i < num_attempts; ++i)
      {
         m_attempts[i] = 0;
         registerStatsMetric(name, core_id, String("qbs-attempt-")+itostr(i), &m_attempts[i]);
      }
   }

const ComponentPeriod* period = Sim()->getDvfsManager()->getCoreDomain(Sim()->getDvfsManager()->getCoreDomainId(0));
    m_time_converter = new SubsecondTimeCycleConverter(period);
    core_locks = new Lock[32];
    sizes = new uint32_t[32];
    for(uint32_t i=0; i<32; i++){
	sizes[i] = 0;
    }
};

CacheSetInfoLRU::~CacheSetInfoLRU()
{
   delete [] m_access;
   if (m_attempts)
      delete [] m_attempts;
}
