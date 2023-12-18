#include "cache_set.h"
#include "cache_set_lru.h"
#include "cache_set_mru.h"
#include "cache_set_nmru.h"
#include "cache_set_nru.h"
#include "cache_set_plru.h"
#include "cache_set_random.h"
#include "cache_set_round_robin.h"
#include "cache_set_srrip.h"
#include "cache_set_tadip.h"
#include "cache_set_ucp.h"
#include "cache_set_pipp.h"
#include "cache_set_drrip.h"
#include "cache_set_shadow.h"
#include "cache_set_prism.h"
#include "cache_set_prismm.h"
#include "cache_set_skew.h"
#include "cache_set_z.h"
#include "cache_base.h"
#include "log.h"
#include "simulator.h"
#include "config.h"
#include "config.hpp"

/* UNCOMMENT IF REQUIRED */
#define printf(...) { ; }
#define getchar() { ; }
#define fprintf(...) { ; }
CacheSet::CacheSet(CacheBase::cache_t cache_type,
                   UInt32 associativity, UInt32 blocksize):
    m_associativity(associativity), m_blocksize(blocksize)
{
    m_cache_block_info_array = new CacheBlockInfo*[m_associativity];
    for (UInt32 i = 0; i < m_associativity; i++)
    {
        m_cache_block_info_array[i] = CacheBlockInfo::create(cache_type);
    }

    if (Sim()->getFaultinjectionManager())
    {
        m_blocks = new char[m_associativity * m_blocksize];
        memset(m_blocks, 0x00, m_associativity * m_blocksize);
    } else {
        m_blocks = NULL;
    }
}

CacheSet::~CacheSet()
{
    for (UInt32 i = 0; i < m_associativity; i++)
        delete m_cache_block_info_array[i];
    delete [] m_cache_block_info_array;
    delete [] m_blocks;
}

void
CacheSet::read_line(UInt32 line_index, UInt32 offset, Byte *out_buff, UInt32 bytes, bool update_replacement, Core::ExtraMemoryRequestInfo *info)
{
    assert(offset + bytes <= m_blocksize);
    //assert((out_buff == NULL) == (bytes == 0));

    if (out_buff != NULL && m_blocks != NULL)
        memcpy((void*) out_buff, &m_blocks[line_index * m_blocksize + offset], bytes);

    if (update_replacement)
        updateReplacementIndex(line_index, info);
}

void
CacheSet::write_line(UInt32 line_index, UInt32 offset, Byte *in_buff, UInt32 bytes, bool update_replacement, Core::ExtraMemoryRequestInfo *info)
{
    assert(offset + bytes <= m_blocksize);
    //assert((in_buff == NULL) == (bytes == 0));

    if (in_buff != NULL && m_blocks != NULL)
        memcpy(&m_blocks[line_index * m_blocksize + offset], (void*) in_buff, bytes);

    if (update_replacement)
        updateReplacementIndex(line_index, info);
}

CacheBlockInfo*
CacheSet::find(IntPtr tag, UInt32* line_index)
{   
    //fprintf(stdout, "Line Index %d \n", p->actualSize);
    for (SInt32 index = m_associativity-1; index >= 0; index--)
    {
        if (m_cache_block_info_array[index]->getTag() == tag)
        {
            if (line_index != NULL)
                *line_index = index;
            return (m_cache_block_info_array[index]);
        }
    }
    return NULL;
}


#if 0
CacheBlockInfo*
CacheSet::find(IntPtr addr, CacheSet** sets)
{
    for (SInt32 index = m_associativity-1; index >= 0; index--)
    {
        if (m_cache_block_info_array[index]->getTag() == tag)
        {
            if (line_index != NULL)
                *line_index = index;
            return (m_cache_block_info_array[index]);
        }
    }
    return NULL;
}
#endif

bool
CacheSet::invalidate(IntPtr& tag)
{
    for (SInt32 index = m_associativity-1; index >= 0; index--)
    {
        if (m_cache_block_info_array[index]->getTag() == tag)
        {
            m_cache_block_info_array[index]->invalidate();
            return true;
        }
    }
    return false;
}

