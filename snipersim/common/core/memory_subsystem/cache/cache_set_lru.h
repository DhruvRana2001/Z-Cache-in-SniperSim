#ifndef CACHE_SET_LRU_H
#define CACHE_SET_LRU_H

#include "cache_set.h"
#include "simulator.h"
#include "core_manager.h"
#include "performance_model.h"
#include "dvfs_manager.h"
#include "config.hpp"
#include "stats.h"


class CacheSetInfoLRU : public CacheSetInfo
{
   public:
    SubsecondTimeCycleConverter* m_time_converter;
    UInt64 m_reallocate_period =5000000;
    UInt64 m_current_period;
    Lock* core_locks;
    uint32_t* sizes;
    UInt32 getCurrentAllocationPeriod() {
        SubsecondTime time = Sim()->getCoreManager()->getCoreFromID(0)->getPerformanceModel()->getElapsedTime();
        return m_time_converter->subsecondTimeToCycles(time) / m_reallocate_period;
    }

      CacheSetInfoLRU(String name, String cfgname, core_id_t core_id, UInt32 associativity, UInt8 num_attempts);
      virtual ~CacheSetInfoLRU();
      void increment(UInt32 index)
      {
         LOG_ASSERT_ERROR(index < m_associativity, "Index(%d) >= Associativity(%d)", index, m_associativity);
         ++m_access[index];
      }
      void incrementAttempt(UInt8 attempt)
      {
         if (m_attempts)
            ++m_attempts[attempt];
         else
            LOG_ASSERT_ERROR(attempt == 0, "No place to store attempt# histogram but attempt != 0");
      }
   private:
      const UInt32 m_associativity;
      UInt64* m_access;
      UInt64* m_attempts;
   friend class CacheSetLRU;
};

class CacheSetLRU : public CacheSet
{
   public:
      CacheSetLRU(CacheBase::cache_t cache_type,
            UInt32 associativity, UInt32 blocksize, CacheSetInfoLRU* set_info, UInt8 num_attempts);
      virtual ~CacheSetLRU();

      virtual UInt32 getReplacementIndex(CacheCntlr *cntlr,  Core::ExtraMemoryRequestInfo *info, UInt32 &setidx, IntPtr addr);
      void updateReplacementIndex(UInt32 accessed_index, Core::ExtraMemoryRequestInfo *info);

      UInt8 getLRUBit(UInt32 accessed_index);
   protected:
      const UInt8 m_num_attempts;
      UInt8* m_lru_bits;
      uint8_t* core_bits;
      CacheSetInfoLRU* m_set_info;
      void moveToMRU(UInt32 accessed_index);
};

#endif /* CACHE_SET_LRU_H */
