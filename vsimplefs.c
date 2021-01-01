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
int emptyFAT;
int freeBlockCount;
int fileCount;

struct File openTable[MAX_OPENED_FILE_PER_PROCESS]; // keeps indices for files array or -1
int openedFileCount = 0;

// Global Variables =======================================
int vdisk_fd; // Global virtual disk file descriptor. Global within the library.
              // Will be assigned with the vsfs_mount call.
              // Any function in this file can use this.
              // Applications will not use  this directly. 
// ========================================================


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
    printf("Ch\n");
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

    /*for(int i = 0 ; i < MAX_FILE_NUMBER ; i++){
        files[i].name = NULL;
    }
    file_count = 0;*/

    /*for(int i = 0 ; i < MAX_OPENED_FILE_PER_PROCESS ; i++){
        openTable[i] = -1;
    }*/
    //openedFileCount = 0;
    vdisk_fd = open(vdiskname, O_RDWR);
    int dataCount = count - SUPERBLOCK_COUNT - DIR_COUNT - FAT_COUNT;
    initSuperblock(dataCount);
    initFAT(dataCount);
    initDirectoryStructure();
    printf("At the end of creat and format, openFile");
    fsync (vdisk_fd); // copy everything in memory to disk
    close (vdisk_fd);
    return (0); 
}

// already implemented
int vsfs_mount (char *vdiskname)
{
    // simply open the Linux file vdiskname and in this
    // way make it ready to be used for other operations.
    // vdisk_fd is global; hence other function can use it. 
    vdisk_fd = open(vdiskname, O_RDWR); 
    getSuperblock();
    clearOpenTable();
    return(0);
}


// already implemented
int vsfs_umount ()
{
    updateSuperblock();
    for(int i = 0 ; i < MAX_OPENED_FILE_PER_PROCESS ; i++){
        if(openTable[i].directoryBlock > -1){
            vsfs_close(i);
        }
    }
    fsync (vdisk_fd); // copy everything in memory to disk
    close (vdisk_fd);
    return (0); 
}

