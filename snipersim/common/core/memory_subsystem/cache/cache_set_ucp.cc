#include <iostream>
#include <sstream>
#include <algorithm>

#include "log.h"
#include "stats.h"
#include "config.hpp"

#include "cache_cntlr.h"

#include "cache_base.h"
#include "cache_set_ucp.h"


CacheSetUCP::CacheSetUCP(
      CacheBase::cache_t cache_type,
      UInt32 associativity, UInt32 blocksize, UInt32 set_no, CacheSetInfoUMON *info) :
   CacheSetUMON(cache_type, associativity, blocksize, info),
   m_set_no(set_no)
{
   m_lru_bits = new UInt8[m_associativity];
   for (UInt32 i = 0; i < m_associativity; i++) {
      m_lru_bits[i] = i;
   }
}

CacheSetUCP::~CacheSetUCP()
{
   delete [] m_lru_bits;
}

UInt32
CacheSetUCP::getReplacementIndex(CacheCntlr *cntlr, Core::ExtraMemoryRequestInfo *info, UInt32 &setidx, IntPtr addr)
{
   LOG_ASSERT_ERROR(info != NULL, "ExtraMemoryRequestInfo not set");
   CacheSetUMON::getReplacementIndex(cntlr, info, setidx, addr);

   UInt32 count = 0;
   // Count number of blocks belonging to requester
   for (UInt32 i = 0; i < m_associativity; i++)
   {
      if (m_cache_block_info_array[i]->getOwner() == UInt64(info->core_id))
      {
         count++;
      }
   }

   // NOTE: While the UMON can allocate us 0 columns, we still need to store somewhere as the cache is inclusive. So we change it to one in that case
   auto allocations = getUMON()->getAllocation(info->core_id);
   allocations = allocations > 0 ? allocations : 1;

   // Should we canabalize our own allocations or steal from an other core
   bool canabalize = count >= allocations;

   // Scan for the first valid replacement starting with the LRU, stepping towards the MRU
   int index = 0;
   int max_bit = 0;

   for (UInt32 i = 0; i < m_associativity; i++)
   {
      if (m_lru_bits[i] > max_bit && isValidReplacement(i))
      {
         bool mine = m_cache_block_info_array[i]->getOwner() == UInt64(info->core_id);
         bool valid = (mine && canabalize) || (!mine && !canabalize);

         if (valid) {
            index = i;
            max_bit = m_lru_bits[i];
         }
      }
   }

   updateReplacementIndex(index, info);
   return index;
}

// Moves the block at accessed_index to MRU
void
CacheSetUCP::updateReplacementIndex(UInt32 accessed_index, Core::ExtraMemoryRequestInfo *info)
{
   CacheSetUMON::updateReplacementIndex(accessed_index, info);

   for (UInt32 i = 0; i < m_associativity; i++)
   {
      if (m_lru_bits[i] < m_lru_bits[accessed_index])
         m_lru_bits[i] ++;
   }
   m_lru_bits[accessed_index] = 0;
}
