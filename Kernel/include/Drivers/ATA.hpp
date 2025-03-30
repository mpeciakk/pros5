#pragma once

#include "Lib/Types.hpp"
#include "Hardware/Port.hpp"

#define ATA_SECTOR_SIZE 512

class ATA {
public:
    ATA(u16 portBase, bool master);

    void identify();

    u32 read(u32 sector, u8* buffer, u32 count);

private:
    u8* readSector(u32 lba);

private:
    Port16Bit dataPort;
    Port8Bit errorPort;
    Port8Bit sectorCountPort;
    Port8Bit lbaLowPort;
    Port8Bit lbaMidPort;
    Port8Bit lbaHighPort;
    Port8Bit devicePort;
    Port8Bit commandPort;
    Port8Bit controlPort;

    bool master;
};
