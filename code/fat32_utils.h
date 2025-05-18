// file: fat32_utils.h
#ifndef FAT32_UTILS_H
#define FAT32_UTILS_H

#include <stdio.h>
#include <stdint.h>
#include <wchar.h>

#pragma pack(push, 1)
typedef struct {
    uint8_t  jumpBoot[3];
    char     OEMName[8];
    uint16_t bytesPerSector;
    uint8_t  sectorsPerCluster;
    uint16_t reservedSectorCount;
    uint8_t  numFATs;
    uint16_t rootEntryCount;
    uint16_t totalSectors16;
    uint8_t  mediaType;
    uint16_t FATSize16;
    uint16_t sectorsPerTrack;
    uint16_t numHeads;
    uint32_t hiddenSectors;
    uint32_t totalSectors32;
    uint32_t FATSize32;
    uint16_t extFlags;
    uint16_t FSVersion;
    uint32_t rootCluster;
    uint16_t FSInfo;
    uint16_t backupBootSector;
    uint8_t  reserved[12];
    uint8_t  driveNumber;
    uint8_t  reserved1;
    uint8_t  bootSignature;
    uint32_t volumeID;
    char     volumeLabel[11];
    char     fileSystemType[8];
} FAT32BootSector;

typedef struct {
    uint8_t name[11];
    uint8_t attr;
    uint8_t ntReserved;
    uint8_t createTimeTenth;
    uint16_t createTime;
    uint16_t createDate;
    uint16_t lastAccessDate;
    uint16_t firstClusterHigh;
    uint16_t writeTime;
    uint16_t writeDate;
    uint16_t firstClusterLow;
    uint32_t fileSize;
} DirectoryEntry;

typedef struct {
    uint8_t order;
    uint16_t name1[5];
    uint8_t attr;
    uint8_t type;
    uint8_t checksum;
    uint16_t name2[6];
    uint16_t firstClusterLow;
    uint16_t name3[2];
} LFNEntry;
#pragma pack(pop)

#define MAX_LFN_ENTRIES 20

void readBootSector(FILE *disk, FAT32BootSector *bootSector);
void readFAT(FILE *disk, FAT32BootSector *bootSector, uint8_t *FAT);
void readCluster(FILE *disk, FAT32BootSector *bootSector, uint32_t clusterNum, uint8_t *buffer);

void copyLFNChars(const uint16_t *src, int count, wchar_t *dest, int *index);
void wcharToUtf8(const wchar_t *wstr, char *out, int maxLen);
void getShortNameDeleted(const DirectoryEntry *entry, char *out);

void recoverDeletedFile(FILE *disk, FAT32BootSector *bootSector, uint8_t *FAT,
                        uint32_t startCluster, uint32_t fileSize, const char *outputFilename);
void findDeletedFilesWithLFN(FILE *disk, FAT32BootSector *bootSector, uint8_t *FAT, const char *targetFileName);

#endif // FAT32_UTILS_H
