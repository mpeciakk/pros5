#include "FS/VFS.hpp"
#include "Lib/Log.hpp"
#include "Lib/String.hpp"

bool VFSManager::validatePath(const char* path) {
    if (path[0] != '/') {
        return false;
    }

    if (path[1] != '\0' && path[strlen(path) - 1] == '/') {
        return false;
    }

    for (u32 i = 1; path[i] != '\0'; i++) {
        if (path[i] == '/' && path[i - 1] == '/') {
            return false;
        }
    }

    return true;
}

bool VFSManager::isPathMounted(const char* path) {
    for (MountPoint& mountPoint : mountPoints) {
        if (strcmp(mountPoint.path, path) == 0) {
            return true;
        }
    }

    return false;
}

MountPoint* VFSManager::findMountPoint(const char* path) {
    MountPoint* bestMatch = nullptr;
    u32 bestMatchLength = 0;

    for (MountPoint& mountPoint : mountPoints) {
        u32 mountPathLen = strlen(mountPoint.path);

        if (strncmp(path, mountPoint.path, mountPathLen) == 0) {
            if (mountPathLen > bestMatchLength) {
                bestMatch = &mountPoint;
                bestMatchLength = mountPathLen;
            }
        }
    }

    return bestMatch;
}

bool VFSManager::mount(const char* path, FileSystem* fs) {
    if (!path || !fs || !validatePath(path) || isPathMounted(path)) {
        return false;
    }

    MountPoint mountPoint;

    strncpy(mountPoint.path, path, sizeof(mountPoint.path) - 1);
    mountPoint.path[sizeof(mountPoint.path) - 1] = '\0';
    mountPoint.fs = fs;
    mountPoint.isRoot = (mountPoints.size() == 0);

    mountPoints.push_back(mountPoint);

    return true;
}

bool VFSManager::unmount(const char* path) {
    for (auto i = mountPoints.begin(); i != mountPoints.end(); i++) {
        if (strcmp((*i).path, path) == 0) {
            if ((*i).isRoot) {
                return false;
            }

            mountPoints.erase(i);
            return true;
        }
    }

    return false;
}

FileDescriptor* VFSManager::open(const char* path, u32 flags) {
    if (!path) {
        return nullptr;
    }

    MountPoint* mountPoint = findMountPoint(path);
    if (!mountPoint) {
        return nullptr;
    }

    const char* relativePath = path + strlen(mountPoint->path);
    if (*relativePath == '/') {
        relativePath++;
    }

    FileDescriptor* fd = mountPoint->fs->open(relativePath, flags);
    if (fd) {
        fd->mountPoint = mountPoint;
    }
    return fd;
}

u32 VFSManager::read(FileDescriptor* fd, void* buffer, u32 size) {
    if (!fd) {
        return 0;
    }

    if (!fd->mountPoint) {
        return 0;
    }

    return fd->mountPoint->fs->read(fd, buffer, size);
}

u32 VFSManager::write(FileDescriptor* fd, const void* buffer, u32 size) {
    if (!fd) {
        return 0;
    }

    if (!fd->mountPoint) {
        return 0;
    }

    return fd->mountPoint->fs->write(fd, buffer, size);
}

int VFSManager::close(FileDescriptor* fd) {
    if (!fd) {
        return -1;
    }

    if (!fd->mountPoint) {
        return -1;
    }

    return fd->mountPoint->fs->close(fd);
}
