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
ubyte mcsl::_File::g.isInit = []()-> ubyte { mcsl::_File::globalSetup(); return mcsl::_File::g.isInit; }();

//error levels (how dangereous an error is)
enum class ERR : ubyte {
   UNEXP = 0,
   MINOR = 1,
   MAJOR = 2,
   FATAL = 3,

   RESC_IN_USE = MAJOR, //resource in use
   OUT_OF_RESC = MAJOR, //out of resources
   CHANGE_FILE = MAJOR, //file(s) would need to be created/deleted
   CHANGE_FDES = FATAL, //file descriptor(s) would need to be reopened
};
//array mapping syscall errno codes to error levels
//supported syscalls: read, write, open, openat
constexpr static ERR ERROR_LEVELS[256] = {
   [EINTR       ] = ERR::MINOR,
   [EDESTADDRREQ] = ERR::MINOR,
   [EIO         ] = ERR::MINOR,
   [EAGAIN      ] = ERR::MINOR,
#if EAGAIN != EWOULDBLOCK
   [EWOULDBLOCK ] = ERR::MINOR,
#endif


   [EACCES      ] = ERR::MAJOR,
   [EPERM       ] = ERR::MAJOR,
   [EDQUOT      ] = ERR::MAJOR,

   [EEXIST      ] = ERR::CHANGE_FILE,
   [ELOOP       ] = ERR::CHANGE_FILE,
   [EISDIR      ] = ERR::CHANGE_FILE,
   [ENOENT      ] = ERR::CHANGE_FILE,
   [EOVERFLOW   ] = ERR::CHANGE_FILE,

   [EBUSY       ] = ERR::OUT_OF_RESC,
   [EMFILE      ] = ERR::OUT_OF_RESC,
   [ENFILE      ] = ERR::OUT_OF_RESC,
   [ENOMEM      ] = ERR::OUT_OF_RESC,
   [ENOSPC      ] = ERR::OUT_OF_RESC,

   [ENODEV      ] = ERR::RESC_IN_USE,
   [ENXIO       ] = ERR::RESC_IN_USE,
   [ETXTBSY     ] = ERR::RESC_IN_USE,


   [EBADF       ] = ERR::FATAL,
   [EFAULT      ] = ERR::FATAL,
   [EFBIG       ] = ERR::FATAL,
   [EINVAL      ] = ERR::FATAL,
   [ENAMETOOLONG] = ERR::FATAL,
   [EOPNOTSUPP  ] = ERR::FATAL,
   [EROFS       ] = ERR::FATAL,

   [ENOTDIR     ] = ERR::CHANGE_FDES,
   [EPIPE       ] = ERR::CHANGE_FDES,
};
constexpr static uint TRIES_MAP[] = {
   [UNEXP] = 0,
   [MINOR] = FILE_TRIES_HARD_CAP,
   [MAJOR] = FILE_TRIES_SOFT_CAP,
   [FATAL] = 0
};
constexpr getTries(Errno err) {
   return TRIES_MAP[ERROR_LEVELS[err]];
}

#pragma region rdwr
#pragma region rdwrImpl
sint mcsl::_File::_read(mcsl::arr_span<ubyte> data) {
   ubyte* dest = data.begin();
   sint rem = data.size();
   sint count = 0;

   sint tries = 0;
   sint maxTries = 0;

   //core read loop
   do {
      ++tries;

      //read
      sint res = read(fd, dest, rem);
      //handle results
      if (res < 0) { //error
         err = res = errno;
         maxTries = getTries(err);
      }
      else if (res == 0) { //eof
         _flags |= FileFlags::SAW_EOF;

         maxTries = 0;
      }
      else { //successful read
         rem -= res;
         dest += res;
         count += res;

         _flags &= ~FileFlags::SAW_EOF;

         maxTries = FILE_PARTIAL_RDRW_CAP;
      }
   } while (rem > 0 && tries <= maxTries);
   return count;
}
sint mcsl::_File::_write(const mcsl::arr_span<ubyte> data) {
   ubyte* dest = data.begin();
   sint rem = data.size();
   sint count = 0;

   sint tries = 0;
   sint maxTries = 0;

   //core write loop
   do {
      ++tries;

      //write
      sint res = write(fd, dest, rem);
      //handle results
      if (res < 0) { //error
         err = res = errno;
         maxTries = getTries(err);
      }
      else { //successful write
         rem -= res;
         dest += res;
         count += res;

         maxTries = FILE_PARTIAL_RDRW_CAP;
      }
   } while (rem > 0 && tries <= maxTries);
   return count;
}
#pragma endregion rdwrImpl

