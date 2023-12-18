#ifndef CACHE_SET_H
#define CACHE_SET_H

#include "fixed_types.h"
#include "cache_block_info.h"
#include "cache_base.h"
#include "lock.h"
#include "random.h"
#include "log.h"
#include "core.h"

#include <cstring>

// Per-cache object to store replacement-policy related info (e.g. statistics),
// can collect data from all CacheSet* objects which are per set and implement the actual replacement policy
class CacheSetInfo
{
   public:
      virtual ~CacheSetInfo() {}

      virtual void updateAccessCounter(UInt32 set_no, core_id_t requester, bool core_hit) {};
};

// Everything related to cache sets
class CacheSet
{
   public:

      static CacheSet* createCacheSet(String cfgname, core_id_t core_id, String replacement_policy, CacheBase::cache_t cache_type, UInt32 associativity, UInt32 blocksize, CacheSetInfo* set_info = NULL, UInt32 set_no = 0, CacheSet** sets = NULL, UInt32 m_log_blocksize=0, UInt32 m_num_sets=0, UInt32 num_cores=0);
      static CacheSetInfo* createCacheSetInfo(String name, String cfgname, core_id_t core_id, String replacement_policy, UInt32 associativity, UInt32 num_sets = 1, UInt32 blocksize = 64, UInt32 shared_cores = 1, CacheBase::hash_t hash_function = CacheBase::HASH_MASK);
      static CacheBase::ReplacementPolicy parsePolicyType(String policy);
      static UInt8 getNumQBSAttempts(CacheBase::ReplacementPolicy, String cfgname, core_id_t core_id);
      CacheBlockInfo** m_cache_block_info_array;
      char* m_blocks;
      UInt32 m_associativity;
      UInt32 m_blocksize;
   protected:
      Lock m_lock;

   public:

      CacheSet(CacheBase::cache_t cache_type,
            UInt32 associativity, UInt32 blocksize);
      virtual ~CacheSet();

      UInt32 getBlockSize() { return m_blocksize; }
      UInt32 getAssociativity() { return m_associativity; }
      Lock& getLock() { return m_lock; }

      void read_line(UInt32 line_index, UInt32 offset, Byte *out_buff, UInt32 bytes, bool update_replacement, Core::ExtraMemoryRequestInfo *info);
      void write_line(UInt32 line_index, UInt32 offset, Byte *in_buff, UInt32 bytes, bool update_replacement, Core::ExtraMemoryRequestInfo *info);
      CacheBlockInfo* find(IntPtr tag, UInt32* line_index = NULL);
      bool invalidate(IntPtr& tag);
      void insert(CacheBlockInfo* cache_block_info, Byte* fill_buff, bool* eviction, CacheBlockInfo* evict_block_info, Byte* evict_buff, IntPtr addr = 0, UInt32 m_num_sets = 0, UInt32 m_log_blocksize = 0, CacheCntlr *cntlr = NULL, Core::ExtraMemoryRequestInfo *info = NULL, CacheSet** setarray = NULL, CacheBase::hash_t hash = CacheBase::HASH_MASK, UInt32 way = 0, UInt32 idx = 0);

      CacheBlockInfo* peekBlock(UInt32 way) const { return m_cache_block_info_array[way]; }

      char* getDataPtr(UInt32 line_index, UInt32 offset = 0);
      UInt32 getBlockSize(void) const { return m_blocksize; }

// make skew-caches overload this function and all should be fine.     
virtual UInt32 getReplacementIndex(CacheCntlr *cntlr, Core::ExtraMemoryRequestInfo *info, UInt32 &setidx, IntPtr addr) = 0;
	UInt32 getReplacementIndex(CacheCntlr *cntlr, Core::ExtraMemoryRequestInfo *info){
		UInt32 idx;
		return getReplacementIndex(cntlr, info, idx, (IntPtr)0);
	}

      virtual void updateReplacementIndex(UInt32, Core::ExtraMemoryRequestInfo *info) = 0;

      bool isValidReplacement(UInt32 index);
};

#endif /* CACHE_SET_H */
