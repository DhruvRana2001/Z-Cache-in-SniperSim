#include "cache_set_skew.h"
#include "log.h"
#include "stats.h"
#include "utils.h"
#include "meh.h"

// #define SKEW_HASH
CacheSetSkew::CacheSetSkew(CacheBase::cache_t cache_type,
                           UInt32 associativity, UInt32 blocksize, CacheSetInfoSkew* set_info, CacheSet** sets, UInt32 num_sets, UInt32 logblk, UInt32 set_no, UInt32 num_cores)
        : CacheSet(cache_type, associativity, blocksize),
        m_num_sets(num_sets),
        m_log_blocksize(logblk),
        m_num_cores(num_cores)
{
        setarray = sets;
        this->set_no = set_no;
        // We keep global cache variables in set 0. This is nasty, but hey after
        // all it was not me who developed sniper and assumed set-based replacement policies...
        if(set_no == 0) {
                printf("Creating Skew-associative global state\n");
                counter = 0;
                statussen = new VantageStatus[m_num_cores + 1];
                #if 0
		umon = new skewUMON*[m_num_cores];
                // create vantage states and UMON's for each core.
                for(UInt32 i=0; i<m_num_cores+1; i++) {
                        statussen[i] = VantageStatus();
                        if(i<m_num_cores)
                                umon[i] = skewUMON(m_associativity, m_num_sets, m_num_cores);
                }
		#endif
        }
        timestamps = new UInt32[m_associativity];
        partitions = new UInt32[m_associativity];

        for(UInt32 i=0; i<m_associativity; i++)
                timestamps[i] = 0;

        // TAKEN FROM RUNAR: Create a time converter used to convert elapsed time to cycles. ASSUMES CONSTANT FREQ
        //const ComponentPeriod* period = Sim()->getDvfsManager()->getCoreDomain(Sim()->getDvfsManager()->getCoreDomainId(0));
        //m_time_converter = new SubsecondTimeCycleConverter(period);
}


CacheSetSkew::~CacheSetSkew()
{
        delete [] timestamps;
        delete [] partitions;
        //delete m_time_converter;
}

UInt32 CacheSetSkew::hashedIndex(IntPtr addr, UInt32 way, UInt32 m_num_sets, UInt32 m_log_blocksize){
//#ifdef SKEW_HASH
        int N = floorLog2(m_num_sets);
        UInt32 mask = m_num_sets - 1;
        UInt32 A1 = (addr >> m_log_blocksize) & mask;
        UInt32 A2 = (addr >> (m_log_blocksize + N)) & mask;
        // Now we'll need to bit rotate assuming an N-bits int
        for(unsigned int i=0; i<way; i++) {
                IntPtr tmp = (A1 << 1) & mask;
                A1 = tmp | (A1 >> (N-1));
        }
        return A2 ^ A1;
#if 0
        // H3-hash the tag, not the full address.
        IntPtr tmp_addr = addr >> 6;
        UInt32 idx = 0;
        for(UInt32 i=0; i<64-(m_log_blocksize); i++) {
                idx ^= Q_H32[way*(64-(m_log_blocksize))+i]*(tmp_addr & 0x1);
                tmp_addr >>= 1;
        }
        return idx;
#endif
}

UInt32 CacheSetSkew::getReplacementIndex(CacheCntlr *cntlr, Core::ExtraMemoryRequestInfo *info, UInt32 &setidx, IntPtr addr){ //, bool update){
#ifndef VANTAGE
        // At first all our timestamps are zero. We don't care where it goes
        UInt32 maxtime = 0;
        UInt32 index = 0;
        UInt32 tmp = 0;

        CacheSetSkew* root = dynamic_cast<CacheSetSkew*>(setarray[0]);

        Lock* lck = root->getCounterLock();
        lck->acquire();
        UInt32 curTime = root->counter;
        lck->release();

        for(unsigned int i=0; i<m_associativity; i++) {
                tmp = hashedIndex(addr, i, m_num_sets, m_log_blocksize);
                // <= instead of < to make sure we always make at least one correct update
                // to the setidx.
                if(maxtime <= (curTime - dynamic_cast<CacheSetSkew*>(setarray[tmp])->timestamps[i])) {
                        index = i;
                        setidx = tmp;
                        maxtime =  curTime - dynamic_cast<CacheSetSkew*>(setarray[tmp])->timestamps[i];
                }
        }
        // To find the correct set for insertion, getReplacementIndex is called an additional time
        // to prevent double updating, we perform this check.
        //if(update){
        setarray[setidx]->updateReplacementIndex(index, info);
        //}
        return index;
#else
/* Vantage LRU:
 * Inspect all candidates and possibly demote them (NOT EVICT)
 * Evict oldest item in Unmanaged region
 */
// At first all our timestamps are zero. We don't care where it goes
        UInt32 maxtime = 0;
        UInt32 index = 0;
        UInt32 tmp = 0;

        CacheSetSkew* root = dynamic_cast<CacheSetSkew*>(setarray[0])->getCounterLock();
        // Perform the demotion process
        for(unsigned int i=0; i<m_associativity; i++) {
                // calculate set
                tmp = hashedIndex(addr, i, m_num_sets, m_log_blocksize);
                // now find partition and lock it's current timer.
                CacheSetSkew* curSet = dynamic_cast<CacheSetSkew*>(setarray[tmp]);
                UInt32 part =  curSet->partitions[i];
                VantageStatus* stat = root->statussen[part];
                Lock* lck = stat->counterLock;
                lck.acquire();
                if(stat->ActualSize > stat->TargetSize) {

                        if(curSet->timestamps[i] < stat->SetPointTs || curSet->timestamps[i] > stat->CurrentTs) {
                                // demote candidate
                                curSet->partitions[i] = m_num_cores;
                                // TODO: fix race here
                                curSet->timestamps[i] = root->statussen[m_num_cores+1]->CurrentTs;
                                stat->ActualSize--;
                                stat->CandsDemoted++;
                                stat->CandsSeen++;
                        }
                }
                lck.release();
        }
        // Now find our eviction candidate;
#endif
}

