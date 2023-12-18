#include "cache_set_drrip.h"
#include "simulator.h"
#include "config.hpp"
#include "stats.h"
#include "log.h"
#include "math.h"

// DRRIP: Dynamic Re-reference Interval Prediction policy

CacheSetInfoDRRIP::CacheSetInfoDRRIP(String name, String cfgname, core_id_t core_id, UInt32 associativity, UInt32 num_sets) :
   m_num_sets(num_sets),
   m_master_core(core_id)
{
   m_shared_cores = Sim()->getCfg()->getInt(cfgname + "/shared_cores");

   m_policy_counters = new int[m_shared_cores];
   m_stat_srrip_switch = new UInt64[m_shared_cores];
   m_stat_brrip_switch = new UInt64[m_shared_cores];
   m_time_of_last_switch = new SubsecondTime[m_shared_cores];
   m_time_in_srrip = new SubsecondTime[m_shared_cores];
   m_time_in_brrip = new SubsecondTime[m_shared_cores];

   accessToBIP = new UInt64[m_shared_cores];
   accessToLIP = new UInt64[m_shared_cores];


   for(core_id_t i = 0; i < m_shared_cores; i++) {
      m_policy_counters[i] = 0x1FF;
      accessToBIP[i] = accessToLIP[i] = m_stat_srrip_switch[i] = m_stat_brrip_switch[i] = 0;

      registerStatsMetric(name, m_master_core+i, "switch-to-srrip", &m_stat_srrip_switch[i]);
      registerStatsMetric(name, m_master_core+i, "switch-to-brrip", &m_stat_brrip_switch[i]);
      registerStatsMetric(name, m_master_core+i, "time-in-srrip", &m_time_in_srrip[i]);
      registerStatsMetric(name, m_master_core+i, "time-in-brrip", &m_time_in_brrip[i]);
   }

   rng = new Random();

   int width = log2(num_sets);
   int counter_width = log2(32*m_shared_cores/4); // TODO Move to configuration

   LOG_ASSERT_ERROR(width >= counter_width*2, "Too few sets to accomidate DRRIP");
#if 0
   setno_higher_offset = (width - counter_width);
   setno_higher_filter = (int(pow(2, counter_width)) - 1) << (setno_higher_offset);
   setno_lower_filter = (num_sets - 1) ^ setno_higher_filter;
#else
   // straight from TADIPs paper. No modifications required, scales automatically.
   setno_higher_offset = 7;
   setno_higher_filter = 0xf80;
   setno_lower_filter  = 0x7f; 
#endif
}

CacheSetInfoDRRIP::~CacheSetInfoDRRIP()
{
    for(uint32_t i=0; i<m_shared_cores; i++){
	   SubsecondTime period = Sim()->getCoreManager()->getCoreFromID(m_master_core)->getPerformanceModel()->getElapsedTime() - m_time_of_last_switch[i];

            if ((m_policy_counters[i]&0x200) > 0) {
               m_time_in_brrip[i] += period;
            } else {
               m_time_in_srrip[i] += period;
            }
	    printf("[FINAL STATS]: %u %u %u %u\n", m_time_in_srrip[i].getNS(), m_time_in_brrip[i].getNS(), m_stat_srrip_switch[i], m_stat_brrip_switch[i]);
    }


   delete[] m_policy_counters;
   delete rng;
}

// Hash function implementation
CacheSetInfoDRRIP::policy_t CacheSetInfoDRRIP::getSetPolicy(UInt32 set_no, core_id_t core_id)
{
   int upper = (set_no & setno_higher_filter) >> setno_higher_offset;
   int lower = set_no & setno_lower_filter;

   if ((upper + core_id) == lower) {
      return SRRIP;
   } else if ((upper + core_id + m_shared_cores) == lower) {
      return BRRIP;
   } else {
      if (m_policy_counters[core_id] & 0x200) {
         return BRRIP;
      }
      return SRRIP;
   }
}

bool CacheSetInfoDRRIP::isOwner(UInt32 set_no, core_id_t core_id)
{
   int upper = (set_no & setno_higher_filter) >> setno_higher_offset;
   int lower = set_no & setno_lower_filter;

   return (upper + core_id) == lower || (upper + core_id + m_shared_cores) == lower;
}

