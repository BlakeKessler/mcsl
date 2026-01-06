#ifndef MCSL__FILE_CPP
#define MCSL__FILE_CPP

#include "file.hpp"

#include "assert.hpp"
#include "mem.hpp"

#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <errno.h>
#include <signal.h>
#include <cstdlib>

#undef NULL

#define MCSL_LARGEST_MULT_GEQ(x, mod) (x + (-x % mod))

//run global setup
bool mcsl::_File::__dummy = mcsl::_File::globalSetup();

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

   NOT_MAPPING = UNEXP, //should not happen because no fd was specified
   NO_SPEC_LOC = UNEXP, //should not happen because no location to map was specified
   BAD_HCPARAM = UNEXP, //should not happen because of other hardcoded parameters
};
constexpr auto operator+(ERR err) { return std::to_underlying(err); }
//array mapping syscall errno codes to error levels for IO-related syscalls
//supported syscalls: read, write, open, openat
constexpr static ERR ERROR_LEVELS_IO[256] = {
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
constexpr static ERR ERROR_LEVELS_MEM[256] = {
   [ENOMEM             ] = ERR::MAJOR,
   
   [EACCES             ] = ERR::NOT_MAPPING,
   [EBADF              ] = ERR::NOT_MAPPING,
   [ENFILE             ] = ERR::NOT_MAPPING,
   [ENODEV             ] = ERR::NOT_MAPPING,
   [EPERM              ] = ERR::NOT_MAPPING,
   [ETXTBSY            ] = ERR::NOT_MAPPING,
   
   [EEXIST             ] = ERR::NO_SPEC_LOC,
   //[MAP_FIXED_NOREPLACE] = ERR::NO_SPEC_LOC,
   [SIGSEGV            ] = ERR::NO_SPEC_LOC,
   [SIGBUS             ] = ERR::NO_SPEC_LOC,
   
   [EAGAIN             ] = ERR::BAD_HCPARAM,
   [EINVAL             ] = ERR::BAD_HCPARAM,
   [EOVERFLOW          ] = ERR::BAD_HCPARAM,
};
constexpr static uint TRIES_MAP[] = {
   [+ERR::UNEXP] = 0,
   [+ERR::MINOR] = FILE_TRIES_HARD_CAP,
   [+ERR::MAJOR] = FILE_TRIES_SOFT_CAP,
   [+ERR::FATAL] = 0
};
constexpr uint getTries_io(Errno err) {
   debug_assert((ulong)err < sizeof(ERROR_LEVELS_IO) / sizeof(ERR));
   return TRIES_MAP[+ERROR_LEVELS_IO[+err]];
}
constexpr uint getTries_mem(Errno err) {
   debug_assert((ulong)err < sizeof(ERROR_LEVELS_MEM) / sizeof(ERR));
   return TRIES_MAP[+ERROR_LEVELS_MEM[+err]];
}

//!NOTE: when implementing setbuf, clear the SAW_EOF flag if left != 0
bool mcsl::_File::eof() {
   return +(_flags & FileFlags::SAW_EOF) && !(_left && +(_flags & FileFlags::BUFFERED));
}

#pragma region rdwr
#pragma region rdwrImpl
//!TODO: calculate appropriate try cap based on the length of the requested read/write
sint mcsl::_File::_read(mcsl::arr_span<ubyte> data) {
   debug_assert(data.begin() && data.size());

   ubyte* dest = data.begin();
   sint rem = data.size();
   sint count = 0;

   sint tries = 0;
   sint maxTries = 0;

   //core read loop
   do {
      ++tries;

      //read
      sint res = ::read(_fd, dest, rem);
      //handle results
      if (res < 0) { //error
         res = errno;
         _err = (Errno)res;
         maxTries = getTries_io(_err);
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
   debug_assert(data.begin() && data.size());

   const ubyte* dest = data.begin();
   sint rem = data.size();
   sint count = 0;

   sint tries = 0;
   sint maxTries = 0;

   //core write loop
   do {
      ++tries;

      //write
      sint res = ::write(_fd, dest, rem);
      //handle results
      if (res < 0) { //error
         res = errno;
         _err = (Errno)res;
         maxTries = getTries_io(_err);
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
sint mcsl::_File::read(mcsl::arr_span<ubyte> data) {
   if (!(_flags & FileFlags::READ)) {
      _err = Errno::BAD_FILE_STATE;
      return FILE_ERROR_VAL;
   }
   if (!(data.begin() && data.size())) {
      _err = Errno::FS_NULL_BUF;
      return FILE_ERROR_VAL;
   }

   //unbuffered IO
   if (!(_flags & FileFlags::BUFFERED)) {
      return _read(data);
   }

   //buffered IO
   if (Errno err = _ensureBuf(); +err) {
      _err = err;
      return FILE_ERROR_VAL;
   }

   ubyte* dest = data.begin();
   uint rem = data.size();
   uint count = 0;

   sint tries = 0;
   sint maxTries = FILE_TRIES_IMPL_CAP;
   uint cpylen;

   do {
      //update iteration counter
      ++tries;

      //check if there is data in the buffer
      if (!_left) { //buffer is empty
         //check if it is worth reading into the buffer for this read
         if (rem * FILE_LONG_RDRW_FACTOR >= _len) { //not worth reading into the buffer
            //read
            sint tmp = _read({dest, rem});
            //check success
            if (!tmp) {
               goto CONTINUE;
            }
            //update locals
            count += tmp;
            dest += tmp;
            rem -= tmp;

            //check if there are still bytes to read
            if (!rem) {
               break;
            }
         }
         else { //worth reading into the buffer
            //update base and index
            _base += _index;
            _index = 0;
            debug_assert(_left == 0);
            //read
            sint tmp = _read({_buf, _cap});
            //check success
            if (!tmp) {
               goto CONTINUE;
            }
            //update len and left
            count += tmp;
            _len = tmp;
            _left = tmp;
         }
      }

      //debug checks
      debug_assert(_left > 0);
      debug_assert(rem > 0);

      //calculate amount of data to copy from buffer
      cpylen = rem < _left ? rem : _left;
      debug_assert(cpylen < _left);
      //copy data
      memcpy(dest, _buf + _index, cpylen);
      //update buffer
      _index += cpylen;
      _left -= cpylen;
      //update locals
      count += cpylen;
      dest += cpylen;
      rem -= cpylen;

      //label for continuing while still checking the loop condition
      CONTINUE:
   } while (rem && tries <= maxTries && !eof());

   return count;
}
sint mcsl::_File::write(const mcsl::arr_span<ubyte> data) {
   if (!(_flags & FileFlags::WRITE)) {
      _err = Errno::BAD_FILE_STATE;
      return FILE_ERROR_VAL;
   }
   if (!(data.begin() && data.size())) {
      _err = Errno::FS_NULL_BUF;
      return FILE_ERROR_VAL;
   }

   //unbuffered IO
   if (!(_flags & FileFlags::BUFFERED)) {
      return _write(data);
   }

   //buffered IO
   if (Errno err = _ensureBuf(); +err) {
      _err = err;
      return FILE_ERROR_VAL;
   }

   const ubyte* src = data.begin();
   uint rem = data.size();
   uint count = 0;

   sint tries = 0;
   sint maxTries = FILE_TRIES_IMPL_CAP;
   uint cpylen;

   do {
      //update iteration counter
      ++tries;

      //check if there is data in the buffer
      if (!_left) { //buffer is empty
         //check if it is worth writing into the buffer for this read
         if (rem * FILE_LONG_RDRW_FACTOR >= _len) { //not worth writing into the buffer
            //read
            sint tmp = _write(arr_span<ubyte>::make(src, rem));
            //check success
            if (!tmp) {
               goto CONTINUE;
            }
            //update locals
            count += tmp;
            src += tmp;
            rem -= tmp;

            //check if there are still bytes to write
            if (!rem) {
               break;
            }
         }
         else { //worth writing into the buffer
            //update base and index
            _base += _index;
            _index = 0;
            debug_assert(_left == 0);
            //write
            sint tmp = _write({_buf, _cap});
            //check success
            if (!tmp) {
               goto CONTINUE;
            }
            //update len and left
            count += tmp;
            _len = tmp;
            _left = tmp;
         }
      }

      //debug checks
      debug_assert(_left > 0);
      debug_assert(rem > 0);

      //calculate amount of data to copy from buffer
      cpylen = rem < _left ? rem : _left;
      debug_assert(cpylen < _left);
      //copy data
      memcpy(_buf + _index, src, cpylen);
      //update buffer
      _index += cpylen;
      _left -= cpylen;
      //update locals
      count += cpylen;
      src += cpylen;
      rem -= cpylen;

      //label for continuing while still checking the loop condition
      CONTINUE:
   } while (rem && tries <= maxTries);

   return count;
}
#pragma endregion rdwr

#pragma region open
mcsl::_File::FileRes mcsl::_File::open(cstr path, FileFlags flags, mode_t createMode) {
   //sanity checks
   if (path) {
      return {.err = Errno::BAD_PATH, .file = nullptr};
   }

   //convert flags to OS flags
   sint osFlags = flagsToOS(flags);

   //allocate file
   _File* file;
   if (FileRes res = allocFile(); !res.err) {
      file = res.file;
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
      fd = ::open(path.begin(), osFlags, createMode);
      if (fd >= 0) { //check for success
         [[likely]];
         //return
         return file->_open(fd, flags, osFlags);
      }
      [[unlikely]];
      file->_err = (Errno)errno;
      maxTries = getTries_io(file->_err);
   } while (tries <= maxTries);

   //return (failure)
   return {.err = file->_err, .file = nullptr};
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
   _fd = fd;
   _flags = flags;
   _osFlags = osFlags;
   _err = Errno::NO_ERR;
   //return
   return {.err = Errno::NO_ERR, .file = this};
}
#pragma endregion open
mcsl::Errno mcsl::_File::close() {
   //check state
   if (!(_flags & FileFlags::IS_OPEN)) {
      _err = Errno::BAD_FILE_STATE;
      return Errno::BAD_FILE_STATE;
   }

   //flush
   if (!(~_flags & (FileFlags::WRITE | FileFlags::BUF_SAVE))) {
      if (Errno err = flush(); +err) { return err; }
   }
   //sync
   if (Errno err = sync(); +err) { return err; }

   //unmap buffer if applicable
   if (+(_flags & FileFlags::BUF_SAVE) && !(_flags & FileFlags::BUF_USERPROV)) {
      if (Errno err = _unmapbuf(); +err) { return err; }
   }

   sint res = ::close(_fd);
   // mark the file as closed, regardless of the results of the close syscall
   // the `close` syscall puts the file descriptor back in the pool of available file descriptors before checking for errors
   // any actionable errors will be caught when trying to flush the file
   // the above only running for write-capable files should not cause any issues
   // the errors that can be returned by a `close` call should only be relevant because they can indicate that there was data that didn't get written to disk, which is not a thing for non-write-capable files
   // for further details, see the man page for `close`
   (void)res;
   _flags |= FileFlags::IS_CLOSED;
   _flags &= ~FileFlags::IS_OPEN;

   //return
   return freeFile(this);
}
mcsl::Errno mcsl::_File::flush() {
   if (!(~_flags & (FileFlags::WRITE | FileFlags::BUF_SAVE))) {
      _err = Errno::BAD_FILE_STATE;
      return Errno::BAD_FILE_STATE;
   }

   ubyte* src = _buf;
   sint rem = _len;
   sint count = 0;

   sint tries = 0;
   sint maxTries = 0;

   Errno err = Errno::NO_ERR;

   //core write loop
   do {
      ++tries;

      //write
      sint res = ::write(_fd, src, rem);
      //handle results
      if (res < 0) { //error
         res = errno;
         _err = err = (Errno)res;
         maxTries = getTries_io(err);
      }
      else { //successful write
         rem -= res;
         src += res;
         count += res;

         maxTries = FILE_PARTIAL_RDRW_CAP;
      }
   } while (rem > 0 && tries <= maxTries);
   
   debug_assert(rem >= 0);
   if (rem > 0) { //partial flush
      std::memmove(_buf, src, rem);
   }

   //update variables
   _index = 0;
   _left  = rem;
   _len  -= count;
   _base += count;

   //return
   return err;
}
mcsl::Errno mcsl::_File::sync() {
   decltype(fsync)* const syncfunc = +(_flags & FileFlags::MIN_OS_CACHE) ? &fdatasync : &fsync;
   sint tries = 0;
   sint maxTries = 0;

   //core write loop
   do {
      ++tries;

      //write
      sint res = syncfunc(_fd);
      //handle results
      if (res >= 0) { //success
         return Errno::NO_ERR;
      }
      //error
      res = errno;
      _err = (Errno)res;
      maxTries = getTries_io(_err);
   } while (tries <= maxTries);
   return _err;
}

Errno mcsl::_File::_ensureBuf() {
   //debug check that the file is buffered
   debug_assert(_flags & FileFlags::BUF_SAVE);
   //return if the file already has a buffer
   if (_buf) {
      return Errno::NO_ERR;
   }
   //mark the file as having a non-user-provided buffer
   _flags &= ~FileFlags::BUF_USERPROV;
   //map a buffer
   void* ptr;
   const uint len = g.pageSize;
   uint tries = 0;
   uint maxTries;
   do {
      ++tries;
      //try to map
      ptr = mmap(nullptr, len, MAP_ANONYMOUS | MAP_PRIVATE, PROT_READ | PROT_WRITE, -1, 0);
      if (ptr != MAP_FAILED) { //check for success
         [[likely]];
         //handle success
         _setbuf({(ubyte*)ptr, len});
         return Errno::NO_ERR;
      }
      //handle failure
      [[unlikely]];
      _err = (Errno)errno;
      maxTries = getTries_mem(_err);
   } while (tries <= maxTries);

   //handle overall failure
   return _err;
}
void mcsl::_File::_setbuf(arr_span<ubyte> data) {
   debug_assert(data.begin() && data.begin() != MAP_FAILED);
   debug_assert(data.size());

   _buf = data.begin();
   _cap = data.size();
   _len = 0;
   _index = 0;
   _left = 0;
}
mcsl::Errno mcsl::_File::_unmapbuf() {
   debug_assert(+(_flags & FileFlags::BUF_SAVE) && !(_flags & FileFlags::BUF_USERPROV));
   sint res;
   uint tries = 0;
   uint maxTries;
   do {
      ++tries;
      //try to unmap
      res = munmap(_buf, _cap);
      if (!res) { //check for success
         [[likely]];
         //return
         return Errno::NO_ERR;
      }
      [[unlikely]];
      _err = (Errno)errno;
      maxTries = getTries_io(_err);
   } while (tries <= maxTries);
   //return
   return _err;
}

sint mcsl::flagsToOS(FileFlags flags) {
   sint osFlags = 0;

   // file type
   if (__builtin_popcount(+(flags & FileFlags::FILE_TYPES)) > 1) {
      TODO;
   }
   switch (flags & FileFlags::FILE_TYPES) {
      case FileFlags::REGFILE  : break;
      case FileFlags::TMPFILE  : 
         osFlags |= O_TMPFILE;
         if (!(flags & (FileFlags::READ | FileFlags::WRITE))) {
            TODO;
         }
         break;
      case FileFlags::DIRECTORY: osFlags |= O_DIRECTORY; break;
      case FileFlags::FIFO     : break;
      case FileFlags::SOCKET   : break;

      default: UNREACHABLE;
   }

   // access mode
   if (+(flags & FileFlags::PATH)) {
      osFlags |= O_PATH;
      if (+(flags & (FileFlags::READ | FileFlags::WRITE))) {
         TODO;
      }
   }
   else if (+(flags & FileFlags::WRITE)) {
      if (+(flags & FileFlags::READ)) {
         osFlags |= O_RDWR;
      } else {
         osFlags |= O_WRONLY;
      }
   } else if (+(flags & FileFlags::READ)) {
      osFlags |= O_RDONLY;
   }

   // file existance reqs
   if (+(flags & FileFlags::ERR_IF_EX)) {
      osFlags |= O_CREAT | O_EXCL;
      
      if (+(flags & FileFlags::ERR_IF_NE)) {
         TODO;
      }
   }
   else if (!(flags & FileFlags::ERR_IF_NE)) {
      osFlags |= O_CREAT;
   }

   // append
   if (+(flags & FileFlags::APPEND)) {
      osFlags |= O_APPEND;
   }

   // IO mode
   if (+(flags & FileFlags::ASYNC)) {
      osFlags |= O_ASYNC;
   }
   if (+(flags & FileFlags::NONBLOCK)) {
      osFlags |= O_NONBLOCK;
   }

   // synchronization mode
   if (+(flags & FileFlags::FSYNC)) {
      osFlags |= O_SYNC;
   }
   else if (+(flags & FileFlags::DSYNC)) {
      osFlags |= O_DSYNC;
   }

   // metadata updates
   if (+(flags & FileFlags::NO_NEW_METADATA)) {
      osFlags |= O_NOATIME;
   }

   // close on exec
   if (!(flags & FileFlags::NO_CLO_EXEC)) {
      osFlags |= O_CLOEXEC;
   }

   // cache effect
   if (+(flags & FileFlags::MIN_OS_CACHE)) {
      osFlags |= O_DIRECT;
   }

   // return
   return osFlags;
}
#pragma region filealloc
mcsl::_File::FileRes mcsl::_File::allocFile() {
   debug_assert(_File::g.isInit);

   //check that there are files available
   if (!g.availLen) {
      return FileRes{.err = Errno::NO__FILES, .file = nullptr};
   }

   //get fnum
   sint fnum = g.avail[0];
   assume(fnum >= 0 && (uint)fnum < g.fileBuf.size());
   //update fnum list
   g.avail++;
   g.availLen--;
   g.inUseLen++;

   //get file object
   _File* file = g.fileBuf + fnum;
   //update file object fields
   file->_magicNum = FILE_MAGIC_NUM;
   file->_fnum = fnum;

   file->_len = 0;
   file->_index = 0;
   file->_base = 0;
   file->_buf = nullptr;
#if defined(SAFE_MODE)
   file->_fd = -1;
   file->_err = Errno::NO_ERR;
         
   file->_flags = 0;
   file->_osFlags = 0;
#endif
   //return
   return {.err = Errno::NO_ERR, .file = file};
}
mcsl::Errno mcsl::_File::freeFile(_File* file) {
   debug_assert(file);
   debug_assert(file->_magicNum == FILE_MAGIC_NUM);

   //move file's entry in the inUse list to the back of the inUse section
   {
      sint* target = g.inUse + file->_fnum;
      sint* back = g.inUse + g.inUseLen - 1;
      _File* backFile = g.fileBuf + *back;

      sint tmp = *back;
      *back = *target;
      *target = tmp;

      file->_fnum = *target;
      backFile->_fnum = *back;
   }

   //update fnum list
   g.availLen++;
   g.avail--;
   g.inUseLen--;

   //unset magic number
   file->_magicNum = ~FILE_MAGIC_NUM;
   //return
   return Errno::NO_ERR;
}
#pragma endregion filealloc
#pragma region global
decltype(mcsl::_File::g) mcsl::_File::g{};
bool mcsl::_File::globalSetup() {
   sint err;

   //ensure that partial initialization can be detected
   g.isInit = false;

   //register cleanup function to run atexit
   err = std::atexit(&globalCleanup);
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
   return true;
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
