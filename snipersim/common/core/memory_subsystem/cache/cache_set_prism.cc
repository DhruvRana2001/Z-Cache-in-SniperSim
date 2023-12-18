#include "cache_set_prism.h"
#include "log.h"
#include <cmath>

CacheSetInfoPriSM::CacheSetInfoPriSM(
    String name, String cfgname,
    core_id_t core_id, UInt32 associativity, UInt32 cache_block_size, UInt32 num_sets, UInt32 shared_cores,
    CacheBase::hash_t hash_function)
    : CacheSetInfoShadow(name, cfgname, core_id, associativity, cache_block_size, num_sets, shared_cores, hash_function)
    , m_core_total_usage(0)
    , m_miss_counter(0)
    , m_miss_period_limit(Sim()->getCfg()->getInt(cfgname + "/prism_period"))
    , m_shared_cores(shared_cores)
    , m_num_blocks(num_sets*associativity)
{
    m_core_usage_counter = new UInt64[shared_cores];
    m_shared_miss_counter = new UInt64[shared_cores];
    m_shared_hit_counter = new UInt64[shared_cores];
    // Tomwi: The paper states the period equals the amount of cache-lines in
    // their experiment
    m_miss_period_limit = num_sets*associativity; 
    m_eviction_prob = new double[shared_cores];
    corelocks = new Lock[shared_cores];
    not_founds = 0;
    total_replacements = 0;
    for(UInt32 i = 0; i < shared_cores; i++) {
        m_core_usage_counter[i] = m_shared_hit_counter[i] = m_shared_miss_counter[i] = 0;
        m_eviction_prob[i] = (double)1.0/shared_cores;
    }
}

CacheSetInfoPriSM::~CacheSetInfoPriSM()
{

printf("[FINAL STATS]: %d %d %.2f\n", total_replacements, not_founds, (double)not_founds/total_replacements);
    delete[] m_shared_hit_counter;
    delete[] m_shared_miss_counter;
    delete[] m_core_usage_counter;
    delete[] m_eviction_prob;
}

void
CacheSetInfoPriSM::accessCache(core_id_t core, IntPtr address, bool hit)
{
    // Update shadow tags
    CacheSetInfoShadow::accessCache(core, address, hit);

    // Recalclulate m_eviction_prob using the miss minization algorithm
    lock.acquire();
    if (m_miss_counter >= m_miss_period_limit) {

        double occupancy[m_shared_cores];
        double potential_gain[m_shared_cores];
	double target_occupancy_u[m_shared_cores];
        double total_gain = 0;
        //std::cout << "Hits:\n";
        for(UInt32 core = 0; core < m_shared_cores; core++) {
            corelocks[core].acquire();
	    printf("%d %d\n", getHits(core), m_shared_hit_counter[core]);
            potential_gain[core] = (int64_t)((int64_t)getHits(core) - (int64_t)m_shared_hit_counter[core]);
            total_gain += potential_gain[core];
            occupancy[core] = (double)m_core_usage_counter[core]/m_num_blocks;
            corelocks[core].release();
            //std::cout << "\tCore" << core << ": owns " << m_core_usage_counter[core] << " " << occupancy[core] << " private " << getHits(core) << " shared " << m_shared_hit_counter[core] << " gain " << potential_gain[core] << "\n";
        }
        //std::cout << "Total gain: " << total_gain << "\n";

        // Calculate target occupancy
        double target_occupancy[m_shared_cores];
        if (total_gain > 0) {
            double sum = 0;
            for(UInt32 core = 0; core < m_shared_cores; core++) {
                target_occupancy[core] = occupancy[core] * (1. + (potential_gain[core] / total_gain));
		target_occupancy_u[core]= target_occupancy[core];
                sum += target_occupancy[core];
            }
            //std::cout << "\nTarget occupancy:\n";
            for(UInt32 core = 0; core < m_shared_cores; core++) {
                target_occupancy[core] /= sum;
                //std::cout << "\tCore" << core << ": " << target_occupancy[core] << "\n";
            }
        } else {
            // If there is nothing to gain target should equal current occupancy
            //std::cout << "\nTarget occupancy:\n";
            for(UInt32 core = 0; core < m_shared_cores; core++) {
                target_occupancy[core] = occupancy[core];
                //std::cout << "\tCore" << core << ": " << target_occupancy[core] << "\n";
            }
        }


        //std::cout << "Eviction probabilites:\n";
        // Calculate eviction properties
        for(UInt32 core = 0; core < m_shared_cores; core++) {
            corelocks[core].acquire();
            double eviction = ((occupancy[core] - target_occupancy[core]) * (double)m_num_blocks/m_miss_counter) + (double)m_shared_miss_counter[core]/m_miss_counter;
            eviction = std::max(0., eviction);

            m_eviction_prob[core] = eviction;
            corelocks[core].release();
            //std::cout << "\tCore" << core << ": " << eviction << "\n";
        }

        {
            // Tomwi: Basically one big critical section
            double sum = 0;
#if 1
            for(UInt32 core = 0; core < m_shared_cores; core++) {
                // acquiring locks here
                corelocks[core].acquire();
                sum += m_eviction_prob[core];
            }
#endif
            printf("[prism]: ");
#if 1
	if(sum >= 1.){
            for(UInt32 core = 0; core < m_shared_cores; core++) {
                    m_eviction_prob[core] /= sum;
            }
	}
#endif
	  /* Log statistics */
            for(UInt32 core = 0; core < m_shared_cores; core++) {
		printf("[%.4f,%.4f,%.4f,%.4f,%.4f,%.2f] ", occupancy[core], target_occupancy[core], potential_gain[core], total_gain, target_occupancy_u[core], m_eviction_prob[core]);
            }
	    printf("\n");
	    for(UInt32 core = 0; core < m_shared_cores; core++) {
		printf("%u,", m_shared_miss_counter[core]);
            }
	    printf("\n");

        }

        // Reset counters
        m_miss_counter = 0;
        for(UInt32 core = 0; core < m_shared_cores; core++) {
#if 0
	    corelocks[core].acquire();
#endif
            m_shared_hit_counter[core] = 0;
            m_shared_miss_counter[core] = 0;
            // and releasing them here
            corelocks[core].release();
        }
        resetCounters();
    }
    lock.release();
}

