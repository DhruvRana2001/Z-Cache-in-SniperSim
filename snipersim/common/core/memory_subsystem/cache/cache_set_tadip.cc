#include "cache_set_tadip.h"
#include "log.h"
#include "stats.h"
#include "config.hpp"

#include "simulator.h"

#include <iostream>
#include <algorithm>
#include <math.h>

// TADIP: Thread Aware DIP

CacheSetInfoTADIP::CacheSetInfoTADIP(String name, String cfgname, core_id_t core_id, UInt32 associativity, UInt32 num_sets) :
   m_num_sets(num_sets),
   m_master_core(core_id)
{
   m_shared_cores = Sim()->getCfg()->getInt(cfgname + "/shared_cores"); 
 printf("cfgname %s\n", cfgname.c_str());
   staticPolicies = Sim()->getCfg()->getBoolArray(cfgname + "/tadip/static", core_id);
   if(staticPolicies){
   uint32_t config = Sim()->getCfg()->getIntArray(cfgname + "/tadip/config", core_id);
   policyArray = new policy_t[m_shared_cores];
   
   printf("static policy %x\n", config);
   for(uint32_t i=0; i<m_shared_cores; i++){
	policyArray[i] = (config & (1<<i) ? BIP : LRU); 				
   } 
   }
   m_policy_counters = new int[m_shared_cores];
   m_stat_bip_switch = new UInt64[m_shared_cores];
   m_stat_lip_switch = new UInt64[m_shared_cores];
   accessToBIP = new UInt64[m_shared_cores];
   accessToLIP = new UInt64[m_shared_cores];


   m_time_of_last_switch = new SubsecondTime[m_shared_cores];
   m_time_in_lip = new SubsecondTime[m_shared_cores];
   m_time_in_bip = new SubsecondTime[m_shared_cores];

   for(core_id_t i = 0; i < m_shared_cores; i++) {
      m_policy_counters[i] = 0xFF;
      accessToBIP[i] = accessToLIP[i] = m_stat_lip_switch[i] = m_stat_bip_switch[i] = 0;

      registerStatsMetric(name, m_master_core+i, "switch-to-lip", &m_stat_lip_switch[i]);
      registerStatsMetric(name, m_master_core+i, "switch-to-bip", &m_stat_bip_switch[i]);
      registerStatsMetric(name, m_master_core+i, "time-in-lip", &m_time_in_lip[i]);
      registerStatsMetric(name, m_master_core+i, "time-in-bip", &m_time_in_bip[i]);
   }

   rng = new Random();

   int width = log2(num_sets);
#if 0
   int counter_width = log2(16); // TODO Move to configuration
#else
   int counter_width = log2(32*m_shared_cores/4);
#endif
   LOG_ASSERT_ERROR(width >= counter_width*2, "Too few sets to accomidate TADIP-F");
#if 0
   setno_higher_offset = (width - counter_width);
   setno_higher_filter = (int(pow(2, counter_width)) - 1) << (setno_higher_offset);
   setno_lower_filter = (num_sets - 1) ^ setno_higher_filter;
#else
   #if 1
   // straight from TADIPs paper. No modifications required, scales automatically.
   // Advantage of this is that the densitity stays the same as in the paper.
   setno_higher_offset = 7;
   setno_higher_filter = 0xf80;
   setno_lower_filter  = 0x7f; 
   #else 
	// double the density
	 setno_higher_offset = 6;
	 setno_higher_filter = 0x7c0;
	 setno_lower_filter  = 0x3f; 
   #endif
#endif
}

CacheSetInfoTADIP::~CacheSetInfoTADIP()
{
   for(uint32_t i=0; i<m_shared_cores; i++){
	   SubsecondTime period = Sim()->getCoreManager()->getCoreFromID(m_master_core)->getPerformanceModel()->getElapsedTime() - m_time_of_last_switch[i];

            if ((m_policy_counters[i]&0x100) > 0) {
               m_time_in_bip[i] += period;
            } else {
               m_time_in_lip[i] += period;
            }
	    printf("[FINAL STATS]: %u %u %u %u\n", m_time_in_lip[i].getNS(), m_time_in_bip[i].getNS(), m_stat_lip_switch[i], m_stat_bip_switch[i]);
    }

   delete[] m_policy_counters;
   delete rng;
}

// Hash function implementation
CacheSetInfoTADIP::policy_t CacheSetInfoTADIP::getSetPolicy(UInt32 set_no, core_id_t core_id)
{
   if(staticPolicies){
	return policyArray[core_id];
   }
   else{
   int upper = (set_no & setno_higher_filter) >> setno_higher_offset;
   int lower = set_no & setno_lower_filter;

   int adder1 = 0; //(lower & 0x1) ? m_shared_cores : 0;
   int adder2 = m_shared_cores; //!(lower & 0x1) ? 0 : m_shared_cores;

   if ((upper + core_id+adder2) == lower) {
      return LRU;
   } else if ((upper + core_id + adder1) == lower) {
      return BIP;
   } else {
      if (m_policy_counters[core_id] & 0x100) {
         return BIP;
      }
      return LRU;
   }
   }
}

