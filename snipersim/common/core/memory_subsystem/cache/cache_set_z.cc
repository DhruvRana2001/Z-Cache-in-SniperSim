#include <limits>
#include <random>

#include "cache_set_z.h"
#include "cache_set_lru.h"
#include "pr_l1_cache_block_info.h"
//#include "h3.h"
#include "log.h"
#include "stats.h"
#include "utils.h"

//#define DSS_ENABLED

uint32_t* H3_HASH;

zUMON::zUMON(CacheSetInfoZ* info, CacheSetInfo* ATDinfo) :
    m_set_info(info),
    m_umon_associativity(Sim()->getCfg()->getIntArray(m_set_info->m_cfgname + "/zcache/umon_associativity", m_set_info->m_core_id))
{
    // UMON's with a lower way count than the real cache are not allowed
    assert(m_umon_associativity >= m_set_info->m_associativity);
    m_umon_num_sets = m_set_info->m_num_sets / (m_umon_associativity/m_set_info->m_associativity);
    m_sets = new CacheSet*[m_umon_num_sets];
    accessCounters = new uint64_t[m_umon_associativity];

    for(unsigned int i=0; i<m_umon_associativity; i++) {
        accessCounters[i] = 0;
    }
    missCounter = 0;

    m_ATD_info = ATDinfo;

    for(uint32_t i=0; i<m_umon_num_sets; i++) {
        m_sets[i] = CacheSet::createCacheSet(m_set_info->m_name, m_set_info->m_core_id, "lru", CacheBase::PR_L1_CACHE, m_umon_associativity, 0, m_ATD_info);
    }
}

uint64_t zUMON::getMisses(uint32_t allocated) {
    uint64_t misses = 0;
    // clamp
    if(allocated > m_umon_associativity)
        allocated = m_umon_associativity;

    for(uint32_t i=0; i<m_umon_associativity; i++) {
        if(allocated < (i+1))
            misses += accessCounters[i];
    }
    return misses;
}

zUMON::~zUMON() {
    /* TODO: proper cleanup */
}

/* TODO: fix the hardcoded 64 byte blocksize */
void zUMON::updateUMON(IntPtr address) {
    // If DSS is enabled, we sample 64 sets. Here we determine whether the set corresponding to the address is
    // sampled or not.
    IntPtr tag = address >> 6;
#ifdef DSS_ENABLED
    IntPtr stepSize = m_umon_num_sets/64;
    if((tag & (stepSize-1))!=0)
        return;
#endif
    uint32_t line_index;
    uint32_t set_index = tag & (m_umon_num_sets -1);
    bool found = m_sets[set_index]->find(tag, &line_index) != NULL;
    /* YAY a hit, so it was already there. Increment hit-counter */
    if(found) {
        CacheSetLRU* lru_set = dynamic_cast<CacheSetLRU*>(m_sets[set_index]);
        uint32_t lru_bits = lru_set->getLRUBit(line_index);
#ifdef DSS_ENABLED
        accessCounters[lru_bits] += m_umon_num_sets/64;
#else
        accessCounters[lru_bits]++;
#endif
        lru_set->updateReplacementIndex(line_index, 0);
    }
    /* A miss. Insert in the ATD */
    else {
        missCounter++;
        PrL1CacheBlockInfo* cache_block_info = new PrL1CacheBlockInfo(tag, CacheState::MODIFIED);
        bool eviction;
        PrL1CacheBlockInfo evict_block_info;
        m_sets[set_index]->insert(cache_block_info, NULL, &eviction, &evict_block_info, NULL);
        delete cache_block_info;
    }
}


static inline uint32_t power(uint32_t val, uint32_t pow) {
    // can be more optimized but val will be small
    uint32_t ret = 1;
    uint32_t acc = 1;
    for(uint32_t i = 0; i<pow; i++) {
        ret*=val;
        acc+=ret;
    }
    return acc;
}

void readH3Hash(const char* name, uint32_t* buf, uint32_t bufsize) {
#ifdef DEBUG
    printf("opening %s\n", name);
#endif
    FILE* fp = fopen(name, "rt");
    assert(fp!=NULL);
    for(uint32_t i=0; i<bufsize; i++) {
        fscanf(fp, "%x", buf++);
#ifdef DEBUG
        printf("%x\n", buf[-1]);
#endif
    }
#ifdef DEBUG
    printf("read %d hash values\n", bufsize);
#endif
    fclose(fp);
}

CacheSetZ::CacheSetZ(CacheBase::cache_t cache_type,
                     uint32_t associativity, uint32_t blocksize, CacheSetInfoZ* set_info, CacheSet** sets, uint32_t num_sets, uint32_t logblk, uint32_t levels, uint32_t setn, uint32_t num_cores)
    : CacheSet(cache_type, associativity, blocksize),
      m_set_info(set_info),
      m_num_sets(num_sets),
      m_log_blocksize(logblk),
      L(set_info->L),
      set_no(setn),
      m_num_cores(num_cores)
{
    H3_HASH = m_set_info->H3_HASH;
    assert(m_num_sets >=64);
    setarray = sets;
    candidates_per_way = power(m_associativity-1, L-1);
    plru_period = m_num_sets*m_associativity*ZK;
    if(set_no == 0) {
        counter = 0;
        accessCounter = 0;
        fprintf(stdout, "pseudo-LRU period %u %d\n", plru_period, (uint8_t)(0-1));
        if(vantageEnabled())
            repartition(true);
        printf("ZCache configured with %d ways, %d levels, resulting in %d replacement candidates\n", m_associativity, L, candidates_per_way*m_associativity);
    }
#ifndef WIDE_COUNTERS
    timestamps = new uint8_t[m_associativity];
#else
    timestamps = new uint64_t[m_associativity];
#endif
    vPartitions = new uint32_t[m_associativity];
    vPartitions2 = new uint32_t[m_associativity];

    core_tag = new uint32_t[m_associativity];
#ifdef ASSOC_MEASUREMENTS
    tijden = new uint64_t[m_associativity];

    if(set_no==0) {
        boek = new uint32_t[m_associativity*m_num_sets];
        globale_tijd = 0;
    }
#endif
    for(uint32_t i=0; i<m_associativity; i++) {
        timestamps[i] = 0;
#ifdef ASSOC_MEASUREMENTS
        tijden[i] = 0;
	core_tag[i] = 0xff;
#endif
    }
}


