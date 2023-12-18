#ifndef CACHE_SET_SKEW_H
#define CACHE_SET_SKEW_H

#include "cache_set.h"

class CacheSetInfoSkew : public CacheSetInfo
{

	public:
	private:

};
class VantageStatus {
	UInt32 CurrentTs;
	UInt32 AccessCounter;
	UInt32 ActualSize;
	UInt32 TargetSize;
	UInt32 SetPointTs;
	UInt32 CandsSeen;
	UInt32 CandsDemoted;
	Lock counterLock;
};

class skewUMON {
	  // counters gaan per way
		UInt64** accessCounters;

		public:
			skewUMON(UInt32 associativity, UInt32 num_sets, UInt32 num_cores) :
			m_associativity(associativity),
			m_num_sets(num_sets),
			m_num_cores(num_cores)
			{
				accessCounters = new UInt64*[m_num_sets];
				//allocs = new UInt64[m_num_cores];
				for(UInt32 i=0; i<m_num_sets; i++){
					accessCounters[i] = new UInt64[m_associativity];
					for(UInt32 j=0; j<m_associativity; j++){
						accessCounters[i][j] = 0;
					}
				}
			}

			~skewUMON(){
				delete [] accessCounters;
			}

			UInt64 allocs;

			void update(UInt32 set_no, UInt32 index){
				accessCounters[set_no][index]++;
			}

			UInt64 getAccessCounter(UInt32 set, UInt32 way){ return accessCounters[set][way]; }

		protected:
			const UInt32 m_associativity;
			const UInt32 m_num_sets;
			const UInt32 m_num_cores;
};

class CacheSetSkew : public CacheSet
{
   public:
       CacheSetSkew(CacheBase::cache_t cache_type,
            UInt32 associativity, UInt32 blocksize, CacheSetInfoSkew* set_info, CacheSet** sets, UInt32 num_sets, UInt32 m_log_blocksize, UInt32 set_no, UInt32 num_cores);
     	virtual	~CacheSetSkew();
     	static UInt32 hashedIndex(IntPtr addr, UInt32 way, UInt32 m_num_sets, UInt32 m_log_blocksize);
	virtual  UInt32 getReplacementIndex(CacheCntlr *cntlr, Core::ExtraMemoryRequestInfo *info, UInt32 &setidx, IntPtr addr);
	void updateReplacementIndex(UInt32 accessed_index, Core::ExtraMemoryRequestInfo *info);
	Lock* getCounterLock() { return &counter_lock; }
	UInt32* timestamps;
	UInt32* partitions;
	UInt32 counter;


   protected:
	CacheSetInfoSkew* m_set_info;
	CacheSet** setarray;
	skewUMON* umon;
	Lock counter_lock;
	VantageStatus* statussen;
	UInt32 set_no;
	const UInt32 m_num_sets;
	const UInt32 m_log_blocksize;
	const UInt32 m_num_cores;
	// Taken from Runar
	SubsecondTimeCycleConverter* m_time_converter;
	UInt64 m_current_period;
	UInt64 m_reallocate_period;
	UInt32 getCurrentAllocationPeriod() {
		 //SubsecondTime time = Sim()->getCoreManager()->getCoreFromID(0)->getPerformanceModel()->getElapsedTime();
		 //return m_time_converter->subsecondTimeToCycles(time) / m_reallocate_period;
		 return 0;
	}
	friend class CacheSet;
};

#endif /* CACHE_SET_LRU_H */
