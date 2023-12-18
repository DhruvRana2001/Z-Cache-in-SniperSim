#ifndef CACHE_SET_PRISM_H
#define CACHE_SET_PRISM_H

#include "cache_set_shadow.h"
#include "random.h"


class CacheSetInfoPriSM : public CacheSetInfoShadow
{
public:
    CacheSetInfoPriSM(String name, String cfgname,
                      core_id_t core_id, UInt32 associativity, UInt32 cache_block_size, UInt32 num_sets, UInt32 shared_cores,
                      CacheBase::hash_t hash_function);
    virtual ~CacheSetInfoPriSM();

    void accessCache(core_id_t core, IntPtr address, bool hit);

    void incrementCoreCounter(core_id_t core_id);
    void decrementCoreCounter(core_id_t core_id);
    void incrementHitCounter(core_id_t core_id);
    void incrementMissCounter(core_id_t core_id);

    UInt32 getReplacementTarget();
    bool canEvict(core_id_t);
    bool mustEvict(core_id_t);


private:
    UInt64 *m_shared_hit_counter;
    UInt64 *m_shared_miss_counter;
    UInt64 *m_core_usage_counter;
    UInt64 m_core_total_usage;
    UInt64 m_miss_counter;
    UInt64 m_miss_period_limit;
    UInt32 m_shared_cores;
    UInt32 m_num_blocks;
    Lock lock;
    Lock* corelocks;
    double *m_eviction_prob;
    UInt64 total_replacements;
    UInt64 not_founds;
    Random rng;

friend class CacheSetPriSM;

};

class CacheSetPriSM : public CacheSetShadow
{
public:
    CacheSetPriSM(CacheBase::cache_t cache_type,
                  UInt32 associativity, UInt32 blocksize, UInt32 set_no, CacheSetInfoPriSM *info);
    ~CacheSetPriSM();

    UInt32 getReplacementIndex(CacheCntlr *cntlr, Core::ExtraMemoryRequestInfo *info, UInt32 &setidx, IntPtr addr);
    void updateReplacementIndex(UInt32 accessed_index, Core::ExtraMemoryRequestInfo *info);

private:
    CacheSetInfoPriSM *m_info;

    UInt8 *m_lru_bits;

    void moveToMRU(UInt32 accessed_index);
};

#endif /* CACHE_SET_PRISM_H */