CacheSetZ::~CacheSetZ()
{
// TODO: proper-cleanup
}

uint32_t CacheSetZ::hashedIndex(IntPtr tag, uint32_t way, uint32_t m_log_blocksize) {
    IntPtr tmp_addr = tag;
    uint32_t idx = 0;


    for(uint32_t i=0; i<64-(m_log_blocksize); i++) {
        
        idx ^= uint32_t(H3_HASH[way*(64-(m_log_blocksize))+i])*uint32_t(tmp_addr & 0x1);
        tmp_addr >>= 1;

    }
    
    return idx;
}

#ifdef ASSOC_MEASUREMENTS
uint32_t CacheSetZ::priority(uint64_t tijd) {
    uint32_t priority = m_num_sets*m_associativity-1;

    for(uint32_t i=0; i<m_num_sets; i++) {
        CacheSetZ* tmp = dynamic_cast<CacheSetZ*>(setarray[i]);
        for(uint32_t j=0; j<m_associativity; j++) {
            if(tijd > tmp->tijden[j])
                priority--;
        }
    }
//	printf("%d %d\n", priority, dynamic_cast<CacheSetZ*>(setarray[0])->globale_tijd);
    return priority;
}
#endif

// Does the replacement and eviction.
// way is the way corresponding to the hashed replacement candidates address
// idx is the index of the replacement candidate in the _walk_ array
//
// This is also the place were we can update our replacement candidates' partitions
// for vantage. Note that this is not doable during getReplacmentIndex as we don't
// have the necessary locks on our path
void CacheSetZ::doReplacement(uint32_t* walk, bool* walkv, uint32_t way, uint32_t idx, bool* eviction, CacheBlockInfo* evict_block_info, Byte* evict_buff, uint32_t core_id) {
    // Our first backtrace index
    uint32_t walkidx = candidates_per_way*way + idx;
    // This is the set corresponding to eviction entry
    CacheSetZ* prev = dynamic_cast<CacheSetZ*>(setarray[walk[walkidx]]);
#ifdef ASSOC_MEASUREMENTS
    CacheSetZ* root = dynamic_cast<CacheSetZ*>(setarray[0]);
    root->globale_lock.acquire();
    root->boek[priority(prev->tijden[way])]++;
    root->globale_lock.release();
#endif
    assert(prev->isValidReplacement(way));
    if (prev->m_cache_block_info_array[way]->isValid())
    {
        *eviction = true;
        // FIXME: This is a hack. I dont know if this is the best way to do
        evict_block_info->clone(prev->m_cache_block_info_array[way]);

        if (evict_buff != NULL && m_blocks != NULL)
            memcpy((void*) evict_buff, &prev->m_blocks[way * m_blocksize], m_blocksize);
    }
    else
    {
        *eviction = false;
    }

    for(uint32_t i =0; i<L-1; i++) {
        // This is the next backtracked entry
        uint32_t oldwalkidx = walkidx;
        walkidx = m_set_info->LUT[walkidx];
        uint32_t curway = walkidx/candidates_per_way;

        if(curway == way && walk[walkidx] == walk[oldwalkidx]) {
            break;
        }

        CacheSetZ* cur = dynamic_cast<CacheSetZ*>(setarray[walk[walkidx]]);
        if(cur->m_cache_block_info_array[curway]->getTag() != ((IntPtr) ~0)) {
            // replace the line
            prev->write_line(way, 0, (Byte*)(cur->m_blocks+m_blocksize*curway), m_blocksize, false, NULL);
            // clone the cacheblock info
            prev->m_cache_block_info_array[way]->clone(cur->m_cache_block_info_array[curway]);
            // clone the timestamp
            prev->timestamps[way] = cur->timestamps[curway];
#ifdef ASSOC_MEASUREMENTS
            prev->tijden[way] = cur->tijden[curway];
#endif
            // in case of vantage also copy the vantage partition
            if(vantageEnabled()) {
	    	prev->core_tag[way] = cur->core_tag[curway];
                prev->vPartitions[way] = cur->vPartitions[curway];
                prev->vPartitions2[way] = cur->vPartitions2[curway];

            }
        }
        // go on
        way = curway;
        prev = cur;
    }
}

