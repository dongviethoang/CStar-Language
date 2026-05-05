#ifndef _LIBPOSIX_CSTAR_HEADER
#define _LIBPOSIX_CSTAR_HEADER

#include <string>
#include <iostream>
#include <fstream>
#include <vector>

#ifndef __types_posix
#define __types_posix
// POSIX string
typedef const char* POSIXSTR;
// POSIX wide string
typedef const wchar_t* POSIXWSTR;
#endif

#ifndef __errors_posix
#define __errors_posix
#define EPERM 0x01
#define ENOENT 0x02
#define ESRCH 0x03
#define EINTR 0x04
#define EIO 0x05
#define ENXIO 0x06
#define E2BIG 0x07
#define ENOEXEC 0x08
#define EBADF 0x09
#define ECHILD 0x0A
#define EAGAIN 0x0B
#define ENOMEM 0x0C
#define EACCES 0x0D
#define EFAULT 0x0E
#define ENOTBLK 0x0F
#define EBUSY 0x10
#define EEXIST 0x11
#define EXDEV 0x12
#define ENODEV 0x13
#endif

#ifndef _LIB_POSIX_FILEIO
#define _LIB_POSIX_FILEIO
extern int posix_errno;
extern std::vector<std::fstream*> fd_table;

int getcurrenterrno() {
    return posix_errno;
}

ssize_t fileread(std::string file, int fd, void *buf, size_t mbytes) {
    // bad FD → EBADF equivalent, but use ENXIO (no such device/address)
    if (fd < 0 || fd >= (int)fd_table.size() || fd_table[fd] == nullptr) {
        posix_errno = ENXIO;
        return -1;
    }

    std::fstream* stream = fd_table[fd];

    // stream not open → ENOENT or EACCES depending on design
    if (!stream->is_open()) {
        posix_errno = ENOENT;   // file not found / not available
        return -1;
    }

    if (buf == nullptr) {
        posix_errno = EIO;      // invalid I/O buffer
        return -1;
    }

    stream->clear();
    stream->seekg(stream->tellg());

    char* cbuf = (char*)buf;
    size_t count = 0;

    while (count < mbytes && stream->good()) {
        char ch;
        stream->get(ch);

        if (!stream->good())
            break;

        cbuf[count++] = ch;
    }

    // normal read - no error
    posix_errno = 0;
    return (ssize_t)count;
}

ssize_t filewrite(int fd, const void *buf, size_t mbytes) {
    // invalid descriptor
    if (fd < 0 || fd >= (int)fd_table.size() || fd_table[fd] == nullptr) {
        posix_errno = ENXIO;
        return -1;
    }

    std::fstream* stream = fd_table[fd];

    if (!stream->is_open()) {
        posix_errno = ENOENT;   // file not available
        return -1;
    }

    if (buf == nullptr) {
        posix_errno = EIO;
        return -1;
    }

    stream->seekp(0, std::ios::beg);

    const char* cbuf = static_cast<const char*>(buf);
    size_t written = 0;

    while (written < mbytes && stream->good()) {
        stream->put(cbuf[written]);

        if (!stream->good())
            break;

        written++;
    }

    // Flush every write (matching POSIX write() immediate commit)
    stream->flush();

    posix_errno = 0;
    return (ssize_t)written;
}
#endif
#endif