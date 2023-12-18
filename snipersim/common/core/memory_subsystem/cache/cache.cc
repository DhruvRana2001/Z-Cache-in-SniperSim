#include "simulator.h"
#include "cache.h"
#include "log.h"
#include "cache_set_skew.h"
#include "cache_set_z.h"
#include <vector>
#include <algorithm>

//#define WITHOUT_VANTAGE
#define printf(...) {; }
#define fprintf(...) {;}
// Cache class
// constructors/destructors
Cache::Cache(
    String name,
    String cfgname,
    core_id_t core_id,
    UInt32 num_sets,
    UInt32 associativity,
    UInt32 cache_block_size,
    String replacement_policy,
    cache_t cache_type,
    UInt32 shared_cores,
    hash_t hash,
    FaultInjector *fault_injector,
    AddressHomeLookup *ahl)
    :
    CacheBase(name, num_sets, associativity, cache_block_size, hash, ahl),
    m_enabled(false),
    m_num_accesses(0),
    m_num_hits(0),
    m_core_id(core_id),
    m_cache_type(cache_type),
    m_fault_injector(fault_injector)
{
	m_shared_cores = shared_cores;
    m_set_info = CacheSet::createCacheSetInfo(name, cfgname, core_id, replacement_policy, m_associativity, num_sets, cache_block_size, shared_cores, hash);
    m_sets = new CacheSet*[m_num_sets];
    for (UInt32 i = 0; i < m_num_sets; i++)
    {
        m_sets[i] = CacheSet::createCacheSet(cfgname, core_id, replacement_policy, m_cache_type, m_associativity, m_blocksize, m_set_info, i, m_sets, m_log_blocksize, m_num_sets, shared_cores);
    }

#ifdef ENABLE_SET_USAGE_HIST
    m_set_usage_hist = new UInt64[m_num_sets];
    for (UInt32 i = 0; i < m_num_sets; i++)
        m_set_usage_hist[i] = 0;
#endif
}

Cache::~Cache()
{
#ifdef ENABLE_SET_USAGE_HIST
    printf("Cache %s set usage:", m_name.c_str());
    for (SInt32 i = 0; i < (SInt32) m_num_sets; i++)
        printf(" %" PRId64, m_set_usage_hist[i]);
    printf("\n");
    delete [] m_set_usage_hist;
#endif

    if (m_set_info)
        delete m_set_info;

    for (SInt32 i = 0; i < (SInt32) m_num_sets; i++)
        delete m_sets[i];
    delete [] m_sets;
}

// Tomwi would need fixing but for some odd reason, commenting
// the whole thing and a succesfull compile indicates it's not used anywhere...

#if 0
Lock&
Cache::getSetLock(IntPtr addr)
{
    IntPtr tag;
    UInt32 set_index;

    splitAddress(addr, tag, set_index);
    assert(set_index < m_num_sets);

    return m_sets[set_index]->getLock();
}
#endif

bool
Cache::invalidateSingleLine(IntPtr addr)
{
    IntPtr tag;
    UInt32 set_index;
    splitAddress(addr, tag, set_index);

    if(m_hash != HASH_SKEW && m_hash != HASH_Z) {
        assert(set_index < m_num_sets);
        return m_sets[set_index]->invalidate(tag);
    }
    // TODO: maybe add way idx to invalidate for a bit more efficiency?
    else if(m_hash == HASH_SKEW) {
        for(unsigned int i=0; i<m_associativity; i++) {
            UInt32 idx = CacheSetSkew::hashedIndex(addr, i, m_num_sets, m_log_blocksize);
            if(m_sets[idx]->invalidate(tag))
                return true;
        }
        return false;
    }
    else if(m_hash == HASH_Z) {
        for(unsigned int i=0; i<m_associativity; i++) {
            UInt32 idx = CacheSetZ::hashedIndex(tag, i, m_log_blocksize);
            if(m_sets[idx]->invalidate(tag))
                return true;
        }
    }
    return false;
}

