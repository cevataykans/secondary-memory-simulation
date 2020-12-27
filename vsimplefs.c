#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdbool.h>
#include "vsimplefs.h"

int DATA_COUNT;

int openTable[MAX_OPENED_FILE_PER_PROCESS]; // keeps indices for files array or -1
int fileMode[MAX_OPENED_FILE_PER_PROCESS];
int openedFileCount = 0;
bool fatInit = false;

struct File{
    char* name;
    int directoryBlockNum; // starts from 1
    int directoryBlockOffset;
    int openCount;
};

// Global Variables =======================================
int vdisk_fd; // Global virtual disk file descriptor. Global within the library.
              // Will be assigned with the vsfs_mount call.
              // Any function in this file can use this.
              // Applications will not use  this directly. 
// ========================================================
struct File files[MAX_FILE_NUMBER];
int file_count = 0;

int emptyFAT;
int freeBlockCount;

// read block k from disk (virtual disk) into buffer block.
// size of the block is BLOCKSIZE.
// space for block must be allocated outside of this function.
// block numbers start from 0 in the virtual disk. 
int read_block (void *block, int k)
{
    int n;
    int offset;

    offset = k * BLOCKSIZE;
    lseek(vdisk_fd, (off_t) offset, SEEK_SET);
    n = read (vdisk_fd, block, BLOCKSIZE);
    if (n != BLOCKSIZE) {
	printf ("read error\n");
	return -1;
    }
    return (0); 
}

// write block k into the virtual disk. 
int write_block (void *block, int k)
{
    int n;
    int offset;

    offset = k * BLOCKSIZE;
    lseek(vdisk_fd, (off_t) offset, SEEK_SET);
    n = write (vdisk_fd, block, BLOCKSIZE);
    if (n != BLOCKSIZE) {
	printf ("write error\n");
	return (-1);
    }
    return 0; 
}


/**********************************************************************
   The following functions are to be called by applications directly. 
***********************************************************************/

// this function is partially implemented.
int create_format_vdisk (char *vdiskname, unsigned int m)
{
    char command[1000];
    int size;
    int num = 1;
    int count;
    size  = num << m;
    printf("size: %d\n", size);
    count = size / BLOCKSIZE;
    if(count < SUPERBLOCK_COUNT + DIR_COUNT + FAT_COUNT){
        printf("Please create a disk with a larger size!\n");
        return -1;
    }

    //    printf ("%d %d", m, size);
    sprintf (command, "dd if=/dev/zero of=%s bs=%d count=%d",
             vdiskname, BLOCKSIZE, count);
    //printf ("executing command = %s\n", command);
    system (command);

    // now write the code to format the disk below.
    // .. your code...

    // ?????????
    DATA_COUNT = count - SUPERBLOCK_COUNT - DIR_COUNT - FAT_COUNT;
    for(int i = 0 ; i < MAX_FILE_NUMBER ; i++){
        files[i].name = NULL;
    }
    file_count = 0;

    for(int i = 0 ; i < MAX_OPENED_FILE_PER_PROCESS ; i++){
        openTable[i] = -1;
    }
    openedFileCount = 0;
    printf("At the end of creat and format, openFile");
    
    return (0); 
}

// already implemented
int vsfs_mount (char *vdiskname)
{
    // simply open the Linux file vdiskname and in this
    // way make it ready to be used for other operations.
    // vdisk_fd is global; hence other function can use it. 
    vdisk_fd = open(vdiskname, O_RDWR); 
    return(0);
}


// already implemented
int vsfs_umount ()
{
    fsync (vdisk_fd); // copy everything in memory to disk
    close (vdisk_fd);
    return (0); 
}

