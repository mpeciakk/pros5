#include "Lib/Log.hpp"
#include "Drivers/ATA.hpp"
#include "Lib/String.hpp"

ATA::ATA(u16 portBase, bool master)
    : dataPort(portBase), errorPort(portBase + 1), sectorCountPort(portBase + 2), lbaLowPort(portBase + 3), lbaMidPort(portBase + 4),
      lbaHighPort(portBase + 5), devicePort(portBase + 6), commandPort(portBase + 7), controlPort(portBase + 0x206) {

    this->master = master;
}

void ATA::identify() {
    devicePort.write(master ? 0xA0 : 0xB0); // identify if device is master or slave
    sectorCountPort.write(0);               // clear sector count
    lbaLowPort.write(0);                    // clear lba
    lbaMidPort.write(0);                    // clear lba
    lbaHighPort.write(0);                   // clear lba
    commandPort.write(0xEC);                // send identify command

    u8 status = commandPort.read(); // read device status

    if (status == 0x00) {
        // no device on that bus
        return;
    }

    // is device busy?
    while (((status & 0x80) == 0x80) && ((status & 0x01) != 0x01)) {
        status = commandPort.read();
    }

    if (status & 0x01) {
        Log::error("ATA Error");
    }

    for (u16 i = 0; i < 256; i++) {
        u16 data = dataPort.read();

        char* text = "  \0";
        text[0] = (data >> 8) & 0xFF;
        text[1] = data & 0xFF;

        // Log::kprints(text);
    }
}

u32 ATA::read(u32 sector, u8* buffer, u32 count) {
    if (!buffer || count == 0) {
        return 0;
    }

    devicePort.write(master ? 0xE0 : 0xF0);

    while (commandPort.read() & 0x80) {
    }

    sectorCountPort.write(count);           // Number of sectors to read
    lbaLowPort.write(sector & 0xFF);        // LBA low byte
    lbaMidPort.write((sector >> 8) & 0xFF); // LBA mid byte
    lbaHighPort.write((sector >> 16) & 0xFF); // LBA high byte
    devicePort.write(master ? 0xE0 : 0xF0); // Select device
    commandPort.write(0x20);                // Read command

    for (u32 i = 0; i < count; i++) {
        u8 status;
        do {
            status = commandPort.read();
        } while ((status & 0x80) && !(status & 0x08));

        if (status & 0x01) {
            u8 error = errorPort.read();
            Log::error("ATA Read Error: {}", error);
            return 0;
        }

        for (u32 j = 0; j < ATA_SECTOR_SIZE; j += 2) {
            u16 data = dataPort.read();
            buffer[i * ATA_SECTOR_SIZE + j] = data & 0xFF;
            buffer[i * ATA_SECTOR_SIZE + j + 1] = (data >> 8) & 0xFF;
        }
    }

    return count * ATA_SECTOR_SIZE;
}

