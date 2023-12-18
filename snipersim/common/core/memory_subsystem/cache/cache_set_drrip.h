#ifndef CACHE_SET_DRRIP_H
#define CACHE_SET_DRRIP_H

#include "cache_set.h"
#include "cache_set_lru.h"

#include "random.h"
#include "simulator.h"
#include "core_manager.h"
#include "performance_model.h"
#include "dvfs_manager.h"

class CacheSetInfoDRRIP : public CacheSetInfo
{

   public:
      enum policy_t {
         SRRIP,
         BRRIP
      };

      CacheSetInfoDRRIP(String name, String cfgname, core_id_t core_id, UInt32 associativity, UInt32 num_sets);
      virtual ~CacheSetInfoDRRIP();

      void updateAccessCounter(UInt32 set_no, core_id_t requester, bool core_hit, IntPtr address);

      UInt8 getPosition(UInt32 set_no, core_id_t requester, UInt8 srrip_pos, UInt8 brrip_pos);

   private:
      UInt32 m_num_sets;
      core_id_t m_master_core;
      core_id_t m_shared_cores;

      int* m_policy_counters;

      UInt64* m_stat_srrip_switch;
      UInt64* m_stat_brrip_switch;
      UInt64* accessToBIP;
      UInt64* accessToLIP;

      SubsecondTime* m_time_of_last_switch;
      SubsecondTime* m_time_in_srrip;
      SubsecondTime* m_time_in_brrip;

      Random* rng;


      int setno_lower_filter;
      int setno_higher_filter;
      int setno_higher_offset;

      policy_t getSetPolicy(UInt32 set_no, core_id_t requester);
      bool isOwner(UInt32 set_no, core_id_t core_id);
};

class CacheSetDRRIP : public CacheSet
{
   public:
      CacheSetDRRIP(String cfgname, core_id_t core_id,
            CacheBase::cache_t cache_type,
            UInt32 associativity, UInt32 blocksize, UInt32 set_no, CacheSetInfoDRRIP* set_info);
      ~CacheSetDRRIP();

      UInt32 getReplacementIndex(CacheCntlr *cntlr, Core::ExtraMemoryRequestInfo *info, UInt32 &setidx, IntPtr addr);
      void updateReplacementIndex(UInt32 accessed_index, Core::ExtraMemoryRequestInfo *info);

   private:
      const UInt8 m_rrip_numbits;
      const UInt8 m_rrip_max;
      const UInt8 m_srrip_insert;
      const UInt8 m_brrip_insert;
      UInt8* m_rrip_bits;
      UInt8  m_replacement_pointer;
      const UInt32 m_set_no;
      CacheSetInfoDRRIP* m_set_info;
};

#endif /* CACHE_SET_DRRIP_H */
