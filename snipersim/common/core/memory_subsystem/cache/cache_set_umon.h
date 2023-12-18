#ifndef CACHE_SET_UMON_H
#define CACHE_SET_UMON_H

#include <unordered_map>

#include "cache_set_shadow.h"
#include "simulator.h"
#include "core_manager.h"
#include "performance_model.h"
#include "dvfs_manager.h"
#include "config.hpp"
#include "stats.h"

class CacheSetUMON;

// Extension of the shadow tag set info, adding utility calculations (ucp,pipp) and stream detection (pipp)
class CacheSetInfoUMON : public CacheSetInfoShadow
{
	public:
      CacheSetInfoUMON(String name, String cfgname,
		   core_id_t core_id, UInt32 associativity, UInt32 cache_block_size, UInt32 num_sets, UInt32 shared_cores,
		   CacheBase::hash_t hash_function);
      virtual ~CacheSetInfoUMON();

      void accessCache(core_id_t core, IntPtr address, bool hit);

      // True if the given core is streaming
      bool isStreaming(core_id_t core_id) { return m_enable_stream_detection && m_stream_flags[core_id]; };

      // Total number of cores marked as streaming
      UInt32 countStreamingCores() {
         int count = 0;
         if (m_enable_stream_detection) {
            for(UInt32 i = 0; i < m_shared_cores; i++) {
               count += isStreaming(i);
            }
         }
         return count;
      };

      // True if all cores are marked as streaming
      bool isAllStreaming()  {
         return m_enable_stream_detection && countStreamingCores() == m_shared_cores;
      };

      UInt64 getAllocation(core_id_t core) {
         return m_allocations[core];
      }

   private:
      UInt64 m_reallocate_period;
      bool m_enable_stream_detection;

      UInt64 m_stream_miss_count;
      double m_stream_miss_fraction;


      SubsecondTimeCycleConverter* m_time_converter;
      UInt64 *m_stream_flags;
      UInt64 *m_allocations;
      UInt64 m_current_period;


      UInt32 getCurrentAllocationPeriod() {
         SubsecondTime time = Sim()->getCoreManager()->getCoreFromID(0)->getPerformanceModel()->getElapsedTime();
         return m_time_converter->subsecondTimeToCycles(time) / m_reallocate_period;
      }

      UInt32 getMU(core_id_t core, UInt32 lower, UInt32 upper);
      UInt32 getMaxMU(core_id_t core, UInt32 allocated, int remainding, UInt32 *);

      void logAllocations();

};

// Cache Set implementation that will forward all activity to the set info class
class CacheSetUMON : public CacheSetShadow
{
   public:
      CacheSetUMON(CacheBase::cache_t cache_type, UInt32 associativity, UInt32 blocksize, CacheSetInfoUMON *info);
      ~CacheSetUMON();

      UInt32 getReplacementIndex(CacheCntlr *cntlr, Core::ExtraMemoryRequestInfo *info, UInt32 &setidx, IntPtr addr);

      void updateReplacementIndex(UInt32 accessed_index, Core::ExtraMemoryRequestInfo *info);

   protected:
      CacheSetInfoUMON* getUMON() {
         return m_info;
      }

   private:
   	  CacheSetInfoUMON *m_info;
};

#endif /* CACHE_SET_UMON_H */
