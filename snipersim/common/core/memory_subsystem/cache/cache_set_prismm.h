#ifndef CACHE_SET_PRISMM_H
#define CACHE_SET_PRISMM_H

//#include "cache_set_umon.h"
#include "cache_set.h"
#include "simulator.h"
#include "core_manager.h"
#include "performance_model.h"
#include "dvfs_manager.h"
#include "config.hpp"
#include "stats.h"


#include "random.h"
class CacheSetPriSMM;
class CacheSetInfoPriSMM;
class pUMON;

class CacheSetInfoPriSMM : public CacheSetInfo
{
public:
    CacheSetInfoPriSMM(String name, String cfgname,
                      core_id_t core_id, UInt32 associativity, UInt32 cache_block_size, UInt32 num_sets, UInt32 shared_cores,
                      CacheBase::hash_t hash_function);
    virtual ~CacheSetInfoPriSMM();
    UInt32 getReplacementTarget();
    bool canEvict(core_id_t);
    bool mustEvict(core_id_t);
    SubsecondTimeCycleConverter* m_time_converter;
    UInt64 m_reallocate_period;
    UInt64 m_current_period;
    uint64_t* setAccessCounters;
    UInt32 getCurrentAllocationPeriod() {
        SubsecondTime time = Sim()->getCoreManager()->getCoreFromID(0)->getPerformanceModel()->getElapsedTime();
        return m_time_converter->subsecondTimeToCycles(time) / m_reallocate_period;
    }

    pUMON** umons;
    UInt32* partAllocations;
    uint32_t l2_size;
void incrementCoreCounter(core_id_t core_id);
    void decrementCoreCounter(core_id_t core_id);
    void incrementHitCounter(core_id_t core_id);
    void incrementMissCounter(core_id_t core_id);

private:
    UInt32 m_num_sets;
    UInt32 m_associativity;
    UInt64 *m_shared_hit_counter;
    UInt64 *m_shared_miss_counter;
    UInt64 *m_core_usage_counter;
    UInt64 m_miss_period_limit;
    
    UInt64 m_miss_counter;

    UInt32 m_blocksize;
    String m_name;
    String m_cfgname;
    core_id_t m_core_id;
 
    UInt32 m_shared_cores;
    UInt32 m_num_blocks;
    Lock lock;
    Lock* corelocks;
    double *m_eviction_prob;
    UInt64 total_replacements;
    UInt64* not_founds;

    Random rng;
    friend class CacheSetPriSMM;
    friend class pUMON;
};

class CacheSetPriSMM : public CacheSet
{
public:
    CacheSetPriSMM(CacheBase::cache_t cache_type,
                  UInt32 associativity, UInt32 blocksize, UInt32 set_no, CacheSetInfoPriSMM *info);
    ~CacheSetPriSMM();

    UInt32 getReplacementIndex(CacheCntlr *cntlr, Core::ExtraMemoryRequestInfo *info, UInt32 &setidx, IntPtr addr);
    void updateReplacementIndex(UInt32 accessed_index, Core::ExtraMemoryRequestInfo *info);

private:
    CacheSetInfoPriSMM *m_info;
    UInt8 *m_lru_bits;
    uint32_t set_no;
    void moveToMRU(UInt32 accessed_index);
    void repartition(bool);
    uint64_t interpolate(uint32_t point, uint32_t core);
    UInt64 getMaxMU(uint32_t allocated, uint32_t balance, uint32_t& extra, uint32_t);
    UInt64 getMU(uint32_t a, uint32_t b, uint32_t);

};

class pUMON {
    // counters gaan per way

public:
    pUMON(CacheSetInfoPriSMM* info, CacheSetInfo*);
    virtual	~pUMON();
    void updateUMON(IntPtr address);
    uint64_t getMisses(uint32_t allocated);
    UInt64* accessCounters;
    UInt64 missCounter;
protected:
    CacheSetInfoPriSMM* m_set_info;
    CacheSetInfo* m_ATD_info;
    CacheSet** m_sets;
    uint32_t m_umon_associativity;
    uint32_t m_umon_num_sets;
    friend class CacheSetPriSMM;
};

#endif /* CACHE_SET_PRISMM_H */
