#pragma once

#include "Lib/Types.hpp"
#include "Lib/Log.hpp"
#include "Lib/Vector.hpp"

enum class FileType { Regular, Directory, CharacterDevice, BlockDevice, Unknown };

class FileSystem;

struct MountPoint {
    char path[256];
    FileSystem* fs;
    bool isRoot;
};

struct FileDescriptor {
    u32 id;
    char path[256];
    MountPoint* mountPoint;
};

class FileSystem {
public:
    virtual FileDescriptor* open(const char* path, u32 flags) = 0;
    virtual u32 read(FileDescriptor* fd, void* buffer, u32 size) = 0;
    virtual u32 write(FileDescriptor* fd, const void* buffer, u32 size) = 0;
    virtual int close(FileDescriptor* fd) = 0;
    
    virtual const char* getName() const = 0;
};

class VFSManager {
public:
    static VFSManager& instance() {
        static VFSManager vfs;
        return vfs;
    }

    bool mount(const char* path, FileSystem* fs);
    bool unmount(const char* path);

    FileDescriptor* open(const char* path, u32 flags);
    u32 read(FileDescriptor* fd, void* buffer, u32 size);
    u32 write(FileDescriptor* fd, const void* buffer, u32 size);
    int close(FileDescriptor* fd);

private:
    VFSManager() = default;

    Vector<MountPoint> mountPoints;

    MountPoint* findMountPoint(const char* path);
    bool validatePath(const char* path);
    bool isPathMounted(const char* path);
};