CacheBlockInfo*
Cache::accessSingleLine(IntPtr addr, access_t access_type,
                        Byte* buff, UInt32 bytes, SubsecondTime now, bool update_replacement, Core::ExtraMemoryRequestInfo *info)
{
    //assert((buff == NULL) == (bytes == 0));

    IntPtr tag;
    UInt32 set_index;
    UInt32 line_index = -1;
    UInt32 block_offset;

    splitAddress(addr, tag, set_index, block_offset);
    CacheSet* set = NULL;
    CacheBlockInfo* cache_block_info=NULL;

    // the set_index as given by splitAddress is valid if the hash is of a mask type.
    if(m_hash != HASH_SKEW && m_hash != HASH_Z) {
        set = m_sets[set_index];
        cache_block_info  = set->find(tag, &line_index);
    }
    // when a skew-associative cache is used, we calculate the set index per way and try to find the line
    else if(m_hash == HASH_SKEW) {
        for(unsigned int i =0; i<m_associativity; i++) {
            UInt32 idx = CacheSetSkew::hashedIndex(addr, i, m_num_sets, m_log_blocksize);
            set = m_sets[idx];
            cache_block_info  = set->find(tag, &line_index);
            // we found our set, stop searching
            if(cache_block_info) {
                break;
            }
        }
    }
    // similar for a z-cache
    else if(m_hash == HASH_Z) {
        for(unsigned int i =0; i<m_associativity; i++) {
            UInt32 idx = CacheSetZ::hashedIndex(tag, i, m_log_blocksize);
            set = m_sets[idx];
            cache_block_info = set->find(tag, &line_index);
            if(cache_block_info) {
        	CacheSetZ* root = dynamic_cast<CacheSetZ*>(m_sets[0]);
	        if(root->vantageEnabled()) {
			// check that the partition corresponds to the core requesting
			IntPtr tmpTag = set->m_cache_block_info_array[line_index]->getTag();
		    }
                break;
            }
        }
    }

    // line is not present
    if (cache_block_info == NULL)
        return NULL;


    // TODO (Tomwi): right now I ignore the fault injector. will break horribly as the incorrect set will be used.
    LOG_ASSERT_ERROR(!m_fault_injector, "Fault injection not supported\n");
    if (access_type == LOAD)
    {
        // NOTE: assumes error occurs in memory. If we want to model bus errors, insert the error into buff instead
        if (m_fault_injector)
            m_fault_injector->preRead(addr, set_index * m_associativity + line_index, bytes, (Byte*)m_sets[set_index]->getDataPtr(line_index, block_offset), now);

        set->read_line(line_index, block_offset, buff, bytes, update_replacement, info);
    }
    else
    {
        set->write_line(line_index, block_offset, buff, bytes, update_replacement, info);

        // NOTE: assumes error occurs in memory. If we want to model bus errors, insert the error into buff instead
        if (m_fault_injector)
            m_fault_injector->postWrite(addr, set_index * m_associativity + line_index, bytes, (Byte*)m_sets[set_index]->getDataPtr(line_index, block_offset), now);
    }

    return cache_block_info;
}