// This one is assumed to be called only once on the set that was actually modified!
// This is why getReplacementIndex also returns a set_idx
void CacheSetSkew::updateReplacementIndex(UInt32 accessed_index, Core::ExtraMemoryRequestInfo *info){
#ifndef VANTAGE
        // increase _global_ counter
        CacheSetSkew* root = dynamic_cast<CacheSetSkew*>(setarray[0]);
        Lock* lck = root->getCounterLock();
        // Critical section
        lck->acquire();
        timestamps[accessed_index] = root->counter;
        root->counter++;
        lck->release();
        // increment access counter for this set
        //root->umon[info->core_id]->update(set_no, accessed_index);
#else
        CacheSetSkew* root = dynamic_cast<CacheSetSkew*>(setarray[0]);
        UInt32 part = partitions[accessed_index];
        // get the lock corresponding to this partition
        Lock* lck = set->statussen[part]->counterLock;
        lck->acquire();
        VantageStatus* stat = set->statussen[part];
        // update the timestamp
        timestamps[accessed_index] = stat->CurrentTs;
        stat->CurrentTs++;
        stat->AccessCounter++;
        if(stat->AccessCounter == stat->ActualSize/16) {
                stat->AccessCounter = 0;
                stat->CurrentTs++;
                stat->SetPointTs++;
        }
        // unmanaged partition, promotion
        if(part == m_num_cores) {
                stat->ActualSize++;
                // set partition to the current core
                partitions[accessed_index] = core_id;
        }
#endif
}
#if 0
/* Get marginal utility when going from a units to b units
 * Vantage paper use DSS with 64 sets. Doing all the sets
 * is not feasible due to runtime.
 * UCP paper
 */
UInt32 CacheSetSkew::getMU(UInt32 core, UInt32 a, UInt32 b){
  CacheSetSkew* root = dynamic_cast<CacheSetSkew*>(setarray[0]);
  skewUMON* umon = root->umon[core];
  // First sum all of them. Only then interpolate! (Works due to linearity)
  UInt32 interpp = m_num_sets/64;
  UInt64 acc[m_associativity];
  for(UInt32 i=0; i<64; i++)
    for(UInt32 j=0; j<m_associativity; j++){
      acc[j]+= umon->getAccessCounter(i*interpp, j)
    }
  }
  // TODO: linear interpolation
  UInt32 ret = 0;
  for(UInt32 i=a; i<b; i++){
    acc[i] /= 64;
    ret += acc[i];
  }
  return ret;
}

UInt64 CacheSetSkew::getMaxMU(UInt32 core, UInt32 allocated, UInt32 balance, UInt32& additional_ways){
  UInt64 maxU = 0;
  UInt64 additional_ways = 0;
  UInt64 tmp;
  for(UInt32 i=1; i<balance; i++){
      if(maxU < (tmp = getMU(core, allocated, allocated+i))){
        maxU = tmp;
        additional_ways = i;
      }
  }
  return maxU;
}

void CacheSetSkew::reallocate(void){
  // Look-ahead algorithm
  UInt64 balance = m_num_sets-m_num_cores;
  CacheSetSkew* root = dynamic_cast<CacheSetSkew*>(setarray[0]);

  // Allocate 1 way for each core
  // TODO: 1 unit (based on the interpolation)
  for(UInt32 i=0; i<m_num_cores; i++){
    root->[i]->allocs = 1;
  }

  while(balance){
    UInt64 max_mu = 0;
    UInt32 additional_ways = 0;
    UInt32 min_blocks = 0;
    UInt32 winning_core=-1;

    for(UInt32 i=0; i<m_num_cores; i++){
        UInt32 alloc = root->umon[i]->allocs;
        UInt64 tmp;
        if(max_mu < (tmp=getMaxMU(i, alloc, balance, additional_ways))){
          max_mu = tmp;
          min_blocks = additional_ways;
          winning_core = i;
        }
    }
    balance -= min_blocks;
    root->umon[winning_core]->allocs++;
  }
}
#endif
