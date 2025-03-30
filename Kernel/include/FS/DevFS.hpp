#pragma once

#include "FS/VFS.hpp"
#include "Drivers/ATA.hpp"

struct DeviceEntry {
    char name[256];
    FileType type;
    void* device;
};

class DevFS : public FileSystem {
public:
    DevFS();

    FileDescriptor* open(const char* path, u32 flags) override;
    u32 read(FileDescriptor* fd, void* buffer, u32 size) override;
    u32 write(FileDescriptor* fd, const void* buffer, u32 size) override;
    int close(FileDescriptor* fd) override;

    bool registerDevice(const char* name, FileType type, void* device);
    bool unregisterDevice(const char* name);

    const char* getName() const override { return "DevFS"; }

private:
    Vector<DeviceEntry> devices;
    
    u32 nextFileId;

    DeviceEntry* findDevice(const char* path);
    FileDescriptor* createDeviceFD(DeviceEntry* device, const char* path, u32 flags);
};

