// file: main.c
#include <stdio.h>
#include <stdlib.h>
#include "fat32_utils.h"

int main() {
    const char *diskImage = "\\\\.\\F:";  // replace the disk address as needed
    FILE *disk = fopen(diskImage, "rb");
    if (!disk) {
        perror("Error opening disk image");
        return 1;
    }

    FAT32BootSector bootSector;
    readBootSector(disk, &bootSector);

    printf("Bytes per Sector: %u\n", bootSector.bytesPerSector);
    printf("Sectors per Cluster: %u\n", bootSector.sectorsPerCluster);
    printf("Number of FATs: %u\n", bootSector.numFATs);
    printf("FAT Size (sectors): %u\n", bootSector.FATSize32);

    uint32_t FATSize = bootSector.FATSize32 * bootSector.bytesPerSector;
    uint8_t *FAT = malloc(FATSize);
    if (!FAT) {
        perror("Error allocating memory for FAT");
        fclose(disk);
        return 1;
    }

    readFAT(disk, &bootSector, FAT);

    for (int i = 0; i < 16; i++) {
        printf("FAT Entry %d: 0x%08X\n", i, ((uint32_t *)FAT)[i]);
    }

    printf("=== Deleted files on root directory ===\n");
    findDeletedFilesWithLFN(disk, &bootSector, FAT, "");

    char choice[260];
    printf("\nEnter the exact filename you want to recover (as shown above):\n> ");
    if (!fgets(choice, sizeof(choice), stdin)) {
        fprintf(stderr, "Input error\n");
        free(FAT);
        fclose(disk);
        return 1;
    }
    choice[strcspn(choice, "\r\n")] = '\0';

    printf("\n=== Attempting recovery of \"%s\" ===\n", choice);
    findDeletedFilesWithLFN(disk, &bootSector, FAT, choice);

    free(FAT);
    fclose(disk);
    return 0;
}