int vsfs_create(char *filename)
{   
    if(!fatInit){
        initFAT();
        fatInit = true;
    }
    printf("Check1\n");
    if(file_count == MAX_FILE_NUMBER){
        printf("No space for create a new file\n");
        return -1;
    }
    int firstEmpty = -1;
    for(int i = 0 ; i < MAX_FILE_NUMBER ; i++){
        if(files[i].name == NULL){
            firstEmpty = i;
            break;
        }
    }
    printf("Check2\n");
    int offset = firstEmpty / DIR_ENTRY_PER_BLOCK;
    int blockNum = offset + DIR_START;
    
    void* block = (void*) malloc(BLOCKSIZE);
    int res = read_block(block, blockNum);
    if(res == -1){
        printf("read error in create\n");
        free(block);
        return -1;
    }
    for(int i = 0 ; i < file_count ; i++){
        printf("%s, %d, %d\n", ((char*)(block + i*DIR_ENTRY_SIZE)), ((int*)(block + i*DIR_ENTRY_SIZE + 64))[0], ((int*)(block + i*DIR_ENTRY_SIZE + 68))[0]);
    }
    int blockOffset = firstEmpty % DIR_ENTRY_PER_BLOCK;
    int startByte = blockOffset * DIR_ENTRY_SIZE;

    for(int i = 0 ; i < strlen(filename) ; i++){
        ((char*)block + startByte + i)[0] = filename[i];
    }
    printf("Check4\n");
    ((int*)(block + startByte + 64))[0] = 0; // size
    ((int*)(block + startByte + 68))[0] = -1; // point to FAT entry yet
    printf("Check5\n");
    res = write_block(block, blockNum);
    if(res == -1){
        printf("write error in create\n");
        free(block);
        return -1;
    }
    printf("Check6\n");
    files[firstEmpty].name = filename;
    files[firstEmpty].directoryBlockNum = blockNum;
    files[firstEmpty].directoryBlockOffset = blockOffset;
    files[firstEmpty].openCount = 0;
    file_count++;
    printf("create done\n");
    free(block);
    return (0);
}


int vsfs_open(char *file, int mode)
{   
    printf("Check1\n");
    if(openedFileCount == 16){
        printf("Too much opened file!\n");
        return -1;
    }
    printf("Check2\n");
    // first find it in files array
    int fd = -1;
    for(int i = 0 ; i < MAX_FILE_NUMBER ; i++){
        if(strcmp(files[i].name, file) == 0){ // we found the file
            fd = i;
            break;
        }
    }
    printf("Check3\n");
    if(fd == -1){
        printf("No such a file (open)\n");
        return -1;
    }

    // then check if it is already opened
    for(int i = 0 ; i < MAX_OPENED_FILE_PER_PROCESS ; i++){
        if(openTable[i] == fd){ // it is already opened!
            printf("It is already opened! %d %d\n", openTable[i], fd);
            return -1;
        }
    }
    printf("Check4\n");
    int firstEmpty = -1;
    for(int i = 0 ; i < MAX_OPENED_FILE_PER_PROCESS ; i++){
        if(openTable[i] == -1){
            firstEmpty = i;
        }
    }
    printf("Check5\n");
    openTable[firstEmpty] = fd;
    fileMode[firstEmpty] = mode;
    files[fd].openCount++;
    openedFileCount++;
    printf("Open done\n");
    return fd; 
}

int vsfs_close(int fd){
    int index = checkOpened(fd);
    if(index < 0){
        printf("It couldnt be found in open table\n");
        return -1;
    }
    openTable[index] = -1;
    openedFileCount--;

    files[fd].openCount--;
    return (0); 
}

int vsfs_getsize (int  fd)
{
    if(checkOpened(fd) == -1){
        printf("The file is not open!\n");
        return -1;
    }
    int blockNum = files[fd].directoryBlockNum;
    int blockOffset = files[fd].directoryBlockOffset;
    int size, fatIndex;
    if(getDirectoryEntry(blockNum, blockOffset, &size, &fatIndex) == -1){
        return -1;
    }
    return size; 
}

int vsfs_read(int fd, void *buf, int n){
    int index = checkOpened(fd);
    if(index == -1){
        printf("The file is not opened!\n");
        return -1;
    }
    // check mode
    if(fileMode[index] == MODE_APPEND){
        printf("It is append mode!\n");
        return -1;
    }

    int blockNum = files[fd].directoryBlockNum;
    int blockOffset = files[fd].directoryBlockOffset;
    int size, fatIndex;
    if(getDirectoryEntry(blockNum, blockOffset, &size, &fatIndex) == -1){
        return -1;
    }

    if(size < n){
        printf("Actual size is lower!\n");
        return -1;
    }

    int blockCount = n / BLOCKSIZE;
    blockOffset = n % BLOCKSIZE;

    void* bufPtr = buf;
    void* block = (void*) malloc(BLOCKSIZE);
    for(int i = 0 ; i < blockCount + 1 ; i++){
        fatIndex = getDataBlock(block, fatIndex);
        if(i < blockCount){ // read the whole block
            *((char*)bufPtr) = *((char*)block);
            bufPtr += BLOCKSIZE;
        }
        else{
            for(int j = 0 ; j < blockOffset ; j++){
                ((char*)bufPtr)[0] = ((char*)block)[j];
                bufPtr += 1;
            }
        }
    }
    free(block);
    return (0); 
}

