#ifndef CACHE_SET_TADIP_H
#define CACHE_SET_TADIP_H

#include "cache_set.h"

#include "random.h"

#include "simulator.h"
#include "core_manager.h"
#include "performance_model.h"
#include "dvfs_manager.h"


class CacheSetInfoTADIP : public CacheSetInfo
{

   public:
      enum policy_t {
         LRU,
         BIP
      };

      CacheSetInfoTADIP(String name, String cfgname, core_id_t core_id, UInt32 associativity, UInt32 num_sets);
      virtual ~CacheSetInfoTADIP();

      void updateAccessCounter(UInt32 set_no, core_id_t requester, bool core_hit, IntPtr address);

      bool getPromote(UInt32 set_no, core_id_t requester);

   private:
      UInt32 m_num_sets;
      bool staticPolicies;
      policy_t* policyArray;
      core_id_t m_master_core;
      core_id_t m_shared_cores;

      int* m_policy_counters;

      UInt64* m_stat_bip_switch;
      UInt64* m_stat_lip_switch;
      UInt64* accessToBIP;
      UInt64* accessToLIP;
      SubsecondTime* m_time_of_last_switch;
      SubsecondTime* m_time_in_bip;
      SubsecondTime* m_time_in_lip;

      Random* rng;


      int setno_lower_filter;
      int setno_higher_filter;
      int setno_higher_offset;

      policy_t getSetPolicy(UInt32 set_no, core_id_t requester);
      bool isOwner(UInt32 set_no, core_id_t core_id);
};

class CacheSetTADIP : public CacheSet
{
   public:
      CacheSetTADIP(CacheBase::cache_t cache_type,
            UInt32 associativity, UInt32 blocksize, UInt32 set_no, CacheSetInfoTADIP* cache_set_info);
      ~CacheSetTADIP();

      UInt32 getReplacementIndex(CacheCntlr *cntlr, Core::ExtraMemoryRequestInfo *info, UInt32 &setidx, IntPtr addr);
      void updateReplacementIndex(UInt32 accessed_index, Core::ExtraMemoryRequestInfo *info);

   private:
      void moveToMRU(UInt32 accessed_index);

      UInt32 m_set_no;
      UInt8* m_lru_bits;
      CacheSetInfoTADIP* m_cache_set_info;

};

#endif /* CACHE_SET_TADIP_H */