#pragma endregion rdwr

#pragma region open
mcsl::_File::FileRes mcsl::_File::open(cstr path, FileFlags flags, mode_t createMode) {
   //sanity checks
   if (path) {
      return {.err = Errno::BAD_PATH, .file = nullptr};
   }

   //convert flags to OS flags
   sint osFlags = File_flagsToOS(flags);

   //allocate file
   _File* file;
   if (FileRes res = allocFile(); !res.err) {
      file = res->file;
   } else {
      return res;
   }

   //open file
   sint fd;
   uint tries = 0;
   uint maxTries;
   do {
      ++tries;
      //try to open
      fd = open(path.begin(), osFlags, createMode);
      if (fd >= 0) { //check for success
         [[likely]];
         //return
         return file->_open(fd, flags, osFlags);
      }
      [[unlikely]];
      err = errno;
      maxTries = getTries(err);
   } while (tries <= maxTries);

   //return (failure)
   return {.err = err, .file = nullptr};
}
mcsl::_File::FileRes mcsl::_File::open(sint fd, FileFlags flags, sint osFlags) {
   //allocate file
   FileRes res = allocFile();
   if (+res.err) { //error handling
      return res;
   }
   //goto implementation function
   return res.file->_open(fd, flags, osFlags);
}
mcsl::_File::FileRes mcsl::_File::_open(sint fd, FileFlags flags, sint osFlags) {
   //update fields
   this->fd = fd;
   this->_flags = flags;
   this->_osFlags = osFlags;
   this->err = Errno::NO_ERR;
   //return
   return {.err = Errno::NO_ERR, .file = this};
}
#pragma endregion open
#pragma region close
mcsl::Errno mcsl::_File::close() {
   //check state
   if (!(this->flags & FileFlags::IS_OPEN)) {
      this->err = Errno::BAD_FILE_STATE;
      return Errno::BAD_FILE_STATE;
   }

   //flush
   if (this->flags & FileFlags::WRITE) {
      if (Errno err = flush()) { return err; }
   }
   //sync
   if (Errno err = sync()) { return err; }

   sint res = close(this->fd);
   // mark the file as closed, regardless of the results of the close syscall
   // the `close` syscall puts the file descriptor back in the pool of available file descriptors before checking for errors
   // any actionable errors will be caught when trying to flush the file
   // the above only running for write-capable files should not cause any issues
   // the errors that can be returned by a `close` call should only be relevant because they can indicate that there was data that didn't get written to disk, which is not a thing for non-write-capable files
   // for further details, see the man page for `close`
   this->flags |= FileFlags::IS_CLOSED;
   this->flags &= ~FileFlags::IS_OPEN;

   //return
   return freeFile(this);
}
#pragma endregion close

