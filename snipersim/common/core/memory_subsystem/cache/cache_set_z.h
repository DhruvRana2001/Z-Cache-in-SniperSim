#ifndef CACHE_SET_Z_H
#define CACHE_SET_Z_H

#include "cache_set.h"
#include "simulator.h"
#include "core_manager.h"
#include "performance_model.h"
#include "dvfs_manager.h"
#include "config.hpp"
#include "stats.h"

class CacheSetZ;
class CacheSetInfoZ;

//#define ASSOC_MEASUREMENTS

// z-cache related defines
#define ZK (0.05f)
// vantage related defines
#define NUM_TRESH (8)
#define C_PERIOD (256)
#define AMAX (1.0f)
#define SLACK (0.1f)

//#define WITHOUT_VANTAGE
//#define WIDE_COUNTERS

class zUMON {
    // counters gaan per way

public:
    zUMON(CacheSetInfoZ* info, CacheSetInfo*);
    virtual	~zUMON();
    void updateUMON(IntPtr address);
    uint64_t getMisses(uint32_t allocated);
    UInt64* accessCounters;
    UInt64 missCounter;
    protected:
    CacheSetInfoZ* m_set_info;
    CacheSetInfo* m_ATD_info;
    CacheSet** m_sets;
    // We can mimick another cache associativity for the ATD's
    // than the Z-cache. Z-caches usually have a low way count
    // making the interpolation of the UMON data very inaccurate
    // compared to UCP which uses higher way counts.
    uint32_t m_umon_associativity;
    uint32_t m_umon_num_sets;
    friend class CacheSetZ;

};

class VantagePartitionState {
public:
    uint8_t currentTs;
    uint32_t accessCounter;
    uint32_t actualSize;
    uint32_t inUnmanaged;
    uint32_t targetSize;
    uint8_t setPointTs;
    uint8_t candsSeen;
    uint8_t candsDemoted;
    uint32_t thrSize[NUM_TRESH];
    uint32_t thrDems[NUM_TRESH];
    Lock lock;

};


class CacheSetInfoZ : public CacheSetInfo
{

public:
    CacheSetInfoZ(String name, String cfgname,
                  core_id_t core_id, UInt32 associativity, UInt32 block_size, UInt32 num_sets, UInt32 shared_cores, UInt32 levels);
    ~CacheSetInfoZ();
    VantagePartitionState** states;
    UInt32* partAllocations;
    zUMON** umons;
    uint32_t* H3_HASH;
    SubsecondTimeCycleConverter* m_time_converter;
    UInt64 m_reallocate_period;
    UInt64 m_current_period;

    UInt32 getCurrentAllocationPeriod() {
        SubsecondTime time = Sim()->getCoreManager()->getCoreFromID(0)->getPerformanceModel()->getElapsedTime();
        return m_time_converter->subsecondTimeToCycles(time) / m_reallocate_period;
    }

    float  FRACTION_UNMANAGED;
    uint32_t l2_size;
    uint32_t* set_accesses;
    UInt32* LUT;
	uint64_t totalReplacements;
    uint64_t faultyReplacements;

protected:
    UInt32 m_num_sets;
    UInt32 m_shared_cores;
    UInt32 m_associativity;
    UInt32 m_blocksize;
    String m_name;
    String m_cfgname;
    core_id_t m_core_id;
    UInt32 candidates_per_way;
    UInt32 L;
    uint64_t core0demoted, core1demoted;
    uint64_t core0periods, core1periods;
    uint32_t nindex_bits;
    bool vantage_enabled;
    bool allocate_l2;
    // Only used
    friend class CacheSetZ;
    friend class zUMON;
};


class CacheSetZ : public CacheSet
{
public:
    CacheSetZ(CacheBase::cache_t cache_type,
              UInt32 associativity, UInt32 blocksize, CacheSetInfoZ* set_info, CacheSet** sets, UInt32 num_sets, UInt32 m_log_blocksize, UInt32 levels, UInt32 setn, UInt32 n_cores);
    virtual	~CacheSetZ();
    static UInt32 hashedIndex(IntPtr addr, UInt32 way, UInt32 m_log_blocksize);
    virtual  UInt32 getReplacementIndex(CacheCntlr *cntlr, Core::ExtraMemoryRequestInfo *info, UInt32 &setidx, IntPtr addr);
    virtual  UInt32 getReplacementIndex(CacheCntlr *cntlr, Core::ExtraMemoryRequestInfo *info, UInt32 &setidx, IntPtr addr, bool);
    void updateReplacementIndex(UInt32 accessed_index, Core::ExtraMemoryRequestInfo *info);
    void doWalk(UInt32* tmp, bool*, IntPtr addr, UInt32 m_log_blocksize);
    void doReplacement(UInt32* walk, bool*, UInt32 way, UInt32 idx, bool* eviction, CacheBlockInfo* evict_block_info, Byte* evict_buff, UInt32);
    UInt32   getReplacementIndex(CacheCntlr *cntlr, Core::ExtraMemoryRequestInfo *info, UInt32 &way, IntPtr addr, UInt32& candidate, UInt32* walk, bool*, UInt32& iidx, UInt32* backtrack, UInt32& pathlenght);
    UInt32 getLevels(void) {
        return L;
    }
    uint32_t getVantageReplacementIndex(uint32_t* walk, bool* walkv, uint32_t& candidate, uint32_t& iidx, uint32_t& eway, Core::ExtraMemoryRequestInfo *info);
    Lock* getCounterLock() {
        return &counter_lock;
    }
    void repartition(bool);
    bool vantageEnabled() {
        return m_set_info->vantage_enabled;
    }
    CacheSetInfoZ* getZInfo() {
        return m_set_info;
    }
    UInt64** interpolate(void);
#ifndef WIDE_COUNTERS
    uint8_t* timestamps;
    uint8_t counter;
#else
    uint64_t* timestamps;
    uint64_t counter;
#endif

#ifdef ASSOC_MEASUREMENTS
    uint32_t* boek;
    uint64_t* tijden;
    uint64_t globale_tijd;
    Lock globale_lock;
    uint32_t priority(uint64_t tijd);
#endif
    UInt32* vPartitions;
    uint32_t* vPartitions2;
    uint32_t* core_tag;
    uint32_t accessCounter;
    UInt32 candidates_per_way;
    /* TODO: those variables are common to each set. Move them to info class */
    CacheSet** setarray;

    CacheSetInfoZ* m_set_info;
protected:
    uint32_t plru_period;
    Lock counter_lock;
    const UInt32 m_num_sets;
    const UInt32 m_log_blocksize;
    const UInt32 L;
    const UInt32 set_no;
    const UInt32 m_num_cores;
    uint64_t interpolate(uint32_t point, uint32_t core);
    UInt64 getMaxMU(uint32_t allocated, uint32_t balance, uint32_t& extra, uint32_t);
    UInt64 getMU(uint32_t a, uint32_t b, uint32_t);

};

#endif /* CACHE_SET_LRU_H */
