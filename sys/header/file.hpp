#pragma once
#ifndef MCSL_FILE_HPP
#define MCSL_FILE_HPP

#include "MCSL.hpp"

#include "path.hpp"
#include "arr_span.hpp"

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

namespace mcsl {
   constexpr mode_t DEFAULT_CREATE_MODE = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH;
   enum class FileFlags : uint32 {
      READ             =  0_m, // WRITE ? O_RDWR : O_RDONLY
      WRITE            =  1_m, // READ  ? O_RDWR : O_WRONLY
      PATH             =  2_m, // O_PATH

      IS_OPEN          =  3_m,
      IS_CLOSED        =  4_m,

      SAW_EOF          =  5_m, // feof

      REGFILE          =  6_m,
      TMPFILE          =  7_m, // O_TMPFILE
      DIRECTORY        =  8_m, // O_DIRECTORY
      FIFO             =  9_m,
      SOCKET           = 10_m,

      ERR_IF_EX        = 11_m, // O_CREAT | O_EXCL
      ERR_IF_NE        = 12_m, // ~O_CREAT
      APPEND           = 13_m, // O_APPEND
      TRUNC            = 14_m, // O_TRUNC

      ASYNC            = 15_m, // O_ASYNC
      NONBLOCK         = 16_m, // O_NONBLOCK

      NO_CLO_EXEC      = 17_m, // ~O_CLOEXEC

      FSYNC            = 18_m, // O_SYNC
      DSYNC            = 19_m, // O_DSYNC
      MIN_OS_CACHE     = 20_m, // O_DIRECT

      NO_NEW_METADATA  = 21_m, // O_NOATIME
      
      // read/write buffer
      BUF_SAVE         = 22_m,
      BUF_USERPROV     = 23_m,
      BUF_ALL          = BUF_SAVE | BUF_USERPROV,

      // aliases
      BUFFERED     = BUF_SAVE,
      READONLY     = READ,
      WRITEONLY    = WRITE,
      READ_WRITE   = READ | WRITE,

      SHOULD_FLUSH = REGFILE | DIRECTORY,

      ACCESS_MODES = READ | WRITE | PATH,
      FILE_TYPES   = REGFILE | TMPFILE | DIRECTORY | FIFO | SOCKET,

      FOR_OPEN     = ACCESS_MODES | FILE_TYPES |
                     ERR_IF_NE |
                     APPEND | TRUNC | ASYNC | NONBLOCK |
                     NO_CLO_EXEC |
                     FSYNC | DSYNC | MIN_OS_CACHE |
                     NO_NEW_METADATA |
                     BUF_ALL,
      FOR_SETBUF   = BUF_ALL,
   };
   constexpr uint32 operator+(FileFlags f) { return std::to_underlying(f); }
   constexpr bool operator!(FileFlags f) { return +f; }
   constexpr FileFlags operator~(FileFlags f) { return (FileFlags)~+f; }
   constexpr FileFlags operator&(FileFlags lhs, FileFlags rhs) { return (FileFlags)(+lhs & +rhs); }
   constexpr FileFlags operator|(FileFlags lhs, FileFlags rhs) { return (FileFlags)(+lhs | +rhs); }
   constexpr FileFlags operator^(FileFlags lhs, FileFlags rhs) { return (FileFlags)(+lhs ^ +rhs); }
   constexpr FileFlags& operator&=(FileFlags& lhs, FileFlags rhs) { return lhs = lhs & rhs; }
   constexpr FileFlags& operator|=(FileFlags& lhs, FileFlags rhs) { return lhs = lhs | rhs; }
   constexpr FileFlags& operator^=(FileFlags& lhs, FileFlags rhs) { return lhs = lhs ^ rhs; }
   sint flagsToOS(FileFlags);

   enum class _SeekMode {
      SET = SEEK_SET,
      CURR = SEEK_CUR,
      END = SEEK_END,
   };
};


//file implementation class
//not intended to be used by users - exists to allow a cleaner API in `mcsl::File`
//if you prefer its API to `mcsl::File`, use it instead
//but be warned, there is very little error checking, as most of that happens in `mcsl::File`
struct mcsl::_File {
   private:
      friend class File;

      uint _magicNum;

      sint _fd;
      sint _fnum;
      Errno _err;

      FileFlags _flags;
      sint _osFlags;

      //buffered IO
      //in `_File` instead of `File` for proper flush-on-exit behavior
      uint _cap;   //capacity of the buffer
      uint _len;   //number of initialized bytes in the buffer
      uint _index; //offset of current position from base (tell() == base + index)
      uint _left;  //number of bytes left in the buffer (index + left == len)
      slong _base; //number of bytes into the file that the beginning of the buffer corresponds to
      ubyte* _buf;

   public:
      struct FileRes {
         Errno err;
         _File* file;
      };
   private:
      static struct {
         bool isInit;
         uint pageSize;
         arr_span<ubyte> data;

         mcsl::arr_span<_File> fileBuf;

         sint*   inUse;
         sint inUseLen;
         sint*   avail;
         sint availLen;
      } g;
      static bool __dummy;

      static bool globalSetup();
      static void globalCleanup();
      
      static FileRes allocFile();
      static Errno freeFile(_File*);
   public:
      using SeekMode = _SeekMode;
      using enum SeekMode;
   private:
      FileRes _open(sint fd, FileFlags flags, sint osFlags);
      sint _read(arr_span<ubyte> data);
      sint _write(const arr_span<ubyte> data);
      Errno _ensureBuf();
      void _setbuf(arr_span<ubyte> data);
      Errno _unmapbuf();
   public:
      static FileRes open(cstr path, FileFlags flags, mode_t createMode = DEFAULT_CREATE_MODE);
      static FileRes open(sint fd, FileFlags flags, sint osFlags);
      inline static FileRes open(sint fd, FileFlags flags) { return open(fd, flags, flagsToOS(flags)); }
      Errno close();
      Errno flush();
      Errno sync();

      Errno setbuf(arr_span<ubyte>);

      sint read(arr_span<ubyte>);
      sint readv(arr_span<arr_span<ubyte>>);
      sint write(const arr_span<ubyte>);
      sint writev(const arr_span<arr_span<ubyte>>);

      slong seek(slong offset, SeekMode mode = SET);
      slong tell();
      inline slong setpos(slong offset, SeekMode mode = SET) { return seek(offset, mode); }
      inline slong getpos() { return tell(); }

      bool eof();

      sint getFD();

      struct stat stat();
};

class mcsl::File {
   private:
      _File* _file;
      FileFlags _flags;
      Errno _err;

      cstr _path;
   public:
      using SeekMode = _SeekMode;
      using enum SeekMode;

      File();
      File(cstr path, FileFlags flags);

      Errno close();
      Errno flush();
      Errno sync();

      Errno setbuf(arr_span<ubyte>);
      Errno setbuf(str_slice);

      sint read(arr_span<ubyte>);
      sint readv(arr_span<arr_span<ubyte>>);
      sint write(const arr_span<ubyte>);
      sint writev(const arr_span<arr_span<ubyte>>);

      sint read(str_slice);
      sint readv(arr_span<str_slice>);
      sint write(const str_slice);
      sint writev(const arr_span<str_slice>);

      sint readln(str_slice);
      sint writeln(const str_slice);

      slong seek(slong offset, SeekMode mode = SET);
      slong tell();
      inline slong setpos(slong offset, SeekMode mode = SET) { return seek(offset, mode); }
      inline slong getpos() { return tell(); }

      int fd();

      struct stat stat();
      ulong size();
};

#endif //MCSL_FILE_HPP