int vsfs_create(char *filename)
{   
    /*if(!fatInit){
        initFAT();
        fatInit = true;
    }*/
    //printf("Check1\n");
    if(fileCount == MAX_FILE_NUMBER){
        printf("No space for create a new file\n");
        return -1;
    }

    // name check
    void* block = (void*) malloc(BLOCKSIZE);
    for(int i = 0 ; i < DIR_COUNT ; i++){
        read_block(block, DIR_START + i);
        for(int j = 0 ; j < DIR_ENTRY_PER_BLOCK ; j++){
            int startByte = j * DIR_ENTRY_SIZE;
            char isUsed = ((char*)(block + startByte + 72))[0];
            char* name = ((char*)(block + startByte));
            if(isUsed == 1 && strcmp(name, filename) == 0){
                printf("Already created!\n");
                free(block);
                return -1;
            }
        }
    }

    int dirBlock = -1; // relative to DIR_START
    int dirBlockOffset = -1;
    bool found = false;
    for(int i = 0 ; i < DIR_COUNT ; i++){
        read_block(block, DIR_START + i);
        for(int j = 0 ; j < DIR_ENTRY_PER_BLOCK ; j++){
            int startByte = j * DIR_ENTRY_SIZE;
            char isUsed = ((char*)(block + startByte + 72))[0];
            if(isUsed == 0){
                dirBlock = i;
                dirBlockOffset = j;
                printf("Empty dir entry is found %d, %d!\n", i, j);
                found = true;
                break; 
            }
        }
        if(found){
            break;
        }
    }
    //printf("Check2\n");
    /*int offset = firstEmpty / DIR_ENTRY_PER_BLOCK;
    int blockNum = offset + DIR_START;
    
    
    int res = read_block(block, blockNum);
    if(res == -1){
        printf("read error in create\n");
        free(block);
        return -1;
    }*/
    /*for(int i = 0 ; i < file_count ; i++){
        printf("%s, %d, %d\n", ((char*)(block + i*DIR_ENTRY_SIZE)), ((int*)(block + i*DIR_ENTRY_SIZE + 64))[0], ((int*)(block + i*DIR_ENTRY_SIZE + 68))[0]);
    }*/
    //int blockOffset = firstEmpty % DIR_ENTRY_PER_BLOCK;
    int startByte = dirBlockOffset * DIR_ENTRY_SIZE;

    for(int i = 0 ; i < strlen(filename) ; i++){
        ((char*)block + startByte + i)[0] = filename[i];
    }
    //printf("Check4\n");
    ((int*)(block + startByte + 64))[0] = 0; // size
    ((int*)(block + startByte + 68))[0] = -1; // point to FAT entry yet
    ((char*)(block + startByte + 72))[0] = 1; // it is used now
    //printf("Check5\n");
    int res = write_block(block, dirBlock + DIR_START);
    if(res == -1){
        printf("write error in create\n");
        free(block);
        return -1;
    }
    //printf("Check6\n");
    /*files[firstEmpty].name = filename;
    files[firstEmpty].directoryBlockNum = blockNum;
    files[firstEmpty].directoryBlockOffset = blockOffset;
    files[firstEmpty].openCount = 0;*/
    fileCount++;
    //printf("create done\n");
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
    for(int i = 0 ; i < MAX_OPENED_FILE_PER_PROCESS ; i++){
        if(openTable[i].directoryBlock > -1 && strcmp(openTable[i].directoryEntry.name, file) == 0){ // it is used
            printf("It is already opened\n");
            return -1;
        }
    }

    int fd = -1;
    for(int i = 0 ; i < MAX_OPENED_FILE_PER_PROCESS ; i++){
        if(openTable[i].directoryBlock == -1){ // it is empty
            fd = i;
            break;
        }
    }
    // first find it in files in directory structure
    bool found = false;
    void* block = (void*) malloc(BLOCKSIZE);
    for(int i = 0 ; i < DIR_COUNT ; i++){
        read_block(block, DIR_START + i);
        for(int j = 0 ; j < DIR_ENTRY_PER_BLOCK ; j++){
            int startByte = j * DIR_ENTRY_SIZE;
            char isUsed = ((char*)(block + startByte + 72))[0];
            char* filename = ((char*)(block + startByte));
            if(isUsed == 1 && strcmp(file, filename) == 0){ // we found the directory entry
                openTable[fd].directoryBlock = i;
                openTable[fd].directoryBlockOffset = j;
                openTable[fd].openMode = mode;
                openTable[fd].readPointer = 0;
                openTable[fd].directoryEntry.name = filename;
                openTable[fd].directoryEntry.size = ((int*)(block + startByte + 64))[0];
                openTable[fd].directoryEntry.fatIndex = ((int*)(block + startByte + 68))[0];
                openedFileCount++;
                found = true;
                break;
            }
        }
        if(found) break;
    }
    free(block);
    //printf("Open done\n");
    return fd; 
}

int vsfs_close(int fd){
    if(checkOpened(fd) == -1){
        printf("It couldnt be found in open table\n");
        return -1;
    }

    updateDirectoryEntry(fd);

    openTable[fd].directoryBlock = -1;
    openedFileCount--;
    return (0); 
}

int vsfs_getsize (int  fd)
{
    if(checkOpened(fd) == -1){
        printf("The file is not open!\n");
        return -1;
    }
    return openTable[fd].directoryEntry.size; 
}

int vsfs_read(int fd, void *buf, int n){
    if(checkOpened(fd) == -1){
        printf("The file is not opened!\n");
        return -1;
    }
    // check mode
    if(openTable[fd].openMode == MODE_APPEND){
        printf("It is append mode!\n");
        return -1;
    }

    int size = openTable[fd].directoryEntry.size; 
    int fatIndex = openTable[fd].directoryEntry.fatIndex;
    int readPtr = openTable[fd].readPointer;

    if(size < readPtr + n){
        printf("Actual size is lower!\n");
        return -1;
    }
    int startBlockCount = readPtr / BLOCKSIZE;
    int startBlockOffset = readPtr % BLOCKSIZE;

    int endBlockCount = (readPtr + n) / BLOCKSIZE;
    int endBlockOffset = (readPtr + n) % BLOCKSIZE;

    void* bufPtr = buf;
    void* block = (void*) malloc(BLOCKSIZE);
    for(int i = 0 ; i < endBlockCount + 1 ; i++){
        if(i < startBlockCount){
            fatIndex = getNextFATEntry(fatIndex);
        }
        else{
            fatIndex = getDataBlock(block, fatIndex);
            if(i == startBlockCount){
                for(int j = startBlockOffset ; j < BLOCKSIZE ; j++){
                    ((char*)(bufPtr))[0] = ((char*)(block + j))[0];
                    bufPtr += 1;
                }
            }
            else if(i == endBlockCount){
                for(int j = 0 ; j < endBlockOffset ; j++){
                    ((char*)(bufPtr))[0] = ((char*)(block + j))[0];
                    bufPtr += 1;
                }
            }
            else{
                for(int j = 0 ; j < BLOCKSIZE ; j++){
                    ((char*)(bufPtr))[0] = ((char*)(block + j))[0];
                    bufPtr += 1;
                }
            }
        }
        /*if(i < blockCount){ // read the whole block
            *((char*)bufPtr) = *((char*)block);
            bufPtr += BLOCKSIZE;
        }
        else{
            for(int j = 0 ; j < blockOffset ; j++){
                ((char*)bufPtr)[0] = ((char*)block)[j];
                bufPtr += 1;
            }
        }*/
    }
    free(block);
    openTable[fd].readPointer = readPtr + n;
    return (0); 
}