void
Cache::insertSingleLine(IntPtr addr, Byte* fill_buff,
                        bool* eviction, IntPtr* evict_addr,
                        CacheBlockInfo* evict_block_info, Byte* evict_buff,
                        SubsecondTime now, CacheCntlr *cntlr, Core::ExtraMemoryRequestInfo *info, std::vector<SetLock>* hashlocks, std::vector<SetLock>* L1locks, UInt32* extra_locked, UInt32* extra_L1)
{
    IntPtr tag;
    UInt32 set_index;
    splitAddress(addr, tag, set_index);

    CacheBlockInfo* cache_block_info = CacheBlockInfo::create(m_cache_type);
    cache_block_info->setTag(tag);
    cache_block_info->setOwner(info != NULL ? info->core_id : 9000);

    // We have to do a relocation before we can do a true insertion
    // our walk can be done having only the m_associativity locks, which
    // were already locked by the L1-cache.
    // however as soon as we start the relocation we HAVE TO lock ALL
    // involved locations in the walk.
    UInt32 way, candidate, insertway;
    if(m_hash == HASH_Z) {
        CacheSetZ* set = dynamic_cast<CacheSetZ*>(m_sets[0]);
        UInt32 walk[set->candidates_per_way*m_associativity];
        bool walkv[set->candidates_per_way*m_associativity];
        uint32_t onStack = 0;

        if(!set->vantageEnabled()) {
            onStack = set->getLevels()*2;
        }
        else {
            onStack = (set->candidates_per_way*m_associativity)*2;
        }
        uint32_t backtracked[onStack];
        UInt32 pathlength;
        // The walk (BFS), gets the way and the index into the walk array
        insertway = set->getReplacementIndex(cntlr, info, way, addr, candidate, walk, walkv, set_index, backtracked, pathlength);

        // Since it is impossible to know which candidates we'll discover, it is impossible
        // to lock them advance. In case of vantage we therefore need another function that
        // finds the _real_ replacement candidate

        if(!set->vantageEnabled()) {
            printf("According to cache.cc we will insert at set %x\n", set_index);
        }
        else {
           // fprintf(stdout, "Vantage wants to lock %d entries\n", pathlength);
        }

        std::vector<UInt32> uniqueWalk[m_associativity];
        std::fill(uniqueWalk, uniqueWalk + m_associativity, std::vector<UInt32>());
        std::vector<UInt32>::iterator it;
        // We need to sort our locks way-wise before locking.

        if(!set->vantageEnabled()) {
        // When vantage is not used, we only lock the entries that lie on our path.
        // Do not process the root lock. We'll lock those seperately
        // as cache_cntlr.cc did a releaseStackLock and needs to have them
        // again.
            for(int32_t i =0; i<(int32_t)(pathlength-1); i++) {
                UInt32 tmpway =backtracked[i*2+1];
                uniqueWalk[tmpway].push_back(backtracked[i*2]);
               // fprintf(stdout, "Adding %x %x %x\n", tmpway, backtracked[i*2], i);
            }
        }
        else {
        // In case of vantage ALL entries in the backtrack array
        // are not equal to the root. This means we need to lock all LLC entries
        // as listed
            for(int32_t i =0; i<(int32_t)(pathlength); i++) {
                bool alreadyLocked = false;
                for(uint32_t j=0; j<i; j++) {
                    if(backtracked[i*2]==backtracked[j*2] && backtracked[i*2+1] == backtracked[j*2+1]) {
                        alreadyLocked=true;
                        break;
                    }
                }
                if(!alreadyLocked) {
                    // check that it is not a root entry
                    uint32_t tmp = CacheSetZ::hashedIndex(addr >> 6, backtracked[i*2+1], 6);
                    if(tmp!=backtracked[i*2]) {
                        UInt32 tmpway =backtracked[i*2+1];
                        uniqueWalk[tmpway].push_back(backtracked[i*2]);
                      //  fprintf(stdout, "Vadding %x %x %x\n", tmpway, backtracked[i*2], i);
                    }
                }
                else {
                    assert(false);
                }
            }
        }

        // Now we have found all our locks, we start locking in way-order
        for(UInt32 i = 0; i<m_associativity; i++) {
            // lock root lock for this way, TODO: fix hardocded m_log_blocksize
            uint32_t tmp = CacheSetZ::hashedIndex(addr >> 6, i, 6);
            hashlocks->at(tmp + i * getNumSets()).acquire_exclusive();
          //  fprintf(stdout, "locked %x %x\n", tmp, i);
            // Now process additional locks in this way
            it = uniqueWalk[i].begin();
            // lock all entries in this way
            while(it != uniqueWalk[i].end()) {
               // fprintf(stdout, "Now locking way %x and set %x\n", i, *it);
                hashlocks->at(*it+ i * getNumSets()).acquire_exclusive();
                it++;
            }
        }

        // retake L1/L2 locks as we would do in the controller order
        // At this point we have the hash-locks corresponding to the L3 cache
        // However not taking the locks for the corresponding lines in the L1/L2 cache
        // can eventually lead to some race conditions (see Thesis)
        // Some locks in the L3 might have items in them that
        // map to the same L1/L2 lines, so we need to check for doubles... again
        // This is the right place to inspect the tags at the locations locked by the hash-locks:
        // as at this point it is not possible for any other cache to evict those items (that would mean they already acquired
        // all hash-locks, and then we would not be here)

        // Re-lock our L1-set corresponding to the issued address.
        L1locks->at((addr >> m_log_blocksize) & (64-1)).acquire_exclusive();

        std::vector<UInt32> uniqueL1;
        // Since all LLC locks are in place we can already search for our vantage-replacement candidate.
        if(set->vantageEnabled()) {
            insertway = set->getVantageReplacementIndex(walk, walkv, candidate, set_index, way, info);
        }
        CacheSetZ* root = dynamic_cast<CacheSetZ*>(m_sets[0]);
        uint32_t candidates_per_way = root->candidates_per_way;
	uint32_t levels = root->getLevels();
        // Now block all other outstanding operations on the relocation path.
        // This is done by taking the corresponding l1-locks.
        // since all LLC locks are in place, it is safe to use the tag in
        // the index calculations.
        uint32_t walkidx = candidates_per_way*way + candidate;
 	uint32_t walkway = way;
	for(uint32_t nivo = 0; nivo<levels; nivo++){
		printf("walkidx = %x, %x\n", walkidx, walk[walkidx]);
	        CacheSetZ*  reloc = dynamic_cast<CacheSetZ*>(m_sets[walk[walkidx]]);
        	IntPtr tmpTag = reloc->m_cache_block_info_array[walkway]->getTag();
		if(tmpTag != ((IntPtr)~0)){
			/* TODO: fix hardcoded blocksize */
			if(std::find(uniqueL1.begin(), uniqueL1.end(), (tmpTag & 63)) == uniqueL1.end()) {
				// If it doesn't map to the same L1 set as the issued address, lock that set.
        			if((tmpTag & 63) != ((addr >> 6 ) & 63)) {
        		  		L1locks->at(tmpTag & 63).acquire_exclusive();
          				uniqueL1.push_back(tmpTag & 63);
        			}
			}
		}
		walkidx = reloc->m_set_info->LUT[walkidx];
		walkway = walkidx/candidates_per_way;  
	}

        // Now vantage has selected its replacement candidate as well (if enabled)
        // Now we can relocate, which will do the eviction for us.
        set->doReplacement(walk, walkv, way, candidate, eviction, evict_block_info, evict_buff,(info ? info->core_id : -9000));
        // The cache controller needs to know which locks were additionally locked during the replacement process
        // since it is the cache controller that performs the invalidation of the evicted candidate, and until then
        // no processor should be able to access the corresponding locations.
        for(UInt32 i = 0; i<m_associativity; i++) {
            it = uniqueWalk[i].begin();
            uint32_t cnt = 0;
            while(it != uniqueWalk[i].end()) {
                extra_locked[i*set->candidates_per_way+cnt] = (*it + i * getNumSets());
                cnt++;
                it++;
            }
            // put invalid indices in all empty slots
            for(; cnt < set->candidates_per_way; cnt++) {
                extra_locked[i*set->candidates_per_way+cnt] = 0xFFFFFFFF;
            }
        }
        printf("Now recording the extra locks\n");
        // Do the same for the L1/L2 locks
        it=uniqueL1.begin();
        uint32_t cnt = 0;
        while(it != uniqueL1.end()) {
            extra_L1[cnt] = *it;
            printf("Added lock for %x\n", *it);
            cnt++;
            it++;
        }
        printf("cache.cc: cnt = %d\n", cnt);
        // put invalid indices in all empty slots
        uint32_t numL1Locks;
        if(!set->vantageEnabled()) {
            numL1Locks = set->getLevels();
        }
        else {
            numL1Locks = m_associativity*set->candidates_per_way;
        }
        for(; cnt < numL1Locks; cnt++) {
            extra_L1[cnt] = 0xFFFFFFFF;
        }
    }

    if(m_hash != HASH_Z) {
        m_sets[set_index]->insert(cache_block_info, fill_buff,
                                  eviction, evict_block_info, evict_buff, addr, m_num_sets, m_log_blocksize, cntlr, info, m_sets, m_hash);
    }
    // From here on we have an available spot in the Z-cache. We can therefore skip evicition and directly insert
    // at this spot (which is guaranteed to be still exclusively locked as we only released our additional levels of locks
    // TODO: add argument to insert to indicate the set_index and way for insertion.
    else {
        //printf("Doing insertion from cache.cc at way %x and index %x\n", way, set_index);
        printf("calling insert with %x %x core %x\n", insertway, set_index, info->core_id);
        m_sets[set_index]->insert(cache_block_info, fill_buff,
                                  NULL, NULL, NULL, addr, m_num_sets, m_log_blocksize, cntlr, info, m_sets, m_hash, insertway, set_index);
      //  fprintf(stdout, "Now belongs to partition %d\n", dynamic_cast<CacheSetZ*>(m_sets[set_index])->vPartitions[insertway]);
    }
    printf("evicition address tag %lx\n", tagToAddress(evict_block_info->getTag()));
    *evict_addr = tagToAddress(evict_block_info->getTag());

    if (m_fault_injector) {
        // NOTE: no callback is generated for read of evicted data
        UInt32 line_index = -1;
        __attribute__((unused)) CacheBlockInfo* res = m_sets[set_index]->find(tag, &line_index);
        LOG_ASSERT_ERROR(res != NULL, "Inserted line no longer there?");

        m_fault_injector->postWrite(addr, set_index * m_associativity + line_index, m_sets[set_index]->getBlockSize(), (Byte*)m_sets[set_index]->getDataPtr(line_index, 0), now);
    }

#ifdef ENABLE_SET_USAGE_HIST
    ++m_set_usage_hist[set_index];
#endif

    delete cache_block_info;
}


