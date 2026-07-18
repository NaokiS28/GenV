/*
 * ps1-bare-metal - (C) 2023 spicyjpeg
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
 * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

#pragma once

#include <stddef.h>
#include <stdint.h>

namespace PS1::IO::PCDrive
{
    enum OpenMode : uint8_t
    {
        PCDRV_MODE_READ       = 0,
        PCDRV_MODE_WRITE      = 1,
        PCDRV_MODE_READ_WRITE = 2
    };

    enum SeekMode : uint8_t
    {
        PCDRV_SEEK_SET = 0,
        PCDRV_SEEK_CUR = 1,
        PCDRV_SEEK_END = 2
    };

    enum Attribute : uint8_t
    {
        PCDRV_ATTR_READ_ONLY = 1 << 0,
        PCDRV_ATTR_HIDDEN    = 1 << 1,
        PCDRV_ATTR_SYSTEM    = 1 << 2,
        PCDRV_ATTR_DIRECTORY = 1 << 4,
        PCDRV_ATTR_ARCHIVE   = 1 << 5
    };

    struct DirectoryEntry
    {
        uint32_t attributes, size;
        char name[32];
    };

    /* Standard PCDRV API */

    int Init(void);
    int Create(const char *path, uint32_t attributes);
    int Open(const char *path, OpenMode mode);
    int Close(int fd);
    int Read(int fd, void *data, size_t length);
    int Write(int fd, const void *data, size_t length);
    int Seek(int fd, int offset, SeekMode mode);

    /* Extended PCDRV API */

    int CreateDir(const char *path);
    int RemoveDir(const char *path);
    int Unlink(const char *path);
    int Chmod(const char *path, uint32_t attributes);
    int FindFirst(const char *path, DirectoryEntry *entry);
    int FindNext(int fd, DirectoryEntry *entry);
    int Rename(const char *path, const char *newPath);
} // namespace PS1::IO::PCDrive