UInt32
CacheSetInfoPriSM::getReplacementTarget()
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

        // If every core has either 0 or 1 probability this will keep looping. In that case we just return a invalid core, the replacement algorithm will still work correctly
        if (core==start) {
            return m_shared_cores;
        }
    }
 //   printf("%d %f\n", core, m_eviction_prob[core]);
 //   assert(false);
    return core;
}

bool
CacheSetInfoPriSM::canEvict(core_id_t core)
{
    return m_eviction_prob[core] > 0;
}


bool
CacheSetInfoPriSM::mustEvict(core_id_t core)
{
    return m_eviction_prob[core] == 1.;
}

void
CacheSetInfoPriSM::incrementCoreCounter(core_id_t core_id)
{
    corelocks[core_id].acquire();
    m_core_usage_counter[core_id]++;
    corelocks[core_id].release();
    m_core_total_usage++;
}

void
CacheSetInfoPriSM::decrementCoreCounter(core_id_t core_id)
{

    corelocks[core_id].acquire();
    assert(m_core_usage_counter[core_id] > 0);
    m_core_usage_counter[core_id]--;
    corelocks[core_id].release();
    m_core_total_usage--;
}

void
CacheSetInfoPriSM::incrementHitCounter(core_id_t core_id)
{
    corelocks[core_id].acquire();
    m_shared_hit_counter[core_id]++;
    corelocks[core_id].release();
}

void
CacheSetInfoPriSM::incrementMissCounter(core_id_t core_id)
{
    corelocks[core_id].acquire();
    m_shared_miss_counter[core_id]++;
    corelocks[core_id].release();
    lock.acquire();
    m_miss_counter++;
    lock.release();
}


CacheSetPriSM::CacheSetPriSM(
    CacheBase::cache_t cache_type,
    UInt32 associativity, UInt32 blocksize, UInt32 set_no, CacheSetInfoPriSM *info)
    : CacheSetShadow(cache_type, associativity, blocksize, info)
    , m_info(info)
{
    m_lru_bits = new UInt8[m_associativity];
    for (UInt32 i = 0; i < m_associativity; i++) {
        m_lru_bits[i] = i;
    }
}

CacheSetPriSM::~CacheSetPriSM()
{
    delete[] m_lru_bits;
}

UInt32
CacheSetPriSM::getReplacementIndex(CacheCntlr *cntlr, Core::ExtraMemoryRequestInfo *info, UInt32 &setidx, IntPtr addr)
{
    m_info->accessCache(info->core_id, info->address, false);
    m_info->incrementMissCounter(info->core_id);

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
	m_info->not_founds++;
        replacement_index = max_other_pos;
    } else {
        replacement_index = lru_pos;
	if(target_core < m_info->m_shared_cores){
		m_info->not_founds++;
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
CacheSetPriSM::updateReplacementIndex(UInt32 accessed_index, Core::ExtraMemoryRequestInfo *info)
{
    m_info->accessCache(info->core_id, info->address, true);
    m_info->incrementHitCounter(info->core_id);

    moveToMRU(accessed_index);
}

void
CacheSetPriSM::moveToMRU(UInt32 accessed_index)
{
    for (UInt32 i = 0; i < m_associativity; i++)
    {
        if (m_lru_bits[i] < m_lru_bits[accessed_index])
            m_lru_bits[i]++;
    }
    m_lru_bits[accessed_index] = 0;
}
