#include <log/log.hpp>
#include "file/file.hpp"
#include <time.h>
#include <unistd.h>

File::File() : Iam("{}", (void *)this) {
    file_offset = 0;
    file_size   = 0;
    file_fd     = 0;
    file_mode   = 0;
    file_name   = nullptr;
    locked      = false;
}

File::~File() {
    if (locked) Unlock();
    Close();
}

int File::Open(FD fd) {
    file_fd = fd;
    file_offset = 0;
    file_size   = 0;
    return 1;
}

int File::Open(const char *name, unsigned short mode) {
    if (name == nullptr or strlen(name) == 0) {
        le("empty file name");
        return ::error;
    }

    file_name = new char[strlen(name)+1];
    affirm(file_name == nullptr and "failed allocate memory for file_name size {}", strlen(name)+1);

    strcpy(file_name, name);

    file_offset = 0;
    file_size   = 0;
    file_fd     = 0;
    file_mode   = mode;

    unless (_Open()) {
        le("failed open file {}", name);
        return 0;
    }
    iam("{}[{}]", file_name, file_fd);
    ld(1, "file opened '{}' as [{}]", file_name, file_fd);
    return 1;
}

#ifdef OS_WIN_32
int File::_Open () {
    DWORD dwDesiredAccess;
    DWORD dwCreationDistribution;
    HANDLE fileHandle;

    if (mode|O_RDONLY) {
        dwDesiredAccess = GENERIC_READ;
        }
    if (mode|O_WRONLY) {
        dwDesiredAccess = GENERIC_WRITE;
        }
    dwCreationDistribution = OPEN_EXISTING;
    if (mode|O_CREAT) {
        dwCreationDistribution = OPEN_ALWAYS;
        }
    fileHandle = CreateFile(name, dwDesiredAccess, 0, NULL, dwCreationDistribution, FILE_ATTRIBUTE_NORMAL, NULL);
    CloseHandle(fileHandle);


    if ((_fd=_fopen(name, mode))==INVALID_FD) {
        return 0;
    }
    return 1;
}
#endif

#ifdef OS_LINUX
int File::_Open() {
    if ((file_fd = _fopen(file_name, file_mode, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH))==INVALID_FD) {
        le("failed open file '{}'", file_name);
        return 0;
    }
    auto offset = _fseek(file_fd, 0, SEEK_CUR);
    file_size   = _fseek(file_fd, 0, SEEK_END);
    _fseek(file_fd, 0, offset);

    return 1;
}
#endif

int File::Close() {
    if (file_fd) {
        ld(1, "file {} [{}] close", file_name, file_fd);
        _fclose(file_fd);
        file_fd = 0;
        unless (file_name == nullptr) {
            delete[] file_name;
            file_name = nullptr;
        }
    }
    return 1;
}

size_t File::Read(char *buf, size_t size) {
    affirm(buf == nullptr and "null buffer reference");
    size_t readed = _fread(file_fd, buf, size);
    file_offset += readed;
    return readed;
}

size_t File::Write(char *buf, size_t size) {
    affirm(buf == nullptr and "null buffer reference");
    size_t writen = _fwrite(file_fd, buf, size);
    if (writen != size) {
        le("failed write {} bytes into {}, writen {}, error {}: {} ", size, file_name, writen, errno, strerror(errno));
        return ::error;
    }
    file_offset += writen;
    file_size += writen;
    return writen;
}

size_t File::Seek(unsigned short position, size_t offset) {
    _fseek(file_fd, offset, position);
    return file_offset = _fseek(file_fd, 0, SEEK_CUR);
}

bool File::Eof() {
    return file_offset == file_size ? true : false;
}


int File::Lock(int mode, int timeout) {
    unless (file_fd) return error;
    auto wait_time = time(nullptr) + timeout;
    do {
        switch (flock(file_fd, mode | LOCK_NB)) {
            case 0:
            case EWOULDBLOCK:
                locked = true;
                return ok;
            case EINTR:
                return error;
        }
        usleep(100000);
    }
    while(time(nullptr) < wait_time);
    return error;
}

int File::Unlock() {
    unless (file_fd) return error;
    unless (locked) return ok;
    if (flock(file_fd, LOCK_UN)) {
        le("failed unlock file '{}'", file_name);
        return error;
    }
    locked = false;
    return ok;
}

