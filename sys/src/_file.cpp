#ifndef MCSL__FILE_CPP
#define MCSL__FILE_CPP

#include "file.hpp"

#include "assert.hpp"

#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/resource.h>
#include <sys/stat.h>

#undef NULL

#define MCSL_LARGEST_MULT_GEQ(x, mod) (x + (-x % mod))

//run global setup
ubyte mcsl::_File::dummy = []()-> ubyte { mcsl::_File::globalSetup(); return 0; }();

//error levels (how dangereous an error is)
constexpr ubyte ERR_UNEXP = 0;
constexpr ubyte ERR_MINOR = 1;
constexpr ubyte ERR_MAJOR = 2;
constexpr ubyte ERR_FATAL = 3;
//error levels for read and write syscalls
constexpr static ubyte ERROR_LEVEL_RW[256] = {
   [EINTR       ] = ERR_MINOR,
   [EDESTADDRREQ] = ERR_MINOR,
   [EIO         ] = ERR_MINOR,
   [EAGAIN      ] = ERR_MINOR,
#if EAGAIN != EWOULDBLOCK
   [EWOULDBLOCK ] = ERR_MINOR,
#endif

   [EPERM ] = ERR_MAJOR,
   [ENOSPC] = ERR_MAJOR,
   [EDQUOT] = ERR_MAJOR,

   [EPIPE ] = ERR_FATAL,
   [EBADF ] = ERR_FATAL,
   [EFAULT] = ERR_FATAL,
   [EFBIG ] = ERR_FATAL,
   [EINVAL] = ERR_FATAL,
};

void mcsl::_File::globalSetup() {
   sint err;

   //ensure that partial initialization can be detected
   g.isInit = false;

   //register cleanup function to run atexit
   err = atexit(&globalCleanup);
   if (err) {
      TODO;
   }

   //get system limits 
   struct rlimit fdc;
   err = getrlimit(RLIMIT_NOFILE, &fdc);
   if (err == -1) {
      TODO;
   }
   const uint pageSize = sysconf(_SC_PAGESIZE);

   //determine amount of memory needed for bookkeeping
   const sint entries = fdc.rlim_cur;
   const sint fbufLen = entries * sizeof(_File);
   const sint fmanLen = entries * sizeof(sint);
   sint dataSize = fbufLen + fmanLen;
   assert(dataSize > 0);
   dataSize = MCSL_LARGEST_MULT_GEQ(dataSize, pageSize);
   assert(dataSize > 0);
   assert(dataSize % pageSize == 0);

   //map memory for file buffer
   ubyte* const ptr = (ubyte*)mmap(
      nullptr,
      dataSize,
      PROT_READ | PROT_WRITE,
      MAP_ANONYMOUS | MAP_PRIVATE,
      -1, 0
   );
   if (ptr == MAP_FAILED) {
      TODO; //actual error handling
   }
   
   //save info
   g.data = {ptr, (uint)dataSize};
   g.pageSize = pageSize;
   
   g.inUse = (sint*)ptr;
   g.inUseLen = 0;
   g.avail = (sint*)ptr;
   g.availLen = entries;

   g.fileBuf = {(_File*)ptr + fmanLen, (uint)entries};

   //initialize avail buffer
   for (int i = 0; i < entries; ++i) {
      g.avail[i] = i;
   }

   //mark initialization as successful
   g.isInit = true;
   //return
   return;
}


#endif //MCSL__FILE_CPP