void CacheSetZ::doWalk(uint32_t* tmp, bool* valid, IntPtr addr, uint32_t m_log_blocksize) {
    IntPtr tag = addr >> m_log_blocksize;
    // Hash the tag. This will always result in valid replacement candidates
    for(uint32_t i = 0; i<m_associativity; i++) {
        tmp[i*candidates_per_way] = hashedIndex(tag, i, m_log_blocksize);
        valid[i*candidates_per_way] = true;
    }

    // Number of candidates to be rehashed in the second level
    uint32_t candidates = 1;
    // Number of replacement candidates already present per way
    uint32_t acc_candidates = 1;
    // Number of replacement candidates present in the previous level
    uint32_t acc_prev_candidates = 0;

    // We already did the first trivial level, here we do the next L-1 levels.
    for(uint32_t i = 0; i<(L-1); i++) {
        // For each way
        for(uint32_t j=0; j<m_associativity; j++) {
            // Get the pointers to the first available place to put
            // candidates in way j in.
            uint32_t* wayptr = &tmp[j*candidates_per_way+acc_candidates];
            bool* wayptrv = &valid[j*candidates_per_way+acc_candidates];

            // For every way but j
            for(uint32_t k = 0; k<j; k++) {
                // Find the candidates produced in the previous run in way k
                uint32_t* wrkptr = &tmp[k*candidates_per_way+acc_prev_candidates];
                bool* wrkptrv = &valid[k*candidates_per_way+acc_prev_candidates];
                // For all the candidates produced
                for(uint32_t l = 0; l<candidates; l++) {
                    uint32_t input = wrkptr[l];
                    if(wrkptrv[l]==false) {
                        *wayptrv++ = false;
                        *wayptr++ = 0xffffffff;
                        continue;
                    }
                    // get the tag corresponding to the replacement candidate in way k
                    IntPtr tmpTag = setarray[input]->m_cache_block_info_array[k]->getTag();
                    // our parent was an invalid entry, which are the best replacement entries. Stop here.
                    if(tmpTag == ((IntPtr) ~0)) {
                        *wayptrv++ = false;
                        *wayptr++ = 0xffffffff;
                    }
                    else {
                        uint32_t toInsert = hashedIndex(tmpTag, j, m_log_blocksize);
                        // When selecting a replacement candidate, it is important that our path
                        // does not contain any cycles. This seems pathetic but isn't.
                        // IMPORTANT: this only prevents doubles in a PATH!
                        // not doubles on different paths
                        // TODO: now it will prevent doubles globally. Maybe not be so strict?
                        for(uint32_t meh=0; meh<(acc_candidates+k*candidates+l); meh++) {
                            if(tmp[j*candidates_per_way+meh]==toInsert) {
                                *wayptr++ = 0xffffffff;
                                *wayptrv++ = false;
                                goto smerig;
                            }
                        }
                        *wayptr++ = toInsert;
                        // If the previous entry was invalid this one is invalid too
                        *wayptrv++ = true;
smerig:
                        continue;
                    }
                }
            }

            for(uint32_t k = (j+1); k<m_associativity; k++) {
                // Find the candidates produced in the previous run in way k
                uint32_t* wrkptr = &tmp[k*candidates_per_way+acc_prev_candidates];
                bool* wrkptrv = &valid[k*candidates_per_way+acc_prev_candidates];
                // For all the candidates produced
                for(uint32_t l = 0; l<candidates; l++) {
                    uint32_t input = wrkptr[l];
                    if(wrkptrv[l]==false) {
                        *wayptrv++ = false;
                        *wayptr++ = 0xffffffff;
                        continue;
                    }
                    // get the tag corresponding to the replacement candidate in way k
                    IntPtr tmpTag = setarray[input]->m_cache_block_info_array[k]->getTag();

                    if(tmpTag == ((IntPtr) ~0)) {
                        *wayptrv++ = false;
                        *wayptr++ = 0xffffffff;
                    }
                    else {
                        uint32_t toInsert = hashedIndex(tmpTag, j, m_log_blocksize);
                        // IMPORTANT: the -1 should only be here as there are only m_associativity-1
                        // ways from which new canddates are produced.
                        for(uint32_t meh=0; meh<(acc_candidates+k*candidates+l-1); meh++) {
                            if(tmp[j*candidates_per_way+meh]==toInsert) {
                                *wayptr++ = 0xffffffff;
                                *wayptrv++ = false;
                                goto smerig2;
                            }
                        }
                        *wayptr++ = toInsert;
                        // If the previous entry was invalid this one is invalid too
                        *wayptrv++ = true;
smerig2:
                        continue;
                    }
                }
            }
        }
        acc_prev_candidates += candidates;
        candidates *= (m_associativity-1);
        acc_candidates += candidates;
    }
}

