// file: fat32_utils.c
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <wchar.h>
#include "fat32_utils.h"

// Define structures for FAT32 Boot Sector
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
#pragma pack(pop)

// Function implementations
void readBootSector(FILE *disk, FAT32BootSector *bootSector) {
    fseek(disk, 0, SEEK_SET);
    fread(bootSector, sizeof(FAT32BootSector), 1, disk);
}

void readFAT(FILE *disk, FAT32BootSector *bootSector, uint8_t *FAT) {
    uint32_t FATStart = bootSector->reservedSectorCount * bootSector->bytesPerSector;
    uint32_t FATSize = bootSector->FATSize32 * bootSector->bytesPerSector;
    fseek(disk, FATStart, SEEK_SET);
    fread(FAT, FATSize, 1, disk);
}

void readCluster(FILE *disk, FAT32BootSector *bootSector, uint32_t clusterNum, uint8_t *buffer) {
    uint32_t firstDataSector = bootSector->reservedSectorCount +
                               (bootSector->numFATs * bootSector->FATSize32);
    uint32_t firstSectorOfCluster = ((clusterNum - 2) * bootSector->sectorsPerCluster) + firstDataSector;
    uint32_t byteOffset = firstSectorOfCluster * bootSector->bytesPerSector;
    fseek(disk, byteOffset, SEEK_SET);
    fread(buffer, bootSector->sectorsPerCluster * bootSector->bytesPerSector, 1, disk);
}

#pragma pack(push, 1)
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

void copyLFNChars(const uint16_t *src, int count, wchar_t *dest, int *index) {
    for (int i = 0; i < count; i++) {
        if (src[i] == 0x0000 || src[i] == 0xFFFF) break;
        dest[(*index)++] = src[i];
    }
}

void wcharToUtf8(const wchar_t *wstr, char *out, int maxLen) {
    int i = 0, j = 0;
    while (wstr[i] != 0 && j < maxLen - 1) {
        if (wstr[i] < 0x80) {
            out[j++] = (char)wstr[i];
        } else if (wstr[i] < 0x800) {
            out[j++] = (char)(0xC0 | (wstr[i] >> 6));
            out[j++] = (char)(0x80 | (wstr[i] & 0x3F));
        } else {
            out[j++] = (char)(0xE0 | (wstr[i] >> 12));
            out[j++] = (char)(0x80 | ((wstr[i] >> 6) & 0x3F));
            out[j++] = (char)(0x80 | (wstr[i] & 0x3F));
        }
        i++;
    }
    out[j] = '\0';
}

void getShortNameDeleted(const DirectoryEntry *entry, char *out) {
    int i, j = 0;
    out[j++] = '?';
    for (i = 1; i < 8 && entry->name[i] != ' '; i++) {
        out[j++] = entry->name[i];
    }
    if (entry->name[8] != ' ') {
        out[j++] = '.';
        for (i = 8; i < 11 && entry->name[i] != ' '; i++) {
            out[j++] = entry->name[i];
        }
    }
    out[j] = '\0';
}

void recoverDeletedFile(FILE *disk, FAT32BootSector *bootSector, uint8_t *FAT,
                        uint32_t startCluster, uint32_t fileSize, const char *outputFilename) {
    uint32_t bytesPerCluster = bootSector->sectorsPerCluster * bootSector->bytesPerSector;
    uint32_t firstDataSector = bootSector->reservedSectorCount +
                                (bootSector->numFATs * bootSector->FATSize32);
    FILE *out = fopen(outputFilename, "wb");
    if (!out) { perror("Failed to create output file"); return; }
    uint32_t currentCluster = startCluster;
    while (currentCluster >= 0x00000002 && currentCluster < 0x0FFFFFF8) {
        uint32_t firstSectorOfCluster = ((currentCluster - 2) * bootSector->sectorsPerCluster) + firstDataSector;
        uint32_t byteOffset = firstSectorOfCluster * bootSector->bytesPerSector;
        fseek(disk, byteOffset, SEEK_SET);
        uint8_t *buffer = malloc(bytesPerCluster);
        if (!buffer) { perror("Buffer allocation failed"); break; }
        fread(buffer, bytesPerCluster, 1, disk);
        uint32_t toWrite = (fileSize < bytesPerCluster) ? fileSize : bytesPerCluster;
        fwrite(buffer, 1, toWrite, out);
        fileSize -= toWrite;
        free(buffer);
        if (fileSize == 0) break;
        currentCluster = ((uint32_t *)FAT)[currentCluster];
    }
    fclose(out);
    printf("Recovery completed: %s\n", outputFilename);
}

#define MAX_LFN_ENTRIES 20

void findDeletedFilesWithLFN(FILE *disk, FAT32BootSector *bootSector, uint8_t *FAT, const char *targetFileName) {
    uint32_t clusterSize = bootSector->sectorsPerCluster * bootSector->bytesPerSector;
    uint8_t *clusterData = malloc(clusterSize);
    if (!clusterData) { perror("Malloc failed"); return; }
    readCluster(disk, bootSector, bootSector->rootCluster, clusterData);

    wchar_t lfnParts[MAX_LFN_ENTRIES][14];
    int lfnCount = 0;

    for (int offset = 0; offset < clusterSize; offset += 32) {
        uint8_t firstByte = clusterData[offset];
        if (firstByte == 0x00) break;
        uint8_t attr = clusterData[offset + 11];
        if (attr == 0x0F) {
            LFNEntry *lfn = (LFNEntry *)&clusterData[offset];
            if (lfnCount < MAX_LFN_ENTRIES) {
                int index = 0;
                memset(lfnParts[lfnCount], 0, sizeof lfnParts[lfnCount]);
                copyLFNChars(lfn->name1, 5, lfnParts[lfnCount], &index);
                copyLFNChars(lfn->name2, 6, lfnParts[lfnCount], &index);
                copyLFNChars(lfn->name3, 2, lfnParts[lfnCount], &index);
                lfnCount++;
            }
        } else {
            DirectoryEntry *entry = (DirectoryEntry *)&clusterData[offset];
            if ((entry->name[0] == 0xE5) && !(entry->attr & 0x08)) {
                char finalName[520] = {0};
                if (lfnCount > 0) {
                    wchar_t fullLFN[260] = {0};
                    int pos = 0;
                    for (int i = lfnCount - 1; i >= 0; i--) {
                        for (int j = 0; j < 13 && lfnParts[i][j]; j++) {
                            fullLFN[pos++] = lfnParts[i][j];
                        }
                    }
                    fullLFN[pos] = 0;
                    wcharToUtf8(fullLFN, finalName, sizeof(finalName));
                } else {
                    getShortNameDeleted(entry, finalName);
                }
                uint32_t startCluster = (entry->firstClusterHigh << 16) | entry->firstClusterLow;
                printf("Deleted file found: Name: %s  Start Cluster: %u  Size: %u bytes\n",
                       finalName, startCluster, entry->fileSize);
                if (strcmp(finalName, targetFileName) == 0) {
                    recoverDeletedFile(disk, bootSector, FAT, startCluster, entry->fileSize, "recovered_file.bin");
                }
            }
            lfnCount = 0;
        }
    }
    free(clusterData);
}