void
CacheSet::insert(CacheBlockInfo* cache_block_info, Byte* fill_buff, bool* eviction, CacheBlockInfo* evict_block_info, Byte* evict_buff, IntPtr addr, UInt32 m_num_sets, UInt32 m_log_blocksize, CacheCntlr *cntlr, Core::ExtraMemoryRequestInfo *info, CacheSet** setarray, CacheBase::hash_t hash, UInt32 way, UInt32 idx)
{
    // This replacement strategy does not take into account the fact that
    // cache blocks can be voluntarily flushed or invalidated due to another write request
    UInt32 setidx;
    if(hash==CacheBase::HASH_Z)
        printf("CACHESET INSERT NU block met TAG %lx at index %x in way %x\n", addr >> m_log_blocksize, idx, way);
        UInt32 index = 0;
    if(hash != CacheBase::HASH_Z) {
        index = getReplacementIndex(cntlr, info, setidx, addr);
    }
    else {
        // we already have our index and set. Update the counter
        CacheSetZ* set = dynamic_cast<CacheSetZ*>(this);
        // also set the partitionID
        // this needs a trace file and a Z-level LLC otherwhise info might be null
        CacheSetZ* properSet = dynamic_cast<CacheSetZ*>(set->setarray[idx]);
        assert(info);
        if(properSet->vantageEnabled()) {
            properSet->vPartitions[way] = info->core_id;
            VantagePartitionState* p = properSet->getZInfo()->states[info->core_id];


            p->lock.acquire();
            fprintf(stdout, "size before %d\n", p->actualSize);
            p->actualSize++;
            fprintf(stdout, "Inserted entry with partition %d, size %d and tag %lx\n", info->core_id, p->actualSize, addr >> m_log_blocksize);
            p->lock.release();
            getchar();
        }
        properSet->updateReplacementIndex(way, info);
    }
    // Update replacement index for z-cache
    assert(index < m_associativity);

// Tomwi: TODO: tidy up this eviction skip for zcache
    if(eviction == NULL && evict_buff == NULL && evict_block_info == NULL) {
    }
    else {
        assert(eviction != NULL);

        if(hash == CacheBase::HASH_MASK) {
            /* TODO: select correct cache_set object from list */
            if (m_cache_block_info_array[index]->isValid())
            {
                *eviction = true;
                // FIXME: This is a hack. I dont know if this is the best way to do
                evict_block_info->clone(m_cache_block_info_array[index]);
                if (evict_buff != NULL && m_blocks != NULL)
                    memcpy((void*) evict_buff, &m_blocks[index * m_blocksize], m_blocksize);
            }
            else
            {
                *eviction = false;
            }
        }
        else if(hash == CacheBase::HASH_SKEW) {
            CacheSetSkew* set = dynamic_cast<CacheSetSkew*>(this);
            CacheSetSkew* properSet = dynamic_cast<CacheSetSkew*>(set->setarray[setidx]);

            if (properSet->m_cache_block_info_array[index]->isValid())
            {
                *eviction = true;
                // FIXME: This is a hack. I dont know if this is the best way to do
                evict_block_info->clone(properSet->m_cache_block_info_array[index]);
                if (evict_buff != NULL && m_blocks != NULL)
                    memcpy((void*) evict_buff, &properSet->m_blocks[index * m_blocksize], m_blocksize);
            }
            else
            {
                *eviction = false;
            }
        }
    }
    // FIXME: This is a hack. I dont know if this is the best way to do
    if(hash == CacheBase::HASH_MASK) {
        m_cache_block_info_array[index]->clone(cache_block_info);
        if (fill_buff != NULL && m_blocks != NULL) {
            memcpy(&m_blocks[index * m_blocksize], (void*) fill_buff, m_blocksize);
        }
    }
    else if(hash == CacheBase::HASH_SKEW) {
        //printf("SKEW insertion\n");
        CacheSetSkew* set = dynamic_cast<CacheSetSkew*>(this);
        CacheSetSkew* properSet = dynamic_cast<CacheSetSkew*>(set->setarray[setidx]);
        properSet->m_cache_block_info_array[index]->clone(cache_block_info);
        if (fill_buff != NULL && properSet->m_blocks != NULL)
            memcpy(&properSet->m_blocks[index * m_blocksize], (void*) fill_buff, m_blocksize);
    }
    else if(hash == CacheBase::HASH_Z) {
        CacheSetZ* set = dynamic_cast<CacheSetZ*>(this);
        CacheSetZ* properSet = dynamic_cast<CacheSetZ*>(set->setarray[idx]);
        properSet->m_cache_block_info_array[way]->clone(cache_block_info);
        if (fill_buff != NULL && properSet->m_blocks != NULL) {
            memcpy(&properSet->m_blocks[way * m_blocksize], (void*) fill_buff, m_blocksize);
        }

    }
}