sint mcsl::flagsToOS(FileFlags flags) {
   sint osFlags = 0;
   sint tmp;

   // file type
   tmp = __builtin_popcount(flags & FileFlags::FILE_TYPES);
   if (tmp > 1) {
      TODO;
   }
   tmp = flags & FileFlag::FILE_TYPES;
   switch (tmp) {
      case FileFlags::REGFILE  : break;
      case FileFlags::TMPFILE  : 
         osFlags |= O_TMPFILE;
         if (!(flags & (FileFlags::READ | FileFlags::WRITE))) {
            TODO();
         }
         break;
      case FileFlags::DIRECTORY: osFlags |= O_DIRECTORY; break;
      case FileFlags::FIFO     : break;
      case FileFlags::SOCKET   : break;

      default: UNREACHABLE;
   }

   // access mode
   if (flags & FileFlags::PATH) {
      osFlags |= O_PATH;
      if (flags & (READ | WRITE)) {
         TODO();
      }
   }
   else if (flags & FileFlags::WRITE) {
      if (flags & FileFlags::READ) {
         osFlags |= O_RDWR;
      } else {
         osFlags |= O_WRONLY;
      }
   } else if (flags & FileFlags::READ) {
      osFlags |= O_RDONLY;
   }

   // file existance reqs
   if (flags & FileFlags::ERR::IF_EX) {
      osFlags |= O_CREAT | O_EXCL;
      
      if (flags & FileFlags::ERR::IF_NE) {
         TODO;
      }
   }
   else if (!(flags & FileFlags::ERR::IF_NE)) {
      osFlags |= O_CREAT;
   }

   // append
   if (flags & FileFlags::APPEND) {
      osFlags |= O_APPEND;
   }

   // IO mode
   if (flags & FileFlags::ASYNC) {
      osFlags |= O_ASYNC;
   }
   if (flags & FileFlags::NONBLOCK) {
      osFlags |= O_NONBLOCK;
   }

   // synchronization mode
   if (flags & FileFlags::FSYNC) {
      osFlags |= O_SYNC;
   }
   else if (flags & FileFlags::DSYNC) {
      osFlags |= O_DSYNC;
   }

   // metadata updates
   if (flags & FileFlags::NO_NEW_METADATA) {
      osFlags |= O_NOATIME;
   }

   // close on exec
   if (!(flags & FileFlags::NO_CLO_EXEC)) {
      osFlags |= O_CLOEXEC;
   }

   // cache effect
   if (flags & FileFlags::MIN_OS_CACHE) {
      osFlags |= O_DIRECT;
   }

   // return
   return osFlags;
}
#pragma region filealloc
mcsl::_File::FileRes mcsl::_File::allocFile() {
   assume(_File::g.isInit);

   //check that there are files available
   if (!g.availLen) {
      return FileRes{.err = Errno::NO__FILES, .file = nullptr};
   }

   //get fnum
   sint fnum = g.avail[0];
   assume(fnum >= 0 && fnum < g.fileBufLen);
   //update fnum list
   g.avail++;
   g.availLen--;
   g.inUseLen++;

   //get file object
   _File* file = g.fileBuf + fnum;
   //update file object fields
   file->magicNum = FILE_MAGIC_NUM;
   file->fnum = fnum;

   file->len = 0;
   file->index = 0;
   file->base = 0;
   file->buf = nullptr;
#if defined(SAFE_MODE)
   file->fd = -1;
   file->err = Errno::NO_ERR;

   file->_flags = 0;
   file->_osFlags = 0;
#endif
   //return
   return {.err = Errno::NO_ERR, .file = file};
}
mcsl::Errno mcsl::_File::freeFile(_File* file) {
   debug_assert(file);
   debug_assert(file->magicNum == FILE_MAGIC_NUM);

   //move file's entry in the inUse list to the back of the inUse section
   {
      sint* target = g.inUse + file->fnum;
      sint* back = g.inUse + g.inUseLen - 1;
      File* backFile = g.fileBuf + *back;

      sint tmp = *back;
      *back = *target;
      *target = tmp;

      file->fnum = *target;
      backFile->fnum = *back;
   }

   //update fnum list
   g.availLen++;
   g.avail--;
   g.inUseLen--;

   //unset magic number
   file->magicNum = ~FILE_MAGIC_NUM;
   //return
   return Errno::NO_ERR;
}
#pragma endregion filealloc
#pragma region global
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

   //initialize avail buffer
   sint* avail = (sint*)ptr;
   for (sint i = 0; i < entries; ++i) {
      avail[i] = i;
   }
   
   //save info and mark initialization as successful
   g = {
      .isInit = true,
      .pageSize = pageSize,
      .data = {ptr, (uint)dataSize},
      
      .fileBuf = {(_File*)((ubyte*)ptr + fmanLen), (uint)entries},

      .inUse = avail,
      .inUseLen = 0,
      .avail = avail,
      .availLen = entries,
   };

   //return
   return;
}
void mcsl::_File::globalCleanup() {
   //iterate over open files
   int*        it = g.inUse;
   int* const end = g.inUse + g.inUseLen;
   for (; it != end; ++it) {
      _File* file = g.fileBuf + *it;

      //close file
      Errno err = file->close();
      if (+err) {
         TODO;
      }
   }

   //unmap global file state memory
   sint res = munmap(g.data.data(), g.data.size());
   if (res) {
      TODO;
   }
   
   // return
   return;
}
#pragma endregion global

#endif //MCSL__FILE_CPP
