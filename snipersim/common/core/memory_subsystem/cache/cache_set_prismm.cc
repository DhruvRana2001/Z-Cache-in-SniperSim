#include <limits>
#include <random>

#include "cache_set_prismm.h"
#include "log.h"
#include <cmath>
#include "cache_set_lru.h"
#include "pr_l1_cache_block_info.h"
#include "stats.h"
#include "utils.h"

CacheSetInfoPriSMM::CacheSetInfoPriSMM(
    String name, String cfgname,
    core_id_t core_id, UInt32 associativity, UInt32 cache_block_size, UInt32 num_sets, UInt32 shared_cores,
    CacheBase::hash_t hash_function) :
     m_num_sets(num_sets)
    ,m_associativity(associativity)
    , m_miss_counter(0)
    , m_miss_period_limit(Sim()->getCfg()->getInt(cfgname + "/prism_period"))
    , m_shared_cores(shared_cores)
    , m_num_blocks(num_sets*associativity)
{
    l2_size = Sim()->getCfg()->getInt("perf_model/l2_cache/cache_size")*1024;
    m_reallocate_period = 5000000;
    m_miss_period_limit = num_sets*associativity; 

    not_founds = new uint64_t[shared_cores];
    for(uint32_t i=0; i<shared_cores; i++){
   	 not_founds[i] = 0;
    }
    total_replacements = 0;
    setAccessCounters = new uint64_t[num_sets];
    m_core_usage_counter = new UInt64[shared_cores];
    m_shared_miss_counter = new UInt64[shared_cores];
    m_shared_hit_counter = new UInt64[shared_cores];
    m_eviction_prob = new double[shared_cores];
    corelocks = new Lock[shared_cores];

    for(UInt32 i = 0; i < shared_cores; i++) {
	m_core_usage_counter[i] = m_shared_hit_counter[i] = m_shared_miss_counter[i] = 0;
        m_eviction_prob[i] = (double)1.0/shared_cores;
    }
    uint32_t umon_associativity = associativity; //Sim()->getCfg()->getIntArray(cfgname + "/zcache/umon_associativity", core_id);

    CacheSetInfo* tmp = CacheSet::createCacheSetInfo(name, cfgname, m_core_id, "lru", umon_associativity);

    umons = new pUMON*[shared_cores];
    partAllocations = new uint32_t[shared_cores];

    // Initial allocation will not be based on UMON data.
    for(unsigned int i=0; i<shared_cores; i++) {
        umons[i] = new pUMON(this, tmp);
    }
    // create timer (Took it straight from runar's code)
    const ComponentPeriod* period = Sim()->getDvfsManager()->getCoreDomain(Sim()->getDvfsManager()->getCoreDomainId(0));
    m_time_converter = new SubsecondTimeCycleConverter(period);

}

CacheSetInfoPriSMM::~CacheSetInfoPriSMM()
{
	printf("[FINAL STATS]: %d", total_replacements);
	uint64_t sum = 0;
	for(uint32_t i=0; i<m_shared_cores; i++){
		printf("%d,", not_founds[i]);	
		sum += not_founds[i];
	}
	printf("%f\n", (double)sum/total_replacements);

    	delete[] m_eviction_prob;
}


void
CacheSetInfoPriSMM::incrementCoreCounter(core_id_t core_id)
{
    corelocks[core_id].acquire();
    m_core_usage_counter[core_id]++;
    corelocks[core_id].release();
}

void
CacheSetInfoPriSMM::decrementCoreCounter(core_id_t core_id)
{

    corelocks[core_id].acquire();
    assert(m_core_usage_counter[core_id] > 0);
    m_core_usage_counter[core_id]--;
    corelocks[core_id].release();
}

void
CacheSetInfoPriSMM::incrementHitCounter(core_id_t core_id)
{
    corelocks[core_id].acquire();
    m_shared_hit_counter[core_id]++;
    corelocks[core_id].release();
}