char*
CacheSet::getDataPtr(UInt32 line_index, UInt32 offset)
{
    return &m_blocks[line_index * m_blocksize + offset];
}

CacheSet*
CacheSet::createCacheSet(String cfgname, core_id_t core_id,
                         String replacement_policy,
                         CacheBase::cache_t cache_type,
                         UInt32 associativity, UInt32 blocksize, CacheSetInfo* set_info, UInt32 set_no, CacheSet** sets, UInt32 m_log_blocksize, UInt32 m_num_sets, UInt32 num_cores)
{
    CacheBase::ReplacementPolicy policy = parsePolicyType(replacement_policy);
    switch(policy)
    {
    case CacheBase::ROUND_ROBIN:
        return new CacheSetRoundRobin(cache_type, associativity, blocksize);

    case CacheBase::LRU:
    case CacheBase::LRU_QBS:
        return new CacheSetLRU(cache_type, associativity, blocksize, dynamic_cast<CacheSetInfoLRU*>(set_info), getNumQBSAttempts(policy, cfgname, core_id));

    case CacheBase::NRU:
        return new CacheSetNRU(cache_type, associativity, blocksize);

    case CacheBase::MRU:
        return new CacheSetMRU(cache_type, associativity, blocksize);

    case CacheBase::NMRU:
        return new CacheSetNMRU(cache_type, associativity, blocksize);

    case CacheBase::PLRU:
        return new CacheSetPLRU(cache_type, associativity, blocksize);

    case CacheBase::SRRIP:
    case CacheBase::SRRIP_QBS:
        return new CacheSetSRRIP(cfgname, core_id, cache_type, associativity, blocksize, dynamic_cast<CacheSetInfoLRU*>(set_info), getNumQBSAttempts(policy, cfgname, core_id));

    case CacheBase::DRRIP:
        return new CacheSetDRRIP(cfgname, core_id, cache_type, associativity, blocksize, set_no, dynamic_cast<CacheSetInfoDRRIP*>(set_info));

    case CacheBase::RANDOM:
        return new CacheSetRandom(cache_type, associativity, blocksize);

    case CacheBase::TADIP:
        return new CacheSetTADIP(cache_type, associativity, blocksize, set_no, dynamic_cast<CacheSetInfoTADIP*>(set_info));

    case CacheBase::UCP:
        return new CacheSetUCP(cache_type, associativity, blocksize, set_no, dynamic_cast<CacheSetInfoUMON*>(set_info));

    case CacheBase::PIPP:
        return new CacheSetPIPP(cache_type, associativity, blocksize, set_no, dynamic_cast<CacheSetInfoPIPP*>(set_info));

    case CacheBase::PriSM:
        return new CacheSetPriSM(cache_type, associativity, blocksize, set_no, dynamic_cast<CacheSetInfoPriSM*>(set_info));

    case CacheBase::PriSMM:
        return new CacheSetPriSMM(cache_type, associativity, blocksize, set_no, dynamic_cast<CacheSetInfoPriSMM*>(set_info));

    case CacheBase::SKEW:
        return new CacheSetSkew(cache_type, associativity, blocksize, dynamic_cast<CacheSetInfoSkew*>(set_info), sets, m_num_sets, m_log_blocksize, set_no, num_cores);
    // TODO: fix level argument.
    case CacheBase::Z:
        return new CacheSetZ(cache_type, associativity, blocksize, dynamic_cast<CacheSetInfoZ*>(set_info), sets, m_num_sets, m_log_blocksize, 3, set_no, num_cores) ;
    default:
        LOG_PRINT_ERROR("Unrecognized Cache Replacement Policy: %i",
                        policy);
        break;
    }

    return (CacheSet*) NULL;
}

