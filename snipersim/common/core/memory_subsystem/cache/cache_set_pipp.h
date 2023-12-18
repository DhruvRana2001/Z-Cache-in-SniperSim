#ifndef CACHE_SET_PIPP_H
#define CACHE_SET_PIPP_H

#include "cache_set_umon.h"

#include "simulator.h"
#include "core_manager.h"
#include "performance_model.h"
#include "dvfs_manager.h"
#include "random.h"

class CacheSetInfoPIPP : public CacheSetInfoUMON
{
   public:
      CacheSetInfoPIPP(String name, String cfgname,
         core_id_t core_id, UInt32 associativity, UInt32 cache_block_size, UInt32 num_sets, UInt32 shared_cores,
         CacheBase::hash_t hash_function);
	~CacheSetInfoPIPP();
	uint32_t m_associativity;
      UInt64 m_min_insert_pos;
     Lock lock; 
     uint64_t* activity_counters;

};

class CacheSetPIPP : public CacheSetUMON
{
   public:
      CacheSetPIPP(CacheBase::cache_t cache_type,
            UInt32 associativity, UInt32 blocksize, UInt32 set_no, CacheSetInfoPIPP *info);
      ~CacheSetPIPP();

      UInt32 getReplacementIndex(CacheCntlr *cntlr, Core::ExtraMemoryRequestInfo *info, UInt32 &setidx, IntPtr addr);
      void updateReplacementIndex(UInt32 accessed_index, Core::ExtraMemoryRequestInfo *info);

   private:
      void moveToMRU(UInt32 accessed_index);

      UInt8* m_lru_bits;
      UInt32 m_set_no;

      UInt8 m_min_insert;

      CacheSetInfoPIPP* m_info;

      Random *rng;
};

#endif /* CACHE_SET_PIPP_H */