UInt32
CacheSetInfoPriSMM::getReplacementTarget()
{
    // m_eviction_prob of cores with a value less than 1 sum to 1

    // Draw a random number between 0 and 1, and a random start core
    double random = rng.next(32768) / 32767.;
    UInt32 core = 0; //rng.next(m_shared_cores);
    UInt32 start = core;
  
    // Pick a random core with non zero and less than one eviction probability
    while(random >= m_eviction_prob[core] || m_eviction_prob[core] != 1.) {
        if(random < m_eviction_prob[core] && m_eviction_prob[core] != 1.) {
            break;
        }
        if (m_eviction_prob[core] != 1.) {
            random -= m_eviction_prob[core];
        }
        core = (core + 1) % m_shared_cores;

        if (core==start) {
            return m_shared_cores;
        }
    }
    return core;
}

bool
CacheSetInfoPriSMM::canEvict(core_id_t core)
{
    return m_eviction_prob[core] > 0;
}


bool
CacheSetInfoPriSMM::mustEvict(core_id_t core)
{
    return m_eviction_prob[core] == 1.;
}

CacheSetPriSMM::CacheSetPriSMM(
    CacheBase::cache_t cache_type,
    UInt32 associativity, UInt32 blocksize, UInt32 set_no, CacheSetInfoPriSMM *info)
    : CacheSet(cache_type, associativity, blocksize)
    , m_info(info)
    , set_no(set_no)
{
    m_lru_bits = new UInt8[m_associativity];
    for (UInt32 i = 0; i < m_associativity; i++) {
        m_lru_bits[i] = i;
    }
    if(set_no==0){
	repartition(true);
    }
}

CacheSetPriSMM::~CacheSetPriSMM()
{
    delete[] m_lru_bits;
}

void
CacheSetInfoPriSMM::incrementMissCounter(core_id_t core_id)
{
    corelocks[core_id].acquire();
    m_shared_miss_counter[core_id]++;
    corelocks[core_id].release();
    lock.acquire();
    m_miss_counter++;
    lock.release();
}


UInt32
CacheSetPriSMM::getReplacementIndex(CacheCntlr *cntlr, Core::ExtraMemoryRequestInfo *info, UInt32 &setidx, IntPtr addr)
{
   m_info->umons[info->core_id]->updateUMON(info->address); 
   m_info->incrementMissCounter(info->core_id);
   m_info->lock.acquire();
#if 0
   if(info->core_id==1){
	   printf("*%x\n", addr);
   }
#endif
   // auto this_period = m_info->getCurrentAllocationPeriod();
   // if (m_info->m_current_period < this_period) {
     //   m_info->m_current_period = this_period;
      //  repartition(false);
    //}
    if (m_info->m_miss_counter >= m_info->m_miss_period_limit) {
	repartition(false);
    }
    m_info->lock.release();
#ifdef COUNT_SET_ACCESSES
    if(info->core_id == 0){ 
    	m_info->setAccessCounters[(addr>>6) & (m_info->m_num_sets-1)]++;
    }
#endif
    UInt32 replacement_index = 0;

    // Pick random replacement target
    auto target_core = m_info->getReplacementTarget();

    UInt8 max_one_lru = 0, max_one_pos = m_associativity+1;
    UInt8 max_target_lru = 0, max_target_pos = m_associativity+1;
    UInt8 max_other_lru = 0, max_other_pos = m_associativity+1;
    UInt8 lru_pos = 0;

    for (UInt32 i = 0; i < m_associativity; i++) {
        if(m_info->mustEvict(m_cache_block_info_array[i]->getOwner()) && max_one_lru <= m_lru_bits[i]) {
            max_one_lru = m_lru_bits[i];
            max_one_pos = i;
        } else if(m_cache_block_info_array[i]->getOwner() == target_core && max_target_lru <= m_lru_bits[i]) {
            max_target_lru = m_lru_bits[i];
            max_target_pos = i;
        } else if(max_other_lru <= m_lru_bits[i] && (m_cache_block_info_array[i]->getOwner() == ~0 || m_info->canEvict(m_cache_block_info_array[i]->getOwner()))) {
            max_other_lru = m_lru_bits[i];
            max_other_pos = i;
        }
        if (m_lru_bits[i] >= m_associativity-1) {
            lru_pos = i;
        }
    }
m_info->lock.acquire();
m_info->total_replacements++;

    // Replace either LRU from cores with E=1, LRU from selected victim core, LRU from all cores with E > 0 or finally the actual LRU block
    if (max_one_pos < m_associativity) {
        replacement_index = max_one_pos;
    } else if (max_target_pos < m_associativity) {
        replacement_index = max_target_pos;
    } else if (max_other_pos < m_associativity) {
        replacement_index = max_other_pos;
	if(m_cache_block_info_array[max_other_pos]->getOwner() != ~0){
		if(target_core < m_info->m_shared_cores)
			m_info->not_founds[target_core]++;
	}
    } else {
        replacement_index = lru_pos;
		if(target_core < m_info->m_shared_cores){
			m_info->not_founds[target_core]++;
		}
	}
    m_info->lock.release();
    moveToMRU(replacement_index);

    // Update utilization counters
    if(m_cache_block_info_array[replacement_index]->getOwner() != ~0) {
        m_info->decrementCoreCounter(m_cache_block_info_array[replacement_index]->getOwner());
    }
    m_info->incrementCoreCounter(info->core_id);

    return replacement_index;
}