// Single line cache access at addr
CacheBlockInfo*
Cache::peekSingleLine(IntPtr addr)
{
    UInt32 set_index;
    IntPtr tag;
    splitAddress(addr, tag, set_index);

    // splitAdress returns the correct set_index for mask based hashes
    if(m_hash != HASH_SKEW &&  m_hash!= HASH_Z) {
        return m_sets[set_index]->find(tag);
    }
    // calculate set_index per way
    else if(m_hash == HASH_SKEW) {
        CacheSet* set = NULL;
        for(unsigned int i =0; i<m_associativity; i++) {
            UInt32 idx = CacheSetSkew::hashedIndex(addr, i, m_num_sets, m_log_blocksize);
            set = m_sets[idx];
            if(set->find(tag)) {
                return set->find(tag);
            }
        }
        return NULL;
    }
    // Z-cache
    CacheSet* set = NULL;
    //printf("Zcache peek for %lx with tag %lx\n", addr, tag);
    for(unsigned int i =0; i<m_associativity; i++) {
        UInt32 idx = CacheSetZ::hashedIndex(tag, i, m_log_blocksize);
        set = m_sets[idx];
        if(set->find(tag)) {
            printf("Found %lx [%lx] at set_index %x\n", addr, tag, idx);
            return set->find(tag);
        }
    }
    return NULL;
}



void
Cache::updateCounters(bool cache_hit)
{
    if (m_enabled)
    {
        m_num_accesses ++;
        if (cache_hit)
            m_num_hits ++;
    }
}

void
Cache::updateHits(Core::mem_op_t mem_op_type, UInt64 hits)
{
    if (m_enabled)
    {
        m_num_accesses += hits;
        m_num_hits += hits;
    }
}
