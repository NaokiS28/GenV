/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * storage.hpp - Created on 27-05-2025
 *
 * GenV is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * GenV is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * GenV. If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include <stdint.h>
#include "common/util/misc.hpp"

/*
 * Mass Storage Device Service
 * ===========================
 * It's expected that each type of mass storage device (or interface) will have
 * it's own interface, with an a selection method between the interfaces provided
 * by a seperate class.
 *
 * In practice, this would mean the the following file paths:
 *  hdd0://     would be directed to the first mass storage device on the default partition.
 *  hdd0:0://   would use the literal first partition of the hard drive.
 *  odd0://     would be the first optical disc drive.
 *  odd0:1://   would be the first track on the first optical disc drive. (For audio CD usage mainly)
 *  rdd0://     would be the first removable storage volume (USB flash drive, SD card)
 *  rdd0:0://   would be the first partition on the first removable storage volume
 *  fdd0://     would be the first floppy disk drive
 *  pcc0://     would be the first PCMCIA flash/ATA storage card
 *  pcc1://     would be the second PCMCIA flash/ATA storage card
 *  fsd0://     would be the first Flash storage device of any intergrated flash storage
 *  rom0://     would be the first ROM chip in a ROM bank
 *  ram0://     would be the first writable RAM disk device (please read notes on this)
 *  nvr://      would be the writable RTC/NVRAM RAM storage area
 *  rtc://      is an alias for nvr://
 *
 * Query the storage manager for available storage devices on the running system
 */

namespace Files
{
    constexpr uint16_t kibiByte = 1024;
    constexpr uint16_t kiloByte = 1000;

    constexpr size_t KiB(size_t kb) { return (kibiByte * kb); }      // returns number of bytes in given KB (USE KB for 1000 bytes)
    constexpr size_t MiB(size_t mb) { return (KiB(kibiByte) * mb); } // returns number of bytes in given MB (USE MB for 1000 bytes)
    constexpr size_t GiB(size_t gb) { return (MiB(kibiByte) * gb); } // returns number of bytes in given GB (USE GB for 1000 bytes)

    constexpr size_t KB(size_t kb) { return (kiloByte * kb); }      // returns number of bytes in given KB (USE KiB for 1024 bytes)
    constexpr size_t MB(size_t mb) { return (KiB(kiloByte) * mb); } // returns number of bytes in given MB (USE MiB for 1024 bytes)
    constexpr size_t GB(size_t gb) { return (MiB(kiloByte) * gb); } // returns number of bytes in given GB (USE GiB for 1024 bytes)

    enum class StorageType : uint8_t
    {
        HDD,
        ODD,
        RDD,
        FDD,
        PCC,
        FSD,
        ROM,
        RAM,
        NVR,
        RTC = NVR,
        NUL
    };

    struct StorageDevice
    {
        StorageType type; // Type of media
        size_t size;      // Max size of drive in bytes
        StorageDevice(StorageType type, size_t size) : type(type), size(size) {}
    };

    enum class OpticalType
    {
        CD,
        VCD,
        DVD,
        HDDVD,
        BLURAY
    };

    struct OpticalFormat
    {
        OpticalType type;
        size_t maxSize;
    };

    namespace Optical
    {
        constexpr OpticalFormat CD = {OpticalType::CD, MiB(700)};
        constexpr OpticalFormat VCD = {OpticalType::VCD, MiB(700)};
        constexpr OpticalFormat DVD = {OpticalType::DVD, MiB(4812)};
        constexpr OpticalFormat HDDVD = {OpticalType::HDDVD, GiB(15)};
        constexpr OpticalFormat HDDVD_DL = {OpticalType::HDDVD, GiB(30)};
        constexpr OpticalFormat BLURAY = {OpticalType::BLURAY, GiB(25)};
        constexpr OpticalFormat BLURAY_DL = {OpticalType::BLURAY, GiB(50)};
        constexpr OpticalFormat BLURAY_XL = {OpticalType::BLURAY, GiB(100)};
    }

    enum class HDDBus
    {
        IDE,
        SCSI,
        SATA,
        USB,
        FW
    };

    namespace IDE
    {
        constexpr uint8_t PRIMARY = 0;
        constexpr uint8_t SECONDARY = 1;
    }

    struct HDDDevice
    {
        HDDBus bus;
        bool solidState = false;
        size_t size = 0;
        uint8_t busPosition = 0;
        // HDDPartition  partitions;
    };

    struct OpticalMedia
    {
        StorageDevice odd;
        uint8_t sessions; // DATA sessions
        uint8_t tracks;   // AUDIO tracks
        OpticalMedia(size_t size,
                     uint8_t sessions = 0,
                     uint8_t tracks = 0) : odd(StorageType::ODD, size),
                                           sessions(sessions),
                                           tracks(tracks) {}
    };

    constexpr const char *hddStr = "hdd";
    constexpr const char *oddStr = "odd";
    constexpr const char *rddStr = "rdd";
    constexpr const char *fddStr = "fdd";
    constexpr const char *pccStr = "pcc";
    constexpr const char *fsdStr = "fsd";
    constexpr const char *romStr = "rom";
    constexpr const char *ramStr = "ram";
    constexpr const char *nvrStr = "nvr";

    constexpr const char *providerTypes[] = {
        hddStr,
        oddStr,
        rddStr,
        fddStr,
        pccStr,
        fsdStr,
        romStr,
        ramStr,
        nvrStr};

    struct ProviderType
    {
        const char *typeName;
        StorageType type;
        ProviderType(const char *name,
                     StorageType type) : typeName(name),
                                         type(type)
        {
        }
    };

    // Determines in the given path is a valid FQFP (fully-qualified file path)
    bool isVaildFQFP(const char *path, int len);

    // Gets the provider type from a given path.
    // Returns StorageType::NUL if function fails.
    StorageType getProviderType(const char *path, int len);

    // Gets a storage provider index from path
    // Returns a default 0 if function fails to process the path.
    uint8_t getProviderIndex(const char *path, int len);
    
    // Get the subdevice index number if it exists in path.
    // Returns a default 0 if function fails to process the path.
    uint8_t getProviderSubIndex(const char *path, int len);

    struct IStorageDevice
    {

    };

    class IStorage
    {
    public:
        IStorage() = default;
        ~IStorage() {}

        virtual bool init() = 0;
        virtual bool reset() = 0;
        virtual void shutdown() = 0;

        virtual int openFile(const char *path) { return 0; }
        virtual int closeFile() { return 0; }
        virtual int writeFile() { return 0; }
        virtual int newFile(const char *path, const char *filename) { return 0; }
        virtual int deleteFile() { return 0; }
        virtual int readFile(size_t offset, size_t length) { return 0; }

        // Gets a list of drives present in the system and returns the total count.
        virtual uint8_t getDriveList(IStorageDevice *list) = 0;

    private:
    };
}