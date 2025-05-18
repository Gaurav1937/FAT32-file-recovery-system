# FAT32 Deleted File Recovery Tool

## What is this project?

This project is a basic FAT32 deleted file recovery program written in C.  
It reads and parses the FAT32 file system structures such as the boot sector, FAT tables, directory entries, and long file name (LFN) entries to identify deleted files and potentially recover their data.

### Why this project?

Deleted files on FAT32 drives are not immediately erased; their directory entries are marked as deleted and their clusters are freed. This tool helps recover such files by scanning the file system and extracting deleted entries before the data is overwritten.

This is useful for data recovery on FAT32 formatted USB drives, SD cards, or partitions.

---

## Things to keep in mind before running

- The program must be run on a FAT32 formatted device or disk image.
- You need to compile the program with `gcc` (GNU compiler) on Linux or compatible systems.
- The program requires linking with the `wchar` library (`-lwchar`) for wide character support.
- Ensure you have permission to read the device or image file you want to recover from.
- This is a beginner-level recovery tool; it does **not** handle fragmented files or damaged FAT tables.

---

## How to build and run

### Build

Use the provided Makefile by running:

```bash
make
