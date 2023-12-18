#ifndef CACHE_SET_SHADOW_H
#define CACHE_SET_SHADOW_H

#include <unordered_map>

#include "cache_set.h"
#include "simulator.h"
#include "config.hpp"
#include "stats.h"

class CacheSetShadow;

// Set Info class that creates shadow tag directories for each sharing core
class CacheSetInfoShadow : public CacheSetInfo
{
	public:
      CacheSetInfoShadow(String name, String cfgname,
		   core_id_t core_id, UInt32 associativity, UInt32 cache_block_size, UInt32 num_sets, UInt32 shared_cores,
		   CacheBase::hash_t hash_function);
      virtual ~CacheSetInfoShadow();

      UInt64 getAccesses(core_id_t core) {
      	return m_accesses[core];
      };

      UInt64 getMisses(core_id_t core) {
      	return m_misses[core];
      };

      UInt64 getHits(core_id_t core) {
	return getAccesses(core) - getMisses(core);
      };

      UInt64 getAccessCounter(core_id_t core, UInt32 lru_position) {
      	return m_access_counters[core][lru_position];
      };

      void accessCache(core_id_t core, IntPtr address, bool hit);

   protected:
      UInt32 m_shared_cores;
      UInt32 m_associativity;
      Lock* shadowlocks;

      void resetCounters() {
         for(UInt32 core = 0; core < m_shared_cores; core++) {
	    shadowlocks[core].acquire();
            m_accesses[core] = m_misses[core] = 0;
            for(UInt32 way = 0; way < m_associativity; way++) {
               m_access_counters[core][way] = 0;
            }
	    shadowlocks[core].release();
         }
      };

      void halvAccessCounters() {
         for(UInt32 core = 0; core < m_shared_cores; core++) {
            shadowlocks[core].acquire();
            m_accesses[core] = m_misses[core] = 0;
            for(UInt32 way = 0; way < m_associativity; way++) {
               m_access_counters[core][way] >>= 1;
            }
	    shadowlocks[core].release();
         }
      };

   private:

   	  UInt64* m_accesses;
   	  UInt64* m_misses;
   	  UInt64** m_access_counters;

      CacheBase m_cache_base;
      CacheSetInfo *m_set_info;

      std::unordered_map<UInt32, CacheSet*> m_sets;

      UInt32 getSetIndex(core_id_t core_id, UInt32 set_no) {
      	return core_id << 30 | set_no;
      };

      bool isSampledSet(core_id_t core_id, UInt32 set_index) {
      	return m_sets.count(getSetIndex(core_id, set_index));
      };



   	  friend class CacheSetShadow;
};

// Cache Set implementation that will forward all activity to the set info class
class CacheSetShadow : public CacheSet
{
   public:
      CacheSetShadow(CacheBase::cache_t cache_type, UInt32 associativity, UInt32 blocksize, CacheSetInfoShadow *info);
      ~CacheSetShadow();

      UInt32 getReplacementIndex(CacheCntlr *cntlr, Core::ExtraMemoryRequestInfo *info, UInt32 &setidx, IntPtr addr);

      void updateReplacementIndex(UInt32 accessed_index, Core::ExtraMemoryRequestInfo *info);

   private:
   	  CacheSetInfoShadow *m_info;
};

#endif /* CACHE_SET_SHADOW_H */