void CacheSetInfoDRRIP::updateAccessCounter(UInt32 set_no, core_id_t requester, bool cache_hit, IntPtr address)
{
   if (!cache_hit) {
      if (isOwner(set_no, requester)) {
         // Miss in SRRIP set -> +1, miss in BRRIP set -> -1
         int direction = getSetPolicy(set_no, requester) == SRRIP ?  +1 : -1;

	if(direction == 1){
		accessToLIP[requester]++;
	}
	else{
		accessToBIP[requester]++;
	}

         int nval = m_policy_counters[requester] + direction;
         if (nval >= 0 && nval <= 1024)
         {
            if ((m_policy_counters[requester] & 0x200) != (nval & 0x200)) {
               SubsecondTime period = Sim()->getCoreManager()->getCoreFromID(m_master_core)->getPerformanceModel()->getElapsedTime() - m_time_of_last_switch[requester];
               m_time_of_last_switch[requester] = Sim()->getCoreManager()->getCoreFromID(m_master_core)->getPerformanceModel()->getElapsedTime();

               //std::cout << "Core " << requester << " changed state to " << (nval&0x200 ? "BRRIP" : "SRRIP") << "\n";
               if ((nval&0x200) > 0) {
                  m_stat_brrip_switch[requester]++;
                  m_time_in_brrip[requester] += period;
               } else {
                  m_stat_srrip_switch[requester]++;
                  m_time_in_srrip[requester] += period;
               }
            }
            m_policy_counters[requester] = nval;
         }

      }
   }


}

UInt8 CacheSetInfoDRRIP::getPosition(UInt32 set_no, core_id_t requester, UInt8 srrip_pos, UInt8 brrip_pos)
{
   auto policy = getSetPolicy(set_no, requester);

   if (policy == BRRIP) {
         // Only insert at brrip pos 1/32th of the time
         if (rng->next(32) == 0) {
            return brrip_pos;
         }
	/* 
	"to avoid cache thrashing, we propose Bimodal RRIP (BRRIP) that inserts
	majority of cache blocks with a distant re-reference interval
	prediction (i.e., RRPV of 2^M –1)"
	*/
	 else{
	    return brrip_pos+1; 
	 }
   }

   // srrip position for SRRIP sets
   return srrip_pos;
}


CacheSetDRRIP::CacheSetDRRIP(
      String cfgname, core_id_t core_id,
      CacheBase::cache_t cache_type,
      UInt32 associativity, UInt32 blocksize, UInt32 set_no, CacheSetInfoDRRIP* set_info)
   : CacheSet(cache_type, associativity, blocksize)
   , m_rrip_numbits(Sim()->getCfg()->getIntArray(cfgname + "/drrip/bits", core_id))
   , m_rrip_max((1 << m_rrip_numbits) - 1)
   , m_srrip_insert(m_rrip_max - 1)
   , m_brrip_insert(m_rrip_max - 1)
   , m_replacement_pointer(0)
   , m_set_no(set_no)
   , m_set_info(set_info)
{
   assert(sizeof(UInt8)*8 >= m_rrip_numbits);

   m_rrip_bits = new UInt8[m_associativity];
   for (UInt32 i = 0; i < m_associativity; i++)
      m_rrip_bits[i] = m_rrip_max;
}

CacheSetDRRIP::~CacheSetDRRIP()
{
   delete [] m_rrip_bits;
}

UInt32
CacheSetDRRIP::getReplacementIndex(CacheCntlr *cntlr, Core::ExtraMemoryRequestInfo *info, UInt32 &setidx, IntPtr addr)
{
   LOG_ASSERT_ERROR(info != NULL, "ExtraMemoryRequestInfo not set");

   UInt8 insert_position = m_set_info->getPosition(m_set_no, info->core_id, m_srrip_insert, m_brrip_insert);
   m_set_info->updateAccessCounter(m_set_no, info->core_id, false, info->address);

   // Original paper always starts at the leftmost position
   m_replacement_pointer = 0;

   for(UInt32 j = 0; j <= m_rrip_max; ++j)
   {
      for (UInt32 i = 0; i < m_associativity; i++)
      {
         if (m_rrip_bits[m_replacement_pointer] >= m_rrip_max)
         {
            // We choose the first non-touched line as the victim (note that we start searching from the replacement pointer position)
            UInt8 index = m_replacement_pointer;

            m_replacement_pointer = (m_replacement_pointer + 1) % m_associativity;
            // Prepare way for a new line: set prediction to 'long'
            m_rrip_bits[index] = insert_position;

            LOG_ASSERT_ERROR(isValidReplacement(index), "DRRIP selected an invalid replacement candidate" );

            return index;
         }

         m_replacement_pointer = (m_replacement_pointer + 1) % m_associativity;
      }

      // Increment all RRIP counters until one hits RRIP_MAX
      for (UInt32 i = 0; i < m_associativity; i++)
      {
         if (m_rrip_bits[i] < m_rrip_max)
         {
            m_rrip_bits[i]++;
         }
      }
   }

   LOG_PRINT_ERROR("Error finding replacement index");
}

void
CacheSetDRRIP::updateReplacementIndex(UInt32 accessed_index, Core::ExtraMemoryRequestInfo *info)
{
   m_set_info->updateAccessCounter(m_set_no, info->core_id, true, info->address);
   if (m_rrip_bits[accessed_index] > 0)
      m_rrip_bits[accessed_index]--;
}