uint32_t CacheSetZ::getReplacementIndex(CacheCntlr *cntlr, Core::ExtraMemoryRequestInfo *info, uint32_t &eway, IntPtr addr, uint32_t& candidate, uint32_t* walk, bool* walkv, uint32_t& iidx, uint32_t* backtracked, uint32_t& pathlength) {

    /* Due to our lock in the cache controller, only one thread can be in this function at any given time
    * It is therefore completely safe to repartition here
    */
    //if(vantageEnabled()) {
    auto this_period = m_set_info->getCurrentAllocationPeriod();
    if (m_set_info->m_current_period < this_period) {

        m_set_info->m_current_period = this_period;
        if(vantageEnabled()) {
            repartition(false);
        }
#ifdef ASSOC_MEASUREMENTS
        else {
            CacheSetZ* root = dynamic_cast<CacheSetZ*>(setarray[0]);
            printf("\n-----------------ASSOCIATIVITY CDF------------------\n");
            for(uint32_t i=0; i<m_num_sets; i++) {
                for(uint32_t j=0; j<m_associativity; j++) {
                    if((i*m_associativity+j) % 80 == 0)
                        printf("\n");

                    printf("%d,", root->boek[i*m_associativity+j]);
                }
            }
            printf("\n------------------END---------------------\n");
        }
#endif
    }
    //}
#if 0
// ASSOC_MEASUREMENTS
    CacheSetZ* root = dynamic_cast<CacheSetZ*>(setarray[0]);

    if(root->globale_tijd == 10000) {
        for(uint32_t i=0; i<m_num_sets; i++) {
            for(uint32_t j=0; j<m_associativity; j++) {
                printf("%d,", root->boek[i*m_associativity+j]);
            }
            printf("\n");
        }
    }
#endif
    // At first all our timestamps are zero. We don't care where it goes
    uint32_t tmp = 0;
    doWalk(walk, walkv, addr, m_log_blocksize);


    if(!vantageEnabled()) {
	bool foundAtLeastOne = false;
        Lock* lck = dynamic_cast<CacheSetZ*>(setarray[0])->getCounterLock();

        lck->acquire();
#ifndef WIDE_COUNTERS
        uint8_t curTime = dynamic_cast<CacheSetZ*>(setarray[0])->counter;
        uint8_t maxtime = 0x0;
#else
	uint64_t curTime = dynamic_cast<CacheSetZ*>(setarray[0])->counter;
	uint64_t maxtime = 0xffffffffffffffff;
#endif
        lck->release();

        for(unsigned int i=0; i<m_associativity; i++) {
            for(unsigned int j=0; j<candidates_per_way; j++) {
                tmp = walk[i*candidates_per_way+j];
                bool valid = walkv[i*candidates_per_way+j];
                // only if the tag was valid in hashing sense we check the timestamp.
                if(valid) {
#ifndef WIDE_COUNTERS
		    uint8_t time;
#else
		    uint64_t time;
#endif
		    if(tmp != 0xffffffff){
                    	time = dynamic_cast<CacheSetZ*>(setarray[tmp])->timestamps[i];
#ifndef WIDE_COUNTERS
                        if(((uint8_t)(curTime-time))>maxtime || !foundAtLeastOne) {
#else
                        if(time<maxtime || !foundAtLeastOne) {
#endif
			    foundAtLeastOne = true;
    	                    eway = i;
#ifndef WIDE_COUNTERS
        	            maxtime = ((uint8_t)(curTime-time));
#else
			    maxtime = time;
#endif
                	    candidate = j;
                    	}
		    }
                }
            }
        }

#ifdef WIDE_COUNTERS
//	printf("%u\n", maxtime);
#endif
        /* We need to do this here, to prevent raceconditions (caused by race-conditions during the unlocked phase of the walk */
        // Now trace back to the parent
        // Our first backtrace index
        uint32_t walkidx = candidates_per_way*eway + candidate;
        backtracked[0] = walk[walkidx];
        backtracked[1] = eway;
        pathlength = 1;
        for(uint32_t i =0; i<L-1; i++) {
            // if we are our parent, we are the root of the walk. stop.
            if(walkidx == m_set_info->LUT[walkidx]) {
                break;
            }
            walkidx = m_set_info->LUT[walkidx];
            backtracked[2*pathlength] = walk[walkidx];
            backtracked[2*pathlength+1] = walkidx/candidates_per_way;
            pathlength++;
        }
        iidx = walk[walkidx];
        return walkidx / candidates_per_way;
    }
    else {
        UInt32* tmpBacktrack = backtracked;
        pathlength = 0;
        for(unsigned int i=0; i<m_associativity; i++) {
            for(unsigned int j=1; j<candidates_per_way; j++) {
                tmp = walk[i*candidates_per_way+j];
                bool valid = walkv[i*candidates_per_way+j];
                // only if the tag was valid in hashing sense we add it to be processed by vantage.
                if(valid) {
                    *tmpBacktrack++ = tmp;
                    *tmpBacktrack++ = i;
                    // Nasty name, but vantage wants to update all replacement entries
                    pathlength++;
                }
            }
        }
        return 0;
    }
}

uint32_t CacheSetZ::getReplacementIndex(CacheCntlr *cntlr, Core::ExtraMemoryRequestInfo *info, uint32_t &setidx, IntPtr addr, bool update) {
    assert(false);
    return 0;
}

uint32_t CacheSetZ::getReplacementIndex(CacheCntlr *cntlr, Core::ExtraMemoryRequestInfo *info, uint32_t &setidx, IntPtr addr) {
    assert(false);
    return 0;
}


// This one is assumed to be called only once on the set that was actually modified!
// This is why getReplacementIndex also returns a set_idx
void CacheSetZ::updateReplacementIndex(uint32_t accessed_index, Core::ExtraMemoryRequestInfo *info) {
    if(!vantageEnabled()) {
        // increase _global_ counter
        Lock* lck = dynamic_cast<CacheSetZ*>(setarray[0])->getCounterLock();
        // Critical section
        lck->acquire();
        CacheSetZ* root = dynamic_cast<CacheSetZ*>(setarray[0]);
#ifndef WIDE_COUNTERS
        root->accessCounter++;
        if(root->accessCounter >= plru_period) {
            root->accessCounter = 0;
            root->counter++;
        }
#else
	root->counter++;
#endif
        timestamps[accessed_index] = root->counter;
        lck->release();
#ifdef ASSOC_MEASUREMENTS

        root->globale_lock.acquire();
        tijden[accessed_index] = root->globale_tijd;
        root->globale_tijd++;
        root->globale_lock.release();
#endif
    }
    else { // VANTAGE enabled

// Update corresponding UMON
        if(m_num_cores >= 1) {
            m_set_info->umons[info->core_id]->updateUMON(info->address);
        }
// Update corresponding partition
        uint32_t partition = vPartitions[accessed_index];
        VantagePartitionState* state = m_set_info->states[partition];
        state->lock.acquire();
        // promotion
        if(partition >= m_num_cores) {
            IntPtr tmpTag = m_cache_block_info_array[accessed_index]->getTag();
	    // track unmanaged partition size
	    state->actualSize--;
            // unlock unmanaged region
            state->lock.release();
            state = m_set_info->states[info->core_id];
            state->lock.acquire();
	    vPartitions2[accessed_index] = info->core_id;
            vPartitions[accessed_index] = info->core_id;
	    core_tag[accessed_index] = info->core_id;
            state->actualSize++;
  	    state->inUnmanaged--;
        }
        // update timestamp to the current partitions time
        timestamps[accessed_index] = state->currentTs;
        // increaste the accesscounter of the partition and update the time
        state->accessCounter++;
        if(state->accessCounter>=state->actualSize/16) {
            state->accessCounter = 0;
            state->currentTs++;
            state->setPointTs++;
        }
        state->lock.release();
    }
}

// Interpolate the MISS-curves (not the HIT-curves, that does not make sense)
uint64_t CacheSetZ::interpolate(uint32_t point, uint32_t core) {
    zUMON** umonp = m_set_info->umons;
    // we have to use the associativity of the ATD's which (possibly) differ
    // from the zcache's way count.
    uint32_t umon_associativity = umonp[0]->m_umon_associativity;
#ifdef DSS_ENABLED
    uint32_t umon_num_sets = umonp[0]->m_umon_num_sets;
#endif
    uint64_t period = point*(umon_associativity)/256;
    uint64_t val;
    // calculate delta for interpolation
    // this should always result in positive value. (montonic decrease in misses)
    uint64_t delta =  umonp[core]->getMisses(period) - umonp[core]->getMisses(period+1);
    uint64_t begin = period*256/(umon_associativity);
    val = umonp[core]->getMisses(period);
    // delta is the difference between two periods.
    // and is therefore divided over umon_associativity/256 points
    // do the division at last for accuracy reasons (int division)
    val -= (delta * (point-begin) * (umon_associativity))/256;
    return val;
}


uint64_t CacheSetZ::getMU(uint32_t a, uint32_t b, uint32_t core) {
    assert(a<b);
    uint64_t valb = interpolate(b, core);
    uint64_t vala = interpolate(a, core);
    return (vala-valb)/(b-a);
}

uint64_t CacheSetZ::getMaxMU(uint32_t allocated, uint32_t balance, uint32_t& extra, uint32_t core) {
    uint64_t max_mu = 0;
    extra = 1;
    for(uint32_t ii=1; ii<=balance; ii++) {
        uint64_t mu = getMU(allocated, allocated+ii,core);
        if(mu > max_mu) {
            max_mu = mu;
            extra = ii;
        }
    }
    return max_mu;
}

void CacheSetZ::repartition(bool skipTargetSizes) {
    uint32_t* allocations = m_set_info->partAllocations;
    if(!skipTargetSizes) {
        uint32_t numUnmanagedUnits = m_set_info->FRACTION_UNMANAGED*256;
        uint32_t L2_size = m_set_info->l2_size;
        uint32_t L3_size = m_num_sets*m_associativity*64;
        uint32_t L3_unit_size = L3_size/256;
        uint32_t minUnitsPerCore = (m_set_info->allocate_l2 ? L2_size/L3_unit_size : 0);
#if 0
        printf("minUnitsPerCore %d L2-size %d, L3-size %d\n", minUnitsPerCore, L2_size, L3_size);
#endif
        uint32_t balance = 256-m_num_cores*minUnitsPerCore-numUnmanagedUnits;
        assert(set_no==0);
        // TODO: really, put this in the info object, this is nasty :D
        for(uint32_t i=0; i<m_num_cores; i++) {
            allocations[i]=minUnitsPerCore;
        }
        while(balance) {
            uint64_t max_mu = 0;
            uint32_t extra = 1;
            uint32_t winner = 0;
            for(uint32_t i=0; i<m_num_cores; i++) {
                uint32_t alloc = allocations[i];
                uint32_t extra_alloc;
                uint64_t tmp = getMaxMU(alloc, balance, extra_alloc, i);
                if(tmp > max_mu) {
                    max_mu = tmp;
                    extra = extra_alloc;
                    winner = i;
                }
            }
            allocations[winner] += extra;
            balance -= extra;
        }
    }
    /* Target sizes are available now. Fill in the thresholding table for every core */
    for(uint32_t i=0; i<m_num_cores; i++) {
        VantagePartitionState* state = m_set_info->states[i];
        // convert allocation units to cache line units
        if(!skipTargetSizes) {
            state->targetSize = allocations[i] * (m_num_sets * m_associativity)/256;
        }
        // fill in last entry
        float numSlackLines = SLACK*((float)state->targetSize);
        state->thrSize[NUM_TRESH-1] = state->targetSize+numSlackLines+1;
        state->thrDems[NUM_TRESH-1] = ((float)C_PERIOD)*AMAX;
        // Now divide the slack over the NUM_THRESH-1 remaining entries
        for(uint32_t j=0; j<NUM_TRESH-1; j++) {
            state->thrSize[j] = state->targetSize + (numSlackLines*(j+1))/(NUM_TRESH-1);
            state->thrDems[j] = ((float)(C_PERIOD*(j+1)))*AMAX/NUM_TRESH;
        }
    }

    fprintf(stdout, "PARTITION(target)\t: ");
    for(uint32_t i=0; i<m_num_cores; i++) {
        fprintf(stdout, "%d,", m_set_info->states[i]->targetSize);
    }
    fprintf(stdout, "\n");
    fprintf(stdout, "PARTITION(actual)\t: ");
    for(uint32_t i=0; i<m_num_cores; i++) {
        fprintf(stdout, "%d,", m_set_info->states[i]->actualSize);
    }
    fprintf(stdout, "\n");
    fprintf(stdout, "PARTITION(unmanaged)\t: ");
    uint32_t sum = 0;
    for(uint32_t i=0; i<m_num_cores; i++) {
        fprintf(stdout, "%d,", m_set_info->states[i]->inUnmanaged);
	sum += m_set_info->states[i]->inUnmanaged;
    }
    fprintf(stdout, "\n");
    fprintf(stdout, "Sum %d vs %d\n", sum, m_set_info->states[m_num_cores]->actualSize);
    fprintf(stdout, "UNWANTED REPLACEMENTS\t: %u vs %u\n", m_set_info->totalReplacements, m_set_info->faultyReplacements); 
    assert(sum==m_set_info->states[m_num_cores]->actualSize);
// TODO: move this to the UMON class
    for(uint32_t j=0; j<m_num_cores; j++) {
        fprintf(stdout, "UMON[%d, miss]:\t", j);
        for(int32_t z=-1; z<(int32_t)m_set_info->umons[j]->m_umon_associativity; z++) {
            fprintf(stdout, "%lu,", m_set_info->umons[j]->getMisses(z+1));
        }
	fprintf(stdout, "\n");
#if 0
        fprintf(stdout, "UMON[%d, hit]:\t",j);
        for(int32_t z=0; z<(int32_t)m_set_info->umons[j]->m_umon_associativity; z++) {
            fprintf(stdout, "%lu,", m_set_info->umons[j]->accessCounters[z]);
        }
        fprintf(stdout, "\n");
#endif
    }
	
   fprintf(stdout, "AVG DEMRATE %f %f %d %d %d %d\n", (double)m_set_info->core0demoted/m_set_info->core0periods, (double)m_set_info->core1demoted/m_set_info->core1periods, m_set_info->core0demoted, m_set_info->core0periods, m_set_info->core1demoted, m_set_info->core1periods);
    // halve the umon access counters, this is to keep part of the history.
    for(uint32_t i=0; i<m_num_cores; i++) {
        zUMON* umon = m_set_info->umons[i];
        for(uint32_t j=0; j<umon->m_umon_associativity; j++)
            umon->accessCounters[j] >>=1;
    }
}

CacheSetInfoZ::CacheSetInfoZ(String name, String cfgname, core_id_t core_id, uint32_t associativity,
                             uint32_t block_size, uint32_t num_sets, uint32_t shared_cores, uint32_t levels) :
    m_num_sets(num_sets),
    m_shared_cores(shared_cores),
    m_associativity(associativity),
    m_blocksize(block_size),
    m_name(name),
    m_cfgname(cfgname),
    m_core_id(core_id),
    L(Sim()->getCfg()->getIntArray(cfgname + "/zcache/levels", core_id)),
    nindex_bits(Sim()->getCfg()->getIntArray(cfgname + "/zcache/nindex_bits", core_id)),
    vantage_enabled(Sim()->getCfg()->getBoolArray(cfgname + "/zcache/vantage", core_id)),
    allocate_l2(Sim()->getCfg()->getBoolArray(cfgname + "/allocate_l2", core_id))
{
    l2_size = Sim()->getCfg()->getInt("perf_model/l2_cache/cache_size")*1024;
    m_reallocate_period = 5000000;
    uint32_t candidates_per_way = power(m_associativity-1, L-1);
    // create the LUT
    LUT = new uint32_t[candidates_per_way*m_associativity];
    H3_HASH = new uint32_t[m_num_sets*m_associativity];
    readH3Hash(Sim()->getCfg()->getStringArray(cfgname + "/zcache/hashfile", core_id).c_str(), H3_HASH, nindex_bits*m_associativity);
    for(uint32_t i = 0; i<m_associativity; i++) {
        LUT[i*candidates_per_way] = i*candidates_per_way;
    }

    core0demoted = core1demoted = core0periods = core1periods = 0;
    // read unmanaged region config
    if(vantage_enabled){
    	FRACTION_UNMANAGED = Sim()->getCfg()->getFloat(cfgname + "/zcache/unmanaged");
    	printf("Vantage using %f percent of the cache as unmanaged\n", FRACTION_UNMANAGED*100.f);
    }
    // For each way, fill the LUT table
    for(uint32_t i=0; i<m_associativity; i++) {
        uint32_t candidates = 1;
        uint32_t acc_candidates = 1;
        uint32_t acc_prev_candidates = 0;
        // Per level the amount of candidates changes
        for(uint32_t j = 0; j<(L-1); j++) {
            // Only point to locations containing hashed adresses from other ways
            uint32_t* wayptr = &LUT[i*candidates_per_way+acc_candidates];
            for(uint32_t k = 0; k<i; k++) {
                uint32_t wrkidx = k*candidates_per_way+acc_prev_candidates;
                for(uint32_t l = 0; l<candidates; l++) {
                    *wayptr++ = wrkidx++;
                }
            }

            for(uint32_t k = (i+1); k<m_associativity; k++) {
                uint32_t wrkidx = k*candidates_per_way+acc_prev_candidates;
                for(uint32_t l = 0; l<candidates; l++) {
                    *wayptr++ = wrkidx++;
                }
            }
            acc_prev_candidates += candidates;
            candidates *= (m_associativity-1);
            acc_candidates += candidates;
        }
    }
    // create n+1 states. (one for the unmanaged region)
    states = new VantagePartitionState*[m_shared_cores+1];

    uint32_t numCacheLines = m_num_sets*m_associativity;
    // calculate based on partitioning units as repartition will later do as well.
    // this may involve truncration, that's why we do it here too.
    uint32_t unmanagedSize = (FRACTION_UNMANAGED*256);
    // multiply with unit size.
    unmanagedSize *= numCacheLines/256;

    states[m_shared_cores] = new VantagePartitionState();
    // allocate unmanaged region
    states[m_shared_cores]->targetSize = unmanagedSize;

    // Initial allocation of all our partitions.
    // First allocation may miss m_shared_cores-1 _lines_
    // Look-ahead later on fixes this.
    for(uint32_t i=0; i<m_shared_cores; i++) {
        states[i] = new VantagePartitionState();
        states[i]->targetSize = (numCacheLines-unmanagedSize)/m_shared_cores;
	fprintf(stdout, "inumanaged %d\n", states[i]->inUnmanaged);
//	states[i]->inUnmanaged = 0;
//	states[i]->candsSeen =0;
    }

    // Create an UMON per core.
    assert(m_shared_cores > 0);
    uint32_t umon_associativity = Sim()->getCfg()->getIntArray(cfgname + "/zcache/umon_associativity", m_core_id);
    CacheSetInfo* tmp = CacheSet::createCacheSetInfo(name, cfgname, m_core_id, "lru", umon_associativity);
    umons = new zUMON*[shared_cores];
    partAllocations = new uint32_t[shared_cores];

    // Initial allocation will not be based on UMON data.
    for(unsigned int i=0; i<shared_cores; i++) {
        umons[i] = new zUMON(this, tmp);
    }

    // create timer (Took it straight from runar's code)
    const ComponentPeriod* period = Sim()->getDvfsManager()->getCoreDomain(Sim()->getDvfsManager()->getCoreDomainId(0));
    m_time_converter = new SubsecondTimeCycleConverter(period);

}

CacheSetInfoZ::~CacheSetInfoZ() {
    delete [] umons;
}

/*
 * Inspects all valid replacement candidates as found by the z-cache
 * returns the way of the place for insertion.
 * outputs the index of insertion, the way of eviction and the candidate number
 * of the entry to be evicted
 */
uint32_t CacheSetZ::getVantageReplacementIndex(uint32_t* walk, bool* walkv, uint32_t& candidate, uint32_t& iidx, uint32_t& eway, Core::ExtraMemoryRequestInfo *info) {
    CacheSetZ* set;
    uint32_t oldestDemotedCandidate = 0xffffffff;
    uint32_t oldestDemotedWay = 0x0;
    uint8_t oldestDemotedStamp = 0x0; 
    uint8_t oldestDemotedPartition = 0x0;

    std::vector<uint32_t> toBeDemoted;
    std::vector<uint32_t> managedRCandidates;
    assert(info);
    VantagePartitionState* unmanaged = m_set_info->states[m_num_cores];
    for(uint32_t i=0; i<m_associativity; i++) {
        for(uint32_t k=0; k<candidates_per_way; k++) {
            // if not accepted by the z-cache
            if(!walkv[i*candidates_per_way + k])
                continue;
            // it is an entry containing no data element, so we can use it
            managedRCandidates.push_back(i*candidates_per_way+k);
            // but since it is empty we cannot update its vantage partition state so stop here
            if(!setarray[walk[candidates_per_way*i+k]]->m_cache_block_info_array[i]->isValid())
                continue;
            set = dynamic_cast<CacheSetZ*>(setarray[walk[i*candidates_per_way + k]]);
            /* Hits and misses can occur at the same time */
            uint32_t partition = set->vPartitions[i];
            // if not in the unmanaged region, check for demotion
            bool core0demoted=false;
	    bool core1demoted=false;
            if(partition < m_num_cores) {
                VantagePartitionState* p = m_set_info->states[partition];
                p->lock.acquire();
                if(p->actualSize > p->targetSize) {
                    if((p->currentTs >= p->setPointTs && !(set->timestamps[i] > p->setPointTs && set->timestamps[i] < p->currentTs)) || (p->currentTs < p->setPointTs && (set->timestamps[i] >= p->currentTs && set->timestamps[i] <= p->setPointTs))) {
			if(partition == 0){
				m_set_info->core0demoted++;
				core0demoted = true;
		//		fprintf(stdout, "demoting %d %d %d %d %d\n", p->actualSize, p->targetSize, p->setPointTs, p->currentTs, p->candsDemoted);
			}
			else if(partition == 1){
				m_set_info->core1demoted++;
				core1demoted = true;
			}
                        // Tag it as unmanaged
                        set->vPartitions[i] = m_num_cores;
			set->vPartitions2[i] = partition; 
			// Set core tag to track unmanaged region usage.
			set->core_tag[i] = partition;
                        // TODO: I update the unmanaged timestamp here. Paper does
                        // not clarify where they do that.
                        unmanaged->lock.acquire();
			unmanaged->actualSize++;
                        set->timestamps[i] = unmanaged->currentTs;
                        unmanaged->accessCounter++;

                        // The unmanaged region has a fixed size. Use that in 
                        // incrementing the counter
                        if(unmanaged->accessCounter>=unmanaged->targetSize/16) {
                            unmanaged->accessCounter = 0;
                            unmanaged->currentTs++;
                        }
                        unmanaged->lock.release();
                        // Decrease corresponding partitions size
                        p->actualSize--;
			p->inUnmanaged++;
                        p->candsDemoted++;
                        toBeDemoted.push_back(i*candidates_per_way + k);
                    }
                }
                if(core0demoted){
			m_set_info->core0periods++;
		}
		else if(core1demoted){
			m_set_info->core1periods++;
		}
		// regardless of whether we demoted or not
                p->candsSeen++;
                if(p->candsSeen == 0) {
                    uint32_t j=0;
                    // Find the correct entry
                    // the paper is again a bit vague here. It never defined
                    // what ThrSize contains, lower or upper limit of the partition size
                    // I assume it is upper otherwhise the setpoint is incremented when
                    // the partition is not too big.
                    if(p->actualSize >= p->targetSize) {
                        for(; j<(NUM_TRESH-1); j++) {
                            if(p->actualSize < p->thrSize[j])
                                break;
                        }
                        if(p->candsDemoted > p->thrDems[j]) {
			    if(partition==0){
	//			fprintf(stdout, "%d vs %d %d %d\n", p->candsDemoted, p->thrDems[j], p->actualSize, p->targetSize);
			    }
                            uint32_t before = p->setPointTs;
                            p->setPointTs--;
                            assert(p->setPointTs != before);
                        }
                        else {
			    // once currentTs = setPoinTs, we have maximum demotion
			    // further increasing is a logical flaw.
			    if(p->setPointTs != p->currentTs){
                            	p->setPointTs++;
			    }
                        }
                    }
#if 1 // This was not in the vantage paper. happens when a repartition occurs
//during a demotion phase. That would fix itself though, but doesnt hurt.
                    else {
                        // we are not too big, but demotion has taken place. prevent it.
                        if(p->candsDemoted > 0){
//			    fprintf(stdout, "%d %d %d\n", partition, p->targetSize, p->actualSize);
//			    assert(false);
                            p->setPointTs--;
			}
                    }
#endif
                    // This cannot be moved within the if! (demotions might have taken place to actually
                    p->candsDemoted = 0;
                }
                p->lock.release();
            }
            // This candidate was already in the unmanaged region
            else {

                unmanaged->lock.acquire();
                if(oldestDemotedCandidate == 0xffffffff) {
                    oldestDemotedCandidate = k;
                    oldestDemotedWay = i;
		    oldestDemotedPartition = set->core_tag[i];
                    oldestDemotedStamp = (unmanaged->currentTs-set->timestamps[i]);
                }
                else {
                    if(((uint8_t)(unmanaged->currentTs-set->timestamps[i]))>oldestDemotedStamp) {
                        oldestDemotedCandidate = k;
                        oldestDemotedWay = i;
			oldestDemotedPartition = set->core_tag[i];
                        oldestDemotedStamp = (unmanaged->currentTs-set->timestamps[i]);
                    }
                }
                unmanaged->lock.release();
            }
        }
    }

    int tmp = 0;
    uint32_t partition = 0;
    bool foundFreeEntry = false;

    if(oldestDemotedCandidate != 0xffffffff) {
	// Evicting demoted candidate
	VantagePartitionState* p = m_set_info->states[oldestDemotedPartition];
        p->lock.acquire();
	p->inUnmanaged--;
	p->lock.release();
	p = m_set_info->states[m_num_cores];
	p->lock.acquire();
	p->actualSize--;
	p->lock.release();
        candidate = oldestDemotedCandidate;
        eway = oldestDemotedWay;
        goto backtrackVantage;
    }
    else if(toBeDemoted.size() > 0) {
        // pick a random to be demoted entry
        tmp = rand()%toBeDemoted.size();
	// which core did it belong to?
	set = dynamic_cast<CacheSetZ*>(setarray[walk[toBeDemoted[tmp]]]);
        eway = toBeDemoted[tmp]/candidates_per_way;
	uint32_t core = set->core_tag[eway];	
	VantagePartitionState* p = m_set_info->states[core];
        p->lock.acquire();
	p->inUnmanaged--;
	p->lock.release();
	p = m_set_info->states[m_num_cores];
	p->lock.acquire();
	p->actualSize--;
	p->lock.release();

        candidate = toBeDemoted[tmp]-eway*candidates_per_way;
        goto backtrackVantage;
    }

    // Check wether there exists an invalid entry first.
    for(uint32_t i=0; i<managedRCandidates.size(); i++) {
        eway = managedRCandidates[i]/candidates_per_way;
        candidate = managedRCandidates[i]-eway*candidates_per_way;
        if(!setarray[walk[candidates_per_way*eway+candidate]]->m_cache_block_info_array[eway]->isValid()) {
            foundFreeEntry = true;
            break;
        }
    }
    // This is the case where vantage cannot find a suitable candidate.

    if(!foundFreeEntry) {
	m_set_info->faultyReplacements++;
        tmp = rand()%managedRCandidates.size();
        eway = managedRCandidates[tmp]/candidates_per_way;
        candidate = managedRCandidates[tmp]-eway*candidates_per_way;
        // We will do an eviction without a demotion first.
        // The paper doesn't state this, but this does decrease the partitionsize!
        partition = dynamic_cast<CacheSetZ*>(setarray[walk[candidates_per_way*eway + candidate]])->vPartitions[eway];
        m_set_info->states[partition]->lock.acquire();
        // Make sure we don't make the partition really big (remember unsigned int)
        // This should also not happen, but during unb0rking vantage
        assert(m_set_info->states[partition]->actualSize > 0);
        m_set_info->states[partition]->actualSize--;
        m_set_info->states[partition]->lock.release();
    }
backtrackVantage:
    m_set_info->totalReplacements++;
    uint32_t walkidx = candidates_per_way*eway + candidate;
    for(uint32_t i =0; i<L-1; i++) {
        // if we are our parent, we are the root of the walk. stop.
        if(walkidx == m_set_info->LUT[walkidx]) {
            break;
        }
        walkidx = m_set_info->LUT[walkidx];
    }
    iidx = walk[walkidx];
    return walkidx / candidates_per_way;
}