bool CacheSetInfoTADIP::isOwner(UInt32 set_no, core_id_t core_id)
{
   int upper = (set_no & setno_higher_filter) >> setno_higher_offset;
   int lower = set_no & setno_lower_filter;
   int adder1 = 0; 
   int adder2 = m_shared_cores; 
   return (upper + core_id+adder2) == lower || (upper + core_id + adder1) == lower;
}

void CacheSetInfoTADIP::updateAccessCounter(UInt32 set_no, core_id_t requester, bool cache_hit, IntPtr address)
{
   if(staticPolicies)
	return;
   if (!cache_hit && isOwner(set_no, requester)) {
      // Miss in LRU set -> +1, miss in BIP set -> -1
      int direction = getSetPolicy(set_no, requester) == LRU ?  +1 : -1;
      if(direction == 1){
	accessToLIP[requester]++;
	}
	else{
	accessToBIP[requester]++;
	}
      int nval = m_policy_counters[requester] + direction;
      if (nval >= 0 && nval <= 511)
      {
         if ((m_policy_counters[requester] & 0x100) != (nval & 0x100)) {
            SubsecondTime period = Sim()->getCoreManager()->getCoreFromID(m_master_core)->getPerformanceModel()->getElapsedTime() - m_time_of_last_switch[requester];
            m_time_of_last_switch[requester] = Sim()->getCoreManager()->getCoreFromID(m_master_core)->getPerformanceModel()->getElapsedTime();

            //std::cout << "Core " << requester << " changed state to " << (nval&0x100 ? "BIP" : "LRU") << "\n";
            if ((nval&0x100) > 0) {
               m_stat_bip_switch[requester]++;
               m_time_in_lip[requester] += period;
            } else {
               m_stat_lip_switch[requester]++;
               m_time_in_bip[requester] += period;
            }
         }
         m_policy_counters[requester] = nval;
      }
   }
}

// Check if a insert in set_no by requester should be promoted to MRU position, or be inserted at LRU
bool CacheSetInfoTADIP::getPromote(UInt32 set_no, core_id_t requester)
{
   auto policy = getSetPolicy(set_no, requester);

   if (policy == BIP) {
      // For every 32'th insert into a BIP set we return LRU because we want it to promote
      if (rng->next(32) != 0) {
         return false;
      }
   }

   // True for LRU, or if we fall though on BIP
   return true;
}



CacheSetTADIP::CacheSetTADIP(
      CacheBase::cache_t cache_type,
      UInt32 associativity, UInt32 blocksize, UInt32 set_no, CacheSetInfoTADIP* cache_set_info) :
   CacheSet(cache_type, associativity, blocksize),
   m_set_no(set_no),
   m_cache_set_info(cache_set_info)
{
   m_lru_bits = new UInt8[m_associativity];
   for (UInt32 i = 0; i < m_associativity; i++)
      m_lru_bits[i] = i;

}

CacheSetTADIP::~CacheSetTADIP()
{
   delete [] m_lru_bits;
}

UInt32
CacheSetTADIP::getReplacementIndex(CacheCntlr *cntlr, Core::ExtraMemoryRequestInfo *info, UInt32 &setidx, IntPtr addr)
{
   LOG_ASSERT_ERROR(info != NULL, "ExtraMemoryRequestInfo not set");

   bool promote = m_cache_set_info->getPromote(m_set_no, info->core_id);
   m_cache_set_info->updateAccessCounter(m_set_no, info->core_id, false, info->address);

   // Scan for LRU and replace it
   int max_bit = 0;
   int index = 0;

   for (UInt32 i = 0; i < m_associativity; i++)
   {
      if (m_lru_bits[i] > max_bit && isValidReplacement(i))
      {
         index = i;
         max_bit = m_lru_bits[i];
      }
   }

   if (promote) {
      moveToMRU(index);
   }
   return index;
}

// Moves the block at accessed_index to MRU
void
CacheSetTADIP::updateReplacementIndex(UInt32 accessed_index, Core::ExtraMemoryRequestInfo *info)
{
   m_cache_set_info->updateAccessCounter(m_set_no, info->core_id, true, info->address);
   moveToMRU(accessed_index);
}

void CacheSetTADIP::moveToMRU(UInt32 accessed_index)
{
   for (UInt32 i = 0; i < m_associativity; i++)
   {
      if (m_lru_bits[i] < m_lru_bits[accessed_index])
         m_lru_bits[i] ++;
   }
   m_lru_bits[accessed_index] = 0;
}
