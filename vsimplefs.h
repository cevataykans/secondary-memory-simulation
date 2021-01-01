

// Do not change this file //

#define MODE_READ 0
#define MODE_APPEND 1
#define BLOCKSIZE 4096 // bytes
#define MAX_FILE_NUMBER 112
#define MAX_FILENAME_LENGTH 64
#define MAX_OPENED_FILE_PER_PROCESS 16
#define FAT_ENTRY_SIZE 8 // byte
#define DIR_ENTRY_SIZE 256 // byte
#define SUPERBLOCK_START 0
#define SUPERBLOCK_COUNT 1
#define DIR_START 1
#define DIR_COUNT 7
#define FAT_START 8
#define FAT_COUNT 256
#define DATA_START 264
#define DIR_ENTRY_PER_BLOCK 16
#define FAT_ENTRY_PER_BLOCK 512

struct DirectoryEntry{
    char name[MAX_FILENAME_LENGTH];
    int size;
    int fatIndex;
};

struct File{
    int directoryBlock;
    int directoryBlockOffset;
    struct DirectoryEntry directoryEntry;
    int openMode;
    int readPointer;
};

int create_format_vdisk (char *vdiskname, unsigned int m);

int vsfs_mount (char *vdiskname);

int vsfs_umount (); 

int vsfs_create(char *filename);

int vsfs_open(char *filename, int mode);

int vsfs_close(int fd);

int vsfs_getsize (int fd);

int vsfs_read(int fd, void *buf, int n);

int vsfs_append(int fd, void *buf, int n);

int vsfs_delete(char *filename);

int getDataBlock(void* block, int fatIndex);

int putDataBlock(void* data, int fatIndex);

int getNextFATEntry(int fatIndex);

int getLastFATEntry(int fatIndex, int* lastIndex, int* blockNo);

int changeFATEntry(int fatIndex, int newNext);

void initFAT(int dataCount);

void initSuperblock(int dataCount);

void initDirectoryStructure();

void updateSuperblock();

void getSuperblock();

int checkOpened(int fd);

void updateDirectoryEntry(int fd);

void clearOpenTable();

void printDisk();

void printFile(char* filename);