void
CacheSetPriSMM::updateReplacementIndex(UInt32 accessed_index, Core::ExtraMemoryRequestInfo *info)
{
    m_info->incrementHitCounter(info->core_id);
    moveToMRU(accessed_index);
    m_info->umons[info->core_id]->updateUMON(info->address);
}

void
CacheSetPriSMM::moveToMRU(UInt32 accessed_index)
{
    for (UInt32 i = 0; i < m_associativity; i++)
    {
        if (m_lru_bits[i] < m_lru_bits[accessed_index])
            m_lru_bits[i]++;
    }
    m_lru_bits[accessed_index] = 0;
}


pUMON::pUMON(CacheSetInfoPriSMM* info, CacheSetInfo* ATDinfo) :
    m_set_info(info),
    m_umon_associativity(info->m_associativity)
{
    // UMON's with a lower way count than the real cache are not allowed
    // ah well, they are for prism's-ucp sensitivity experiment.
    printf("%p %d %d\n", m_set_info);
//    assert(m_umon_associativity >= m_set_info->m_associativity);
    if(m_umon_associativity >= m_set_info->m_associativity){
    	m_umon_num_sets = m_set_info->m_num_sets / (m_umon_associativity/m_set_info->m_associativity);
    }
    else{
	m_umon_num_sets = m_set_info->m_num_sets * (m_set_info->m_associativity/m_umon_associativity);
    }
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

uint64_t pUMON::getMisses(uint32_t allocated) {
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

pUMON::~pUMON() {
    /* TODO: proper cleanup */
}

/* TODO: fix the hardcoded 64 byte blocksize */
void pUMON::updateUMON(IntPtr address) {
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

// Interpolate the MISS-curves (not the HIT-curves, that does not make sense)
uint64_t CacheSetPriSMM::interpolate(uint32_t point, uint32_t core) {
    pUMON** umonp = m_info->umons;
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


uint64_t CacheSetPriSMM::getMU(uint32_t a, uint32_t b, uint32_t core) {
    assert(a<b);
    uint64_t valb = interpolate(b, core);
    uint64_t vala = interpolate(a, core);
    return (vala-valb)/(b-a);
}

uint64_t CacheSetPriSMM::getMaxMU(uint32_t allocated, uint32_t balance, uint32_t& extra, uint32_t core) {
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

void CacheSetPriSMM::repartition(bool skip) {
    CacheSetInfoPriSMM* m_set_info = m_info;
    uint32_t* allocations = m_info->partAllocations;
    if(!skip){
    // L2 is 256KiB.
    uint32_t L2_size = m_set_info->l2_size;
    uint32_t L3_size = m_set_info->m_num_sets*m_associativity*64;
    uint32_t L3_unit_size = L3_size/256;
    uint32_t minUnitsPerCore = L2_size/L3_unit_size;
    uint32_t balance = 256-m_set_info->m_shared_cores*minUnitsPerCore;
        for(uint32_t i=0; i<m_set_info->m_shared_cores; i++) {
            allocations[i]=minUnitsPerCore;
        }
        while(balance) {
            uint64_t max_mu = 0;
            uint32_t extra = 1;
            uint32_t winner = 0;
            for(uint32_t i=0; i<m_set_info->m_shared_cores; i++) {
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
     // halve the umon access counters, this is to keep part of the history.
    for(uint32_t i=0; i<m_set_info->m_shared_cores; i++) {
        pUMON* umon = m_set_info->umons[i];
        for(uint32_t j=0; j<umon->m_umon_associativity; j++)
            umon->accessCounters[j] >>= 1;
    }
  
    for(uint32_t j=0; j<m_info->m_shared_cores; j++) {
        fprintf(stdout, "UMON[%d, miss]:\t", j);
        for(int32_t z=-1; z<(int32_t)m_info->umons[j]->m_umon_associativity; z++) {
            fprintf(stdout, "%lu,", m_info->umons[j]->getMisses(z+1));
        }
	fprintf(stdout, "\n");
    }

    for(uint32_t j=0; j<m_info->m_shared_cores; j++) {
        fprintf(stdout, "UMON[%d, hit]:\t", j);
        for(int32_t z=0; z<(int32_t)m_info->umons[j]->m_umon_associativity; z++) {
            fprintf(stdout, "%lu,", m_info->umons[j]->accessCounters[z]);
        }
	fprintf(stdout, "\n");
    }

 fprintf(stdout, "SHARED MISS\t: ");
    for(uint32_t i=0; i<m_info->m_shared_cores; i++) {
        fprintf(stdout, "%d,", m_info->m_shared_miss_counter[i]);
    }
	printf("\n");
    double occupancy[m_set_info->m_shared_cores];
    double target_occupancy[m_info->m_shared_cores];

    uint32_t m_shared_cores = m_set_info->m_shared_cores;
    for(UInt32 core = 0; core < m_set_info->m_shared_cores; core++) {
            m_info->corelocks[core].acquire();
            occupancy[core] = (double)m_info->m_core_usage_counter[core]/m_info->m_num_blocks;
 	    if(!skip){
	    target_occupancy[core] = ((double)m_info->partAllocations[core])/256;
		}
		else{
		occupancy[core] = 0;
		target_occupancy[core] = 1/(double)m_info->m_shared_cores;
	}
	double eviction;
	if(m_info->m_miss_counter != 0){
           eviction = ((occupancy[core] - target_occupancy[core]) * (double)m_info->m_num_blocks/m_info->m_miss_counter) + (double)m_info->m_shared_miss_counter[core]/m_info->m_miss_counter;
	}
	else{
	   eviction = (double)1.0/m_info->m_shared_cores;
	}
            eviction = std::max(0., eviction);

            m_info->m_eviction_prob[core] = eviction;
            m_info->corelocks[core].release();
    }

            uint8_t overig = 0;
            double sum = 0;
#if 1
            for(UInt32 core = 0; core < m_info->m_shared_cores; core++) {
                // acquiring locks here
                m_info->corelocks[core].acquire();
                sum += m_info->m_eviction_prob[core];
	    }
	
	    if(sum >= 1.){
            	for(UInt32 core = 0; core < m_info->m_shared_cores; core++) {
                	m_info->m_eviction_prob[core] /= sum;
       	     	}
	    }

#endif

        // Reset counters
        m_info->m_miss_counter = 0;
        for(UInt32 core = 0; core < m_shared_cores; core++) {
            m_info->m_shared_hit_counter[core] = 0;
            m_info->m_shared_miss_counter[core] = 0;
            // and releasing them here
            m_info->corelocks[core].release();
        }

	fprintf(stdout, "partition(target)\t: ");
	for(uint32_t i=0; i<m_info->m_shared_cores; i++) {
        	fprintf(stdout, "%.4f,", target_occupancy[i]);
    	}
    	printf("\n");
 	fprintf(stdout, "PARTITION(actual)\t: ");
    	for(uint32_t i=0; i<m_info->m_shared_cores; i++) {
        	fprintf(stdout, "%.4f,", occupancy[i]);
    	}
    	printf("\n");
 	fprintf(stdout, "EVICTION\t: ");
    	for(uint32_t i=0; i<m_info->m_shared_cores; i++) {
        	fprintf(stdout, "%.4f,", m_info->m_eviction_prob[i]);
    	}
    	printf("\n");
 	printf("UNWANTED\t: ");
    	for(uint32_t i=0; i<m_info->m_shared_cores; i++) {
        	fprintf(stdout, "%d,", m_info->not_founds[i]);
    	}
    	printf("\n");
}

