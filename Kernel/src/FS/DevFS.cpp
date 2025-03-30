#include "FS/DevFS.hpp"
#include "Drivers/ATA.hpp"
#include "Lib/Log.hpp"
#include "Lib/String.hpp"

DevFS::DevFS() : nextFileId(1) { }

FileDescriptor* DevFS::open(const char* path, u32 flags) {
    if (path[0] == '/') {
        path++;
    }

    DeviceEntry* device = findDevice(path);
    if (!device) {
        return nullptr;
    }

    return createDeviceFD(device, path, flags);
}

u32 DevFS::read(FileDescriptor* fd, void* buffer, u32 size) {
    if (!fd) {
        return 0;
    }

    DeviceEntry* device = findDevice(fd->path);
    if (!device) {
        return 0;
    }

    switch (device->type) {
        case FileType::BlockDevice: {
            ATA* disk = (ATA*) device->device;
            return disk->read(0, (u8*) buffer, size / 512);
        }
        case FileType::CharacterDevice:
            return 0;
        default:
            return 0;
    }
    return 0;
}

u32 DevFS::write(FileDescriptor* fd, const void* buffer, u32 size) {

}

int DevFS::close(FileDescriptor* fd) {
    if (!fd) {
        return -1;
    }

    delete fd;

    return 0;
}

bool DevFS::registerDevice(const char* name, FileType type, void* device) {
    if (findDevice(name)) {
        return false;
    }

    DeviceEntry entry;
    strncpy(entry.name, name, sizeof(entry.name) - 1);
    entry.name[sizeof(entry.name) - 1] = '\0';

    entry.type = type;
    entry.device = device;

    devices.push_back(entry);
    return true;
}

bool DevFS::unregisterDevice(const char* name) {
    for (auto i = devices.begin(); i != devices.end(); i++) {
        if (strcmp((*i).name, name) == 0) {
            devices.erase(i);
            return true;
        }
    }

    return false;
}

DeviceEntry* DevFS::findDevice(const char* path) {
    for (DeviceEntry& entry : devices) {
        if (strcmp(entry.name, path) == 0) {
            return &entry;
        }
    }

    return nullptr;
}

FileDescriptor* DevFS::createDeviceFD(DeviceEntry* device, const char* path, u32 flags) {
    FileDescriptor* fd = new FileDescriptor();

    fd->id = nextFileId++;
    strncpy(fd->path, path, sizeof(fd->path) - 1);
    fd->path[sizeof(fd->path) - 1] = '\0';

    return fd;
}