CacheSetInfo*
CacheSet::createCacheSetInfo(String name, String cfgname, core_id_t core_id, String replacement_policy, UInt32 associativity, UInt32 num_sets, UInt32 blocksize, UInt32 shared_cores, CacheBase::hash_t hash_function)
{
    CacheBase::ReplacementPolicy policy = parsePolicyType(replacement_policy);
    switch(policy)
    {
    case CacheBase::LRU:
    case CacheBase::LRU_QBS:
    case CacheBase::SRRIP:
    case CacheBase::SRRIP_QBS:
        return new CacheSetInfoLRU(name, cfgname, core_id, associativity, getNumQBSAttempts(policy, cfgname, core_id));
    case CacheBase::DRRIP:
        return new CacheSetInfoDRRIP(name, cfgname, core_id, associativity, num_sets);
    case CacheBase::TADIP:
        return new CacheSetInfoTADIP(name, cfgname, core_id, associativity, num_sets);
    case CacheBase::UCP:
        return new CacheSetInfoUMON(name, cfgname, core_id, associativity, blocksize, num_sets, shared_cores, hash_function);
    case CacheBase::PIPP:
        return new CacheSetInfoPIPP(name, cfgname, core_id, associativity, blocksize, num_sets, shared_cores, hash_function);
    case CacheBase::PriSM:
        return new CacheSetInfoPriSM(name, cfgname, core_id, associativity, blocksize, num_sets, shared_cores, hash_function);
    case CacheBase::PriSMM:
        return new CacheSetInfoPriSMM(name, cfgname, core_id, associativity, blocksize, num_sets, shared_cores, hash_function);

    // I should make proper use of the info class, but meh, not necessary....
    case CacheBase::SKEW:
        return new CacheSetInfoLRU(name, cfgname, core_id, associativity, getNumQBSAttempts(policy, cfgname, core_id));
    case CacheBase::Z:
        return new CacheSetInfoZ(name, cfgname, core_id, associativity, blocksize, num_sets, shared_cores, 3);
    default:
        return NULL;
    }
}

UInt8
CacheSet::getNumQBSAttempts(CacheBase::ReplacementPolicy policy, String cfgname, core_id_t core_id)
{
    switch(policy)
    {
    case CacheBase::LRU_QBS:
    case CacheBase::SRRIP_QBS:
        return Sim()->getCfg()->getIntArray(cfgname + "/qbs/attempts", core_id);
    default:
        return 1;
    }
}

CacheBase::ReplacementPolicy
CacheSet::parsePolicyType(String policy)
{
    if (policy == "round_robin")
        return CacheBase::ROUND_ROBIN;
    if (policy == "lru")
        return CacheBase::LRU;
    if (policy == "lru_qbs")
        return CacheBase::LRU_QBS;
    if (policy == "nru")
        return CacheBase::NRU;
    if (policy == "mru")
        return CacheBase::MRU;
    if (policy == "nmru")
        return CacheBase::NMRU;
    if (policy == "plru")
        return CacheBase::PLRU;
    if (policy == "srrip")
        return CacheBase::SRRIP;
    if (policy == "srrip_qbs")
        return CacheBase::SRRIP_QBS;
    if (policy == "random")
        return CacheBase::RANDOM;
    if (policy == "tadip")
        return CacheBase::TADIP;
    if (policy == "ucp")
        return CacheBase::UCP;
    if (policy == "pipp")
        return CacheBase::PIPP;
    if (policy == "drrip")
        return CacheBase::DRRIP;
    if (policy == "prism")
        return CacheBase::PriSM;
     if (policy == "prismm")
        return CacheBase::PriSMM;

    if (policy == "skew")
        return CacheBase::SKEW;
    if (policy == "z")
        return CacheBase::Z;
    LOG_PRINT_ERROR("Unknown replacement policy %s", policy.c_str());
}

bool CacheSet::isValidReplacement(UInt32 index)
{
    if (m_cache_block_info_array[index]->getCState() == CacheState::SHARED_UPGRADING)
    {
        return false;
    }
    else
    {
        return true;
    }
}