int vsfs_append(int fd, void *buf, int n)
{
    if(n <= 0){
        printf("n is %d\n", n);
        return -1;
    }

    if(checkOpened(fd) == -1){
        printf("The file is not opened!\n");
        return -1;
    }
    // check mode
    if(openTable[fd].openMode == MODE_READ){
        printf("It is read mode!\n");
        return -1;
    }

    //printf("Done basic checks\n");

    /*int blockNum = openTable[fd].dire;
    int blockOffset = files[fd].directoryBlockOffset;
    int size, fatIndex;
    if(getDirectoryEntry(blockNum, blockOffset, &size, &fatIndex) == -1){
        return -1;
    }*/

    int size = openTable[fd].directoryEntry.size; 
    int fatIndex = openTable[fd].directoryEntry.fatIndex;
    printf("%s: %d\n", openTable[fd].directoryEntry.name, fatIndex);
    //printf("Get directory entry\n");
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
    //printf("Calculate byte things\n");

    void* block = (void*) malloc(BLOCKSIZE);
    int lastFAT, blockNo;

    if(size == 0){ // the file is empty
        printf("File is empty\n");
        //changeDirectoryFATEntry(blockNum, blockOffset, emptyFAT);
        openTable[fd].directoryEntry.fatIndex = emptyFAT;
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
            printf("%s: The current fat entry is: %d\n", openTable[fd].directoryEntry.name, fatIndex);
            fatIndex = getNextFATEntry(fatIndex);
            printf("%s: The next fat entry is: %d\n", openTable[fd].directoryEntry.name, fatIndex);
        }
        emptyFAT = getNextFATEntry(fatIndex);
        printf("%s: The next fat entry is: %d %d\n", openTable[fd].directoryEntry.name, fatIndex, emptyFAT);
        printf("new empty fat is %d\n", emptyFAT);
        if(changeFATEntry(fatIndex, -1) == -1){
            printf("Problemmmm\n");
            free(block);
            return -1;
        }
        printf("Problemmmm111\n");
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
            printf("Problemmmm\n");
            return -1;
        }
        printf("Problemmmm33333\n");
    }

    else{
        int byteCount = 0;
        if(size == 4097){
            printf("Hello\n");
        }
        if(getLastFATEntry(fatIndex, &lastFAT, &blockNo) == -1){
            return -1;
        }
        if(size == 4097){
            printf("Hello\n");
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
            openTable[fd].directoryEntry.size = size + n;
            freeBlockCount -= requiredBlockCount;
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

        if(size == 4096){
            printf("New empty fat is %d\n", emptyFAT);
            printf("%d, %d\n", fatIndex, getNextFATEntry(fatIndex));
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
    printf("Here! %d\n", size);
    free(block);
    openTable[fd].directoryEntry.size = size + n;
    freeBlockCount -= requiredBlockCount;
    return (0); 
}

int vsfs_delete(char *file)
{
    // first find it in files array
    /*int fd = -1;
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
    }*/
    // make its dir structure invalid
    int fatIndex = -2;
    bool found = false;
    void* block = (void*) malloc(BLOCKSIZE);
    for(int i = 0 ; i < DIR_COUNT ; i++){
        read_block(block, DIR_START + i);
        for(int j = 0 ; j < DIR_ENTRY_PER_BLOCK ; j++){
            int startByte = j * DIR_ENTRY_SIZE;
            char isUsed = ((char*)(block + startByte + 72))[0];
            char* filename = ((char*)(block + startByte));
            if(isUsed == 1 && strcmp(file, filename) == 0){ // we found the directory entry
                ((char*)(block + startByte + 72))[0] = 0; // not used anymore
                fatIndex = ((int*)(block + startByte + 68))[0];
                found = true;
                break;
            }
        }
        if(found){
            write_block(block, DIR_START + i);
            break;
        }
    }
    if(fatIndex == -2){
        printf("No such a file!\n");
        free(block);
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

    free(block);
    fileCount--;
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
    //int dataBlock = ((int*)(block + startByte + 4))[0];
    //if(read_block(block, dataBlock + DATA_START) == -1){
    if(read_block(block, fatIndex + DATA_START) == -1){
        printf("Read error in get data block\n");
        return -1;
    }
    return nextIndex;
}

int putDataBlock(void* data, int fatIndex){
    void* block = (void*) malloc(BLOCKSIZE);
    /*int blockNum = FAT_START + fatIndex / FAT_ENTRY_PER_BLOCK;
    int blockOffset = fatIndex % FAT_ENTRY_PER_BLOCK;
    int startByte = blockOffset * FAT_ENTRY_SIZE;
    if(read_block(block, blockNum) == -1){
        printf("Read error in get data block\n");
        free(block);
        return -1;
    }*/

    int dataBlock = fatIndex; // ((int*)(block + startByte + 4))[0];
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
    /*void* block = (void*) malloc(BLOCKSIZE);
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
    return dataBlock + DATA_START;*/
    return fatIndex + DATA_START;
}

int getLastFATEntry(int fatIndex, int* lastIndex, int* blockNo){ // exact block no
    void* block = (void*) malloc(BLOCKSIZE);
    int prevFatIndex;
    int curFatIndex = fatIndex;
    //printf("%d\n", curFatIndex);
    while(curFatIndex != -1){
        int blockNum = FAT_START + curFatIndex / FAT_ENTRY_PER_BLOCK;
        int blockOffset = curFatIndex % FAT_ENTRY_PER_BLOCK;
        int startByte = blockOffset * FAT_ENTRY_SIZE;
        if(read_block(block, blockNum) == -1){
            printf("Read error in get data block\n");
            free(block);
            return -1;
        }
        prevFatIndex = curFatIndex;
        curFatIndex = ((int*)(block + startByte))[0];
        printf("%d, %d\n", prevFatIndex, curFatIndex);
        *blockNo = prevFatIndex + DATA_START; // ((int*)(block + startByte + 4))[0] + DATA_START;
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
void initFAT(int dataCount){
    void* block = (void*) malloc(BLOCKSIZE);
    int blockCount = dataCount / FAT_ENTRY_PER_BLOCK + 1;
    int blockOffset = dataCount % FAT_ENTRY_PER_BLOCK;
    for(int i = 0 ; i < blockCount; i++){
        for(int j = 0 ; j < FAT_ENTRY_PER_BLOCK ; j++){
            ((int*)(block + j*FAT_ENTRY_SIZE))[0] = FAT_ENTRY_PER_BLOCK * i + j + 1;
            //((int*)(block + j*FAT_ENTRY_SIZE + 4))[0] = FAT_ENTRY_PER_BLOCK * i + j; // block num
            if(i == blockCount - 1 && j == blockOffset - 1){
                ((int*)(block + j*FAT_ENTRY_SIZE))[0] = -1;
                break;
            }
        }
        write_block(block, FAT_START + i);
    }
    //emptyFAT = 0;
    //freeBlockCount = DATA_COUNT;
    free(block);
}

void initSuperblock(int dataCount){
    void* block = (void*) malloc(BLOCKSIZE);
    ((int*)(block))[0] = dataCount; // number of data block
    ((int*)(block + 4))[0] = 0; // empty FAT entry
    ((int*)(block + 8))[0] = dataCount; // number of free blocks
    ((int*)(block + 12))[0] = 0; // file count at the beginning
    write_block(block, SUPERBLOCK_START);
    free(block);
}

void initDirectoryStructure(){
    void* block = (void*) malloc(BLOCKSIZE);
    for(int j = 0 ; j < DIR_ENTRY_PER_BLOCK ; j++){
        int startByte = j * DIR_ENTRY_SIZE;
        ((char*)(block + startByte + 72))[0] = 0;
    }
    for(int i = 0 ; i < DIR_COUNT ; i++){
        write_block(block, DIR_START + i);

    }
    free(block);
}

void updateSuperblock(){
    void* block = (void*) malloc(BLOCKSIZE);
    read_block(block, SUPERBLOCK_START);
    ((int*)(block + 4))[0] = emptyFAT; // empty FAT entry
    ((int*)(block + 8))[0] = freeBlockCount; // empty free count
    ((int*)(block + 12))[0] = fileCount; // file count
    write_block(block, SUPERBLOCK_START);
    free(block);
}

void getSuperblock(){
    void* block = (void*) malloc(BLOCKSIZE);
    read_block(block, SUPERBLOCK_START);
    DATA_COUNT = ((int*)(block))[0];
    emptyFAT = ((int*)(block + 4))[0];
    freeBlockCount = ((int*)(block + 8))[0];
    fileCount = ((int*)(block + 12))[0];
    free(block);
}

int checkOpened(int fd){
    if(openTable[fd].directoryBlock < 0){
        return -1;
    }
    return 0;
}

void updateDirectoryEntry(int fd){
    void* block = (void*) malloc(BLOCKSIZE);
    read_block(block, openTable[fd].directoryBlock + DIR_START);
    int startByte = openTable[fd].directoryBlockOffset * DIR_ENTRY_SIZE;
    ((int*)(block + startByte + 64))[0] = openTable[fd].directoryEntry.size;
    ((int*)(block + startByte + 68))[0] = openTable[fd].directoryEntry.fatIndex;
    write_block(block, openTable[fd].directoryBlock + DIR_START);
    free(block);
}

void clearOpenTable(){
    for(int i = 0 ; i < MAX_OPENED_FILE_PER_PROCESS ; i++){
        openTable[i].directoryBlock = -1;
        openedFileCount = 0;
    }
}

void printDisk(){
    void* block = (void*) malloc(BLOCKSIZE);
    read_block(block, 0);
    printf("Superblock:\n");
    printf("\t# of data blocks: %d\n", ((int*)(block))[0]);
    printf("\tfirst empty FAT index: %d\n", ((int*)(block + 4))[0]);
    int curFatIndex = ((int*)(block + 4))[0];
    int emptyCount = 0;
    while(curFatIndex > -1){
        emptyCount++;
        curFatIndex = getNextFATEntry(curFatIndex);
    }
    printf("\t# of free data blocks: %d\n", ((int*)(block + 8))[0]);
    if(emptyCount != ((int*)(block + 8))[0]){
        printf("We are in trouble!\n");
        return;
    }
    printf("\t# of files: %d\n", ((int*)(block + 12))[0]);
    printf("Directory structure:\n");
    for(int i = DIR_START ; i < DIR_START + DIR_COUNT ; i++){
        read_block(block, i);
        for(int j = 0 ; j < DIR_ENTRY_PER_BLOCK ; j++){
            int startByte = j * DIR_ENTRY_SIZE;
            char isUsed = ((char*)(block + startByte + 72))[0];
            if(isUsed == 1){
                printf("\tDirectory entry %d:\n", (i - 1)* DIR_ENTRY_PER_BLOCK + j);
                printf("\t\tName: %s\n", ((char*)(block + startByte)));
                printf("\t\tSize: %d\n", ((int*)(block + startByte + 64))[0]);
                printf("\t\tFat index: %d\n", ((int*)(block + startByte + 68))[0]);
                curFatIndex = ((int*)(block + startByte + 68))[0];
                int blockCount = 0;
                int remainingByte = ((int*)(block + startByte + 64))[0];
                while(curFatIndex > -1){
                    printf("\t\t\tBlock %d:\n", blockCount);
                    blockCount++;
                    printf("\t\t\t\t");
                    getDataBlock(block, curFatIndex);
                    for(int k = 0 ; k < BLOCKSIZE ; k++){
                        printf("%c", ((char*)(block + k))[0]);
                        remainingByte--;
                        if(remainingByte == 0){
                            break;
                        }
                    }
                    if(remainingByte == 0){
                        printf("\n\t\t\tEnd of the file!\n");
                    }
                    curFatIndex = getNextFATEntry(curFatIndex);
                }
            }
        }
    }
    free(block);
}