int vsfs_append(int fd, void *buf, int n)
{
    if(n <= 0){
        printf("n is %d\n", n);
        return -1;
    }

    int index = checkOpened(fd);
    if(index == -1){
        printf("The file is not opened!\n");
        return -1;
    }
    // check mode
    if(fileMode[index] == MODE_READ){
        printf("It is read mode!\n");
        return -1;
    }

    printf("Done basic checks\n");

    int blockNum = files[fd].directoryBlockNum;
    int blockOffset = files[fd].directoryBlockOffset;
    int size, fatIndex;
    if(getDirectoryEntry(blockNum, blockOffset, &size, &fatIndex) == -1){
        return -1;
    }
    printf("Get directory entry\n");
    int dataBlockOffset = size % BLOCKSIZE;
    if(dataBlockOffset == 0){
        dataBlockOffset = BLOCKSIZE;
    }
    int remainingByte = BLOCKSIZE - dataBlockOffset; // for the last block
    int requiredBlockCount = (n - remainingByte - 1) / BLOCKSIZE + 1;
    if(n <= remainingByte){
        requiredBlockCount = 0;
    }
    if(requiredBlockCount > freeBlockCount){
        printf("No enough space!\n");
        return -1;
    }
    printf("Calculate byte things\n");

    void* block = (void*) malloc(BLOCKSIZE);
    int lastFAT, blockNo;

    if(size == 0){ // the file is empty
        printf("File is empty\n");
        changeDirectoryFATEntry(blockNum, blockOffset, emptyFAT);
        fatIndex = emptyFAT;
        int byteCount = 0;
        for(int i = 0 ; i < requiredBlockCount - 1; i++){
            for(int j = 0 ; j < BLOCKSIZE ; j++){
                ((char*)block)[j] = ((char*)buf)[byteCount];
                byteCount++;
            }
            /*blockNo = getBlockNo(fatIndex);
            if(blockNo == -1){
                printf("No data block\n");
                free(block);
                return -1;
            }
            if(write_block(block, blockNo) == -1){
                printf("Write error in get data block\n");
                free(block);
                return -1;
            }*/
            if(putDataBlock(block, fatIndex) == -1){
                return -1;
            }
            fatIndex = getNextFATEntry(fatIndex);
        }
        emptyFAT = getNextFATEntry(fatIndex);
        if(changeFATEntry(fatIndex, -1) == -1){
            free(block);
            return -1;
        }

        int i = 0;
        while(byteCount < n){
            ((char*)block)[i] = ((char*)buf)[byteCount];
            i++;
            byteCount++;
        }

        /*blockNo = getBlockNo(fatIndex);
        if(blockNo == -1){
            printf("No data block\n");
            free(block);
            return -1;
        }
        if(write_block(block, blockNo) == -1){
            printf("Write error in get data block\n");
            free(block);
            return -1;
        }*/
        if(putDataBlock(block, fatIndex) == -1){
            return -1;
        }
    }

    else{
        int byteCount = 0;

        if(getLastFATEntry(fatIndex, &lastFAT, &blockNo) == -1){
            return -1;
        }

        if(read_block(block, blockNo) == -1){
            printf("Read error in get data block\n");
            return -1;
        }

        for(int i = dataBlockOffset ; i < BLOCKSIZE ; i++){
            if(byteCount == n){
                break;
            }
            ((char*)block)[i] = ((char*)buf)[byteCount];
            byteCount += 1;
        }

        if(write_block(block, blockNo) == -1){
            printf("Write error in get data block\n");
            free(block);
            return -1;
        }

        if(requiredBlockCount == 0){
            free(block);
            return 0;
        }

        if(changeFATEntry(lastFAT, emptyFAT) == -1){
            free(block);
            return -1;
        }

        fatIndex = emptyFAT;
        for(int i = 0 ; i < requiredBlockCount - 1; i++){
            for(int j = 0 ; j < BLOCKSIZE ; j++){
                ((char*)block)[j] = ((char*)buf)[byteCount];
                byteCount++;
            }
            /*blockNo = getBlockNo(fatIndex);
            if(blockNo == -1){
                printf("No data block\n");
                free(block);
                return -1;
            }
            if(write_block(block, blockNo) == -1){
                printf("Write error in get data block\n");
                free(block);
                return -1;
            }*/
            if(putDataBlock(block, fatIndex) == -1){
                return -1;
            }
            fatIndex = getNextFATEntry(fatIndex);
        }
        emptyFAT = getNextFATEntry(fatIndex);
        if(changeFATEntry(fatIndex, -1) == -1){
            free(block);
            return -1;
        }

        int i = 0;
        while(byteCount < n){
            ((char*)block)[i] = ((char*)buf)[byteCount];
            i++;
            byteCount++;
        }

        /*blockNo = getBlockNo(fatIndex);
        if(blockNo == -1){
            printf("No data block\n");
            free(block);
            return -1;
        }
        if(write_block(block, blockNo) == -1){
            printf("Write error in get data block\n");
            free(block);
            return -1;
        }*/
        if(putDataBlock(block, fatIndex) == -1){
            return -1;
        }
    }

    changeDirectorySize(blockNum, blockOffset, size + n);
    return (0); 
}

