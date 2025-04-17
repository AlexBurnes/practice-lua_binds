#pragma once
#ifndef FILE_HEADER
#define FILE_HEADER

#include <typedef.h>

#ifdef OS_WIN_32

#include <windows.h>

#define FD HFILE
#define INVALID_FD HFILE_ERROR
#define O_RDONLY OF_READ
#define O_WRONLY OF_WRITE
#define O_CREAT OF_CREATE
//#define O_TRUNC OF_TRUNCATE
#undef SEEK_END
#undef SEEK_SET
#undef SEEK_CUR
#define SEEK_END FILE_END
#define SEEK_SET FILE_BEGIN
#define SEEK_CUR FILE_CURRENT

#define _fopen                       _lopen
#define _fclose                      _lclose
#define _fseek                       _llseek
#define _fread( file, buf, size )    _lread( file, buf, size)
#define _fwrite( file, buf, size )   _lwrite( file, buf, size)

#else
#define OS_LINUX
#endif

#ifdef OS_LINUX

#include <fcntl.h>
#include <stdio.h>
#include <sys/file.h>

#define FD int
#define INVALID_FD -1

#define _fopen                          open
#define _fclose                         close
#define _fread( file, buf, size )       read( file, buf, size)
#define _fwrite( file, buf, size )      write( file, buf, size)
#define _fseek( file, offset, whence )  lseek( file, offset, whence)

#endif

#define FILE_OPENED 1

class File : public Iam {
    FD              file_fd;
    size_t          file_size;   //whole size of file
    size_t          file_offset; //readed offset of file
    char            *file_name;
    unsigned short  file_mode;
    bool            locked;

    public:

        File();
        ~File();

        int Open(FD fd);
        int Open(const char *name, unsigned short mode = O_RDONLY);
        size_t Size() {return file_size;};
        const char *Name() {return (const char *)file_name;};
        size_t Read(char *buf, size_t size);
        size_t Write(char *buf, size_t size);
        size_t Seek(unsigned short position, size_t offset);
        bool Eof();

        FD operator()() {return file_fd;};
        int Close();

        int Lock(int mode, int timeout);
        int Unlock();

    private:
        int _Open();

};

#endif
