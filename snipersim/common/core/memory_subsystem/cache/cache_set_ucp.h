#ifndef CACHE_SET_UCP_H
#define CACHE_SET_UCP_H

#include "cache_set_umon.h"

#include "cache_cntlr.h"



class CacheSetUCP : public CacheSetUMON
{
   public:
      CacheSetUCP(CacheBase::cache_t cache_type,
            UInt32 associativity, UInt32 blocksize, UInt32 set_no, CacheSetInfoUMON *info);
      ~CacheSetUCP();

      UInt32 getReplacementIndex(CacheCntlr *cntl, Core::ExtraMemoryRequestInfo *info, UInt32 &setidx, IntPtr addr);
      void updateReplacementIndex(UInt32 accessed_index, Core::ExtraMemoryRequestInfo *info);

   private:
      void moveToMRU(UInt32 accessed_index);

      UInt8* m_lru_bits;
      UInt32 m_set_no;
};

#endif /* CACHE_SET_UCP_H */