int vsfs_delete(char *filename)
{
    // first find it in files array
    int fd = -1;
    for(int i = 0 ; i < MAX_FILE_NUMBER ; i++){
        if(strcmp(files[i].name, filename) == 0){ // we found the file
            fd = i;
            break;
        }
    }
    if(fd == -1){
        printf("No such a file (delete)\n");
        return -1;
    }
    if(files[fd].openCount > 0){
        printf("It is still open!\n");
        return -1;
    }

    // to do delete fat
    int blockNum = files[fd].directoryBlockNum;
    int blockOffset = files[fd].directoryBlockOffset;
    int size, fatIndex;
    if(getDirectoryEntry(blockNum, blockOffset, &size, &fatIndex) == -1){
        printf("Couldnt get the directory\n");
        return -1;
    }

    if(fatIndex > -1){    
        int lastFATIndex, blockNo;
        if(getLastFATEntry(fatIndex, &lastFATIndex, &blockNo) == -1){
            printf("Error while get last fat entry function\n");
            return -1;
        }
        if(changeFATEntry(lastFATIndex, emptyFAT) == -1){
            printf("FAT entry couldnt be changed!\n");
            return -1;
        }
        emptyFAT = fatIndex;
    }

    files[fd].name = NULL;
    file_count--;
    return 0;
}

// helper functions

int getDataBlock(void* block, int fatIndex){
    int blockNum = FAT_START + fatIndex / FAT_ENTRY_PER_BLOCK;
    int blockOffset = fatIndex % FAT_ENTRY_PER_BLOCK;
    int startByte = blockOffset * FAT_ENTRY_SIZE;
    if(read_block(block, blockNum) == -1){
        printf("Read error in get data block\n");
        return -1;
    }
    int nextIndex = ((int*)(block + startByte))[0];
    int dataBlock = ((int*)(block + startByte + 4))[0];
    if(read_block(block, dataBlock + DATA_START) == -1){
        printf("Read error in get data block\n");
        return -1;
    }
    return nextIndex;
}

int putDataBlock(void* data, int fatIndex){
    void* block = (void*) malloc(BLOCKSIZE);
    int blockNum = FAT_START + fatIndex / FAT_ENTRY_PER_BLOCK;
    int blockOffset = fatIndex % FAT_ENTRY_PER_BLOCK;
    int startByte = blockOffset * FAT_ENTRY_SIZE;
    if(read_block(block, blockNum) == -1){
        printf("Read error in get data block\n");
        free(block);
        return -1;
    }

    int dataBlock = ((int*)(block + startByte + 4))[0];
    if(write_block(data, dataBlock + DATA_START) == -1){
        printf("Read error in get data block\n");
        free(block);
        return -1;
    }
    free(block);
    return 0;
}

int getNextFATEntry(int fatIndex){
    void* block = (void*) malloc(BLOCKSIZE);
    int blockNum = FAT_START + fatIndex / FAT_ENTRY_PER_BLOCK;
    int blockOffset = fatIndex % FAT_ENTRY_PER_BLOCK;
    int startByte = blockOffset * FAT_ENTRY_SIZE;
    if(read_block(block, blockNum) == -1){
        printf("Read error in get data block\n");
        free(block);
        return -1;
    }
    int nextIndex = ((int*)(block + startByte))[0];
    free(block);
    return nextIndex;
}

int getBlockNo(int fatIndex){
    void* block = (void*) malloc(BLOCKSIZE);
    int blockNum = FAT_START + fatIndex / FAT_ENTRY_PER_BLOCK;
    int blockOffset = fatIndex % FAT_ENTRY_PER_BLOCK;
    int startByte = blockOffset * FAT_ENTRY_SIZE;
    if(read_block(block, blockNum) == -1){
        printf("Read error in get data block\n");
        free(block);
        return -1;
    }
    int dataBlock = ((int*)(block + startByte + 4))[0];
    free(block);
    return dataBlock + DATA_START;
}

int getLastFATEntry(int fatIndex, int* lastIndex, int* blockNo){ // exact block no
    void* block = (void*) malloc(BLOCKSIZE);
    int prevFatIndex;
    int curFatIndex = fatIndex;
    while(curFatIndex != -1){    
        int blockNum = FAT_START + fatIndex / FAT_ENTRY_PER_BLOCK;
        int blockOffset = fatIndex % FAT_ENTRY_PER_BLOCK;
        int startByte = blockOffset * FAT_ENTRY_SIZE;
        if(read_block(block, blockNum) == -1){
            printf("Read error in get data block\n");
            free(block);
            return -1;
        }
        prevFatIndex = curFatIndex;
        curFatIndex = ((int*)(block + startByte))[0];
        *blockNo = ((int*)(block + startByte + 4))[0] + DATA_START;
    }
    *lastIndex = prevFatIndex;
    free(block);
    return 0;
}

int changeFATEntry(int fatIndex, int newNext){
    void* block = (void*) malloc(BLOCKSIZE); 

    int blockNum = FAT_START + fatIndex / FAT_ENTRY_PER_BLOCK;
    int blockOffset = fatIndex % FAT_ENTRY_PER_BLOCK;
    int startByte = blockOffset * FAT_ENTRY_SIZE;

    if(read_block(block, blockNum) == -1){
        printf("Read error in get data block\n");
        free(block);
        return -1;
    }
    ((int*)(block + startByte))[0] = newNext;
    
    if(write_block(block, blockNum) == -1){
        printf("Write error in get data block\n");
        free(block);
        return -1;
    }

    free(block);
    return 0;
}

int getDirectoryEntry(int blockNo, int blockOffset, int* size, int* fatIndex){
    void* block = (void*) malloc(BLOCKSIZE);
    if(read_block(block, blockNo) == -1){
        printf("Read problem in get dir\n");
        free(block);
        return -1;
    }
    int startByte = blockOffset * DIR_ENTRY_SIZE;
    void* readPtr = block + startByte;
    *size = ((int*)(readPtr + 64))[0];
    *fatIndex = ((int*)(readPtr + 68))[0];
    free(block);
    return 0;
}

int changeDirectoryFATEntry(int blockNo, int blockOffset, int fatIndex){
    void* block = (void*) malloc(BLOCKSIZE);
    if(read_block(block, blockNo) == -1){
        printf("Read problem in get dir\n");
        free(block);
        return -1;
    }
    int startByte = blockOffset * DIR_ENTRY_SIZE;
    ((int*)(block + startByte + 68))[0] = fatIndex;
    if(write_block(block, blockNo) == -1){
        printf("Write problem in get dir\n");
        free(block);
        return -1;
    }
    free(block);
    return 0;
}

int changeDirectorySize(int blockNo, int blockOffset, int size){
    void* block = (void*) malloc(BLOCKSIZE);
    if(read_block(block, blockNo) == -1){
        printf("Read problem in get dir\n");
        free(block);
        return -1;
    }
    int startByte = blockOffset * DIR_ENTRY_SIZE;
    ((int*) (block + startByte + 64))[0] = size;
    if(write_block(block, blockNo) == -1){
        printf("Write problem in get dir\n");
        free(block);
        return -1;
    }
    free(block);
    return 0;
}
void initFAT(){
    void* block = (void*) malloc(BLOCKSIZE);
    int blockCount = DATA_COUNT / FAT_ENTRY_PER_BLOCK;
    int blockOffset = DATA_COUNT % FAT_ENTRY_PER_BLOCK;
    for(int i = 0 ; i < blockCount + 1 ; i++){
        for(int j = 0 ; j < FAT_ENTRY_PER_BLOCK ; j++){
            ((int*)(block + j*FAT_ENTRY_SIZE))[0] = FAT_ENTRY_PER_BLOCK * i + j + 1;
            ((int*)(block + j*FAT_ENTRY_SIZE + 4))[0] = FAT_ENTRY_PER_BLOCK * i + j; // block num
            if(i == blockCount && j == blockOffset - 1){
                break;
            }
        }
        write_block(block, FAT_START + i);
    }
    emptyFAT = 0;
    freeBlockCount = DATA_COUNT;
    free(block);
}

int checkOpened(int fd){
    int index = -1;
    for(int i = 0 ; i < MAX_OPENED_FILE_PER_PROCESS ; i++){
        if(openTable[i] == fd){
            index = i;
            break;
        }
    }
    return index;
}