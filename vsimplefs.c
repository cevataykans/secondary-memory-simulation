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

    sprintf (command, "dd if=/dev/zero of=%s bs=%d count=%d",
             vdiskname, BLOCKSIZE, count);

    system (command);

    // now write the code to format the disk below.
    // .. your code...

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
    if(fileCount == MAX_FILE_NUMBER){
        printf("No space for create a new file\n");
        return -1;
    }

    // name check
    char block[BLOCKSIZE];
    for(int i = 0 ; i < DIR_COUNT ; i++){
        read_block((void*)block, DIR_START + i);
        for(int j = 0 ; j < DIR_ENTRY_PER_BLOCK ; j++){
            int startByte = j * DIR_ENTRY_SIZE;
            char isUsed = ((char*)(block + startByte + 72))[0];
            char name[64];
            memcpy(name, ((char*)(block + startByte)), 64);
            if(isUsed == 49 && strcmp(name, filename) == 0){
                printf("Already created!\n");
                return -1;
            }
        }
    }

    int dirBlock = -1; // relative to DIR_START
    int dirBlockOffset = -1;
    bool found = false;
    for(int i = 0 ; i < DIR_COUNT ; i++){
        read_block((void*)block, DIR_START + i);
        for(int j = 0 ; j < DIR_ENTRY_PER_BLOCK ; j++){
            int startByte = j * DIR_ENTRY_SIZE;
            char isUsed = ((char*)(block + startByte + 72))[0];
            if(isUsed == 48){
                dirBlock = i;
                dirBlockOffset = j;
                //printf("Empty dir entry is found %d, %d!\n", i, j);
                found = true;
                break; 
            }
        }
        if(found){
            break;
        }
    }
    
    int startByte = dirBlockOffset * DIR_ENTRY_SIZE;

    for(int i = 0 ; i < strlen(filename) ; i++){
        ((char*)(block + startByte + i))[0] = filename[i];
    }
    memcpy(((char*)(block + startByte)), filename, 64);
    ((int*)(block + startByte + 64))[0] = 0; // size
    ((int*)(block + startByte + 68))[0] = -1; // point to FAT entry yet
    ((char*)(block + startByte + 72))[0] = 49; // it is used now

    int res = write_block((void*)block, dirBlock + DIR_START);
    if(res == -1){
        printf("write error in create\n");
        return -1;
    }
    
    fileCount++;
    return (0);
}


int vsfs_open(char *file, int mode)
{   
    if(openedFileCount == 16){
        printf("Too much opened file!\n");
        return -1;
    }

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
    char block[BLOCKSIZE];
    for(int i = 0 ; i < DIR_COUNT ; i++){
        read_block((void*)block, DIR_START + i);
        for(int j = 0 ; j < DIR_ENTRY_PER_BLOCK ; j++){
            int startByte = j * DIR_ENTRY_SIZE;
            char isUsed = ((char*)(block + startByte + 72))[0];
            char filename[64];
            memcpy(filename, ((char*)(block + startByte)), 64);
            if(isUsed == 49 && strcmp(file, filename) == 0){ // we found the directory entry
                openTable[fd].directoryBlock = i;
                openTable[fd].directoryBlockOffset = j;
                openTable[fd].openMode = mode;
                openTable[fd].readPointer = 0;
                memcpy(openTable[fd].directoryEntry.name, filename, 64);
                openTable[fd].directoryEntry.size = ((int*)(block + startByte + 64))[0];
                openTable[fd].directoryEntry.fatIndex = ((int*)(block + startByte + 68))[0];
                openedFileCount++;
                found = true;
                break;
            }
        }
        if(found) break;
    }
    return fd; 
}

int vsfs_close(int fd){
    if(checkOpened(fd) == -1){
        printf("It couldnt be found in open table\n");
        return -1;
    }

    updateDirectoryEntry(fd);
    updateSuperblock();
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
    char block[BLOCKSIZE];
    for(int i = 0 ; i < endBlockCount + 1 ; i++){
        if(i < startBlockCount){
            fatIndex = getNextFATEntry(fatIndex);
        }
        else{
            fatIndex = getDataBlock((void*)block, fatIndex);
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
    }
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

    int size = openTable[fd].directoryEntry.size; 
    int fatIndex = openTable[fd].directoryEntry.fatIndex;
    printf("Name: %s: %d\n", openTable[fd].directoryEntry.name, fatIndex);

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

    char block[BLOCKSIZE];
    int lastFAT, blockNo;

    if(size == 0){ // the file is empty
        int check = 0;
        //printf("File is empty\n");
        printf("File name is %lu (%d)\n", strlen(openTable[fd].directoryEntry.name), check);
        check++;
        openTable[fd].directoryEntry.fatIndex = emptyFAT;
        fatIndex = emptyFAT;
        int byteCount = 0;
        for(int i = 0 ; i < requiredBlockCount - 1; i++){
            for(int j = 0 ; j < BLOCKSIZE ; j++){
                ((char*)(block + j))[0] = ((char*)(buf + byteCount))[0];
                byteCount++;
            }
            
            if(putDataBlock((void*)block, fatIndex) == -1){
                return -1;
            }
            //printf("%s: The current fat entry is: %d\n", openTable[fd].directoryEntry.name, fatIndex);
            fatIndex = getNextFATEntry(fatIndex);
            //printf("%s: The next fat entry is: %d\n", openTable[fd].directoryEntry.name, fatIndex);
        }
        printf("File name is %lu (%d)\n", strlen(openTable[fd].directoryEntry.name), check);
        check++;
        emptyFAT = getNextFATEntry(fatIndex);
        printf("File name is %lu (%d)\n", strlen(openTable[fd].directoryEntry.name), check);
        check++;
        //printf("%s: The next fat entry is: %d %d\n", openTable[fd].directoryEntry.name, fatIndex, emptyFAT);
        //printf("new empty fat is %d\n", emptyFAT);
        if(changeFATEntry(fatIndex, -1) == -1){
            return -1;
        }
        printf("File name is %lu (%d)\n", strlen(openTable[fd].directoryEntry.name), check);
        check++;
        int i = 0;
        while(byteCount < n){
            if(byteCount < 270) printf("Inside while the name is %s (%d)\n",openTable[fd].directoryEntry.name,byteCount );
            ((char*)(block + i))[0] = ((char*)(buf + byteCount))[0];
            //printf("i: %d, byteCount: %d\n", i, byteCount);
            i += 1;
            byteCount += 1;
        }
        printf("Burada patladi!\n");
        printf("File name is %lu (%d)\n", strlen(openTable[fd].directoryEntry.name), check);
        check++;
        if(putDataBlock((void*)block, fatIndex) == -1){
            printf("Problemmmm\n");
            return -1;
        }
        //printf("File name is %s (%d)\n", openTable[fd].directoryEntry.name, check);
        check++;
        printf("Problemmmm33333\n");
    }

    else{
        int check = 0;
        int byteCount = 0;

        if(getLastFATEntry(fatIndex, &lastFAT, &blockNo) == -1){
            return -1;
        }
        
        getDataBlock((void*)block, lastFAT);
        //printf("File name is %s (%d)\n", openTable[fd].directoryEntry.name, check);
        check++;
        for(int i = dataBlockOffset ; i < BLOCKSIZE ; i++){
            if(byteCount == n){
                break;
            }
            ((char*)block)[i] = ((char*)buf)[byteCount];
            byteCount += 1;
        }
        //printf("File name is %s (%d)\n", openTable[fd].directoryEntry.name, check);
        check++;
        if(putDataBlock((void*)block, lastFAT) == -1){
            printf("Write error in get data block\n");
            return -1;
        }
        //printf("File name is %s (%d)\n", openTable[fd].directoryEntry.name, check);
        check++;
        if(requiredBlockCount == 0){
            openTable[fd].directoryEntry.size = size + n;
            freeBlockCount -= requiredBlockCount;
            return 0;
        }
        //printf("File name is %s (%d)\n", openTable[fd].directoryEntry.name, check);
        check++;
        if(changeFATEntry(lastFAT, emptyFAT) == -1){
            return -1;
        }
        //printf("File name is %s (%d)\n", openTable[fd].directoryEntry.name, check);
        check++;
        fatIndex = emptyFAT;
        for(int i = 0 ; i < requiredBlockCount - 1; i++){
            for(int j = 0 ; j < BLOCKSIZE ; j++){
                ((char*)block)[j] = ((char*)buf)[byteCount];
                byteCount++;
            }
            
            if(putDataBlock((void*)block, fatIndex) == -1){
                return -1;
            }
            fatIndex = getNextFATEntry(fatIndex);
        }
        //printf("File name is %s (%d)\n", openTable[fd].directoryEntry.name, check);
        check++;
        emptyFAT = getNextFATEntry(fatIndex);
        if(changeFATEntry(fatIndex, -1) == -1){
            return -1;
        }

        int i = 0;
        while(byteCount < n){
            ((char*)block)[i] = ((char*)buf)[byteCount];
            i++;
            byteCount++;
        }

        if(putDataBlock((void*)block, fatIndex) == -1){
            return -1;
        }
    }
    printf("Here! %d\n", size);
    openTable[fd].directoryEntry.size = size + n;
    freeBlockCount -= requiredBlockCount;
    return (0); 
}

int vsfs_delete(char *file)
{   
    for(int i = 0 ; i < MAX_OPENED_FILE_PER_PROCESS ; i++){
        if(openTable[i].directoryBlock > -1 && strcmp(file, openTable[i].directoryEntry.name) == 0){
            printf("The file is still open. It is being closing...\n");
            vsfs_close(i);
        }
    }
    // make its dir structure invalid
    int fatIndex = -2;
    bool found = false;
    char block[BLOCKSIZE];
    for(int i = 0 ; i < DIR_COUNT ; i++){
        read_block((void*)block, DIR_START + i);
        for(int j = 0 ; j < DIR_ENTRY_PER_BLOCK ; j++){
            int startByte = j * DIR_ENTRY_SIZE;
            char isUsed = ((char*)(block + startByte + 72))[0];
            char* filename = ((char*)(block + startByte));
            if(isUsed == 49 && strcmp(file, filename) == 0){ // we found the directory entry
                ((char*)(block + startByte + 72))[0] = 48; // not used anymore
                fatIndex = ((int*)(block + startByte + 68))[0];
                found = true;
                break;
            }
        }
        if(found){
            write_block((void*)block, DIR_START + i);
            break;
        }
    }
    if(fatIndex == -2){
        printf("No such a file!\n");
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
    if(read_block(block, fatIndex + DATA_START) == -1){
        printf("Read error in get data block\n");
        return -1;
    }
    return nextIndex;
}

int putDataBlock(void* data, int fatIndex){
    if(write_block(data, fatIndex + DATA_START) == -1){
        printf("Read error in get data block\n");
        return -1;
    }
    return 0;
}

int getNextFATEntry(int fatIndex){
    char block[BLOCKSIZE];
    int blockNum = FAT_START + fatIndex / FAT_ENTRY_PER_BLOCK;
    int blockOffset = fatIndex % FAT_ENTRY_PER_BLOCK;
    int startByte = blockOffset * FAT_ENTRY_SIZE;
    if(read_block((void*)block, blockNum) == -1){
        printf("Read error in get data block\n");
        return -1;
    }
    int nextIndex = ((int*)(block + startByte))[0];
    return nextIndex;
}

int getLastFATEntry(int fatIndex, int* lastIndex, int* blockNo){ // exact block no
    char block[BLOCKSIZE];
    int prevFatIndex;
    int curFatIndex = fatIndex;
    while(curFatIndex != -1){
        int blockNum = FAT_START + curFatIndex / FAT_ENTRY_PER_BLOCK;
        int blockOffset = curFatIndex % FAT_ENTRY_PER_BLOCK;
        int startByte = blockOffset * FAT_ENTRY_SIZE;
        if(read_block((void*)block, blockNum) == -1){
            printf("Read error in get data block\n");
            return -1;
        }
        prevFatIndex = curFatIndex;
        curFatIndex = ((int*)(block + startByte))[0];
        printf("%d, %d\n", prevFatIndex, curFatIndex);
        *blockNo = prevFatIndex + DATA_START;
    }
    *lastIndex = prevFatIndex;
    return 0;
}

int changeFATEntry(int fatIndex, int newNext){
    char block[BLOCKSIZE]; 

    int blockNum = FAT_START + fatIndex / FAT_ENTRY_PER_BLOCK;
    int blockOffset = fatIndex % FAT_ENTRY_PER_BLOCK;
    int startByte = blockOffset * FAT_ENTRY_SIZE;

    if(read_block((void*)block, blockNum) == -1){
        printf("Read error in get data block\n");
        return -1;
    }
    ((int*)(block + startByte))[0] = newNext;
    
    if(write_block((void*)block, blockNum) == -1){
        printf("Write error in get data block\n");
        return -1;
    }

    return 0;
}

void initFAT(int dataCount){
    char block[BLOCKSIZE];
    int blockCount = dataCount / FAT_ENTRY_PER_BLOCK + 1;
    int blockOffset = dataCount % FAT_ENTRY_PER_BLOCK;
    for(int i = 0 ; i < blockCount; i++){
        for(int j = 0 ; j < FAT_ENTRY_PER_BLOCK ; j++){
            ((int*)(block + j*FAT_ENTRY_SIZE))[0] = FAT_ENTRY_PER_BLOCK * i + j + 1;
            if(i == blockCount - 1 && j == blockOffset - 1){
                ((int*)(block + j*FAT_ENTRY_SIZE))[0] = -1;
                break;
            }
        }
        write_block((void*)block, FAT_START + i);
    }
}

void initSuperblock(int dataCount){
    char block[BLOCKSIZE];
    ((int*)(block))[0] = dataCount; // number of data block
    ((int*)(block + 4))[0] = 0; // empty FAT entry
    ((int*)(block + 8))[0] = dataCount; // number of free blocks
    ((int*)(block + 12))[0] = 0; // file count at the beginning
    write_block((void*)block, SUPERBLOCK_START);
}

void initDirectoryStructure(){
    char block[BLOCKSIZE];
    for(int j = 0 ; j < DIR_ENTRY_PER_BLOCK ; j++){
        int startByte = j * DIR_ENTRY_SIZE;
        ((char*)(block + startByte + 72))[0] = 48;
    }
    for(int i = 0 ; i < DIR_COUNT ; i++){
        write_block((void*)block, DIR_START + i);

    }
}

void updateSuperblock(){
    char block[BLOCKSIZE];
    read_block((void*)block, SUPERBLOCK_START);
    ((int*)(block + 4))[0] = emptyFAT; // empty FAT entry
    ((int*)(block + 8))[0] = freeBlockCount; // empty free count
    ((int*)(block + 12))[0] = fileCount; // file count
    write_block((void*)block, SUPERBLOCK_START);
}

void getSuperblock(){
    char block[BLOCKSIZE];
    read_block((void*)block, SUPERBLOCK_START);
    DATA_COUNT = ((int*)(block))[0];
    emptyFAT = ((int*)(block + 4))[0];
    freeBlockCount = ((int*)(block + 8))[0];
    fileCount = ((int*)(block + 12))[0];
}

int checkOpened(int fd){
    if(openTable[fd].directoryBlock < 0){
        return -1;
    }
    return 0;
}

void updateDirectoryEntry(int fd){
    char block[BLOCKSIZE];
    read_block((void*)block, openTable[fd].directoryBlock + DIR_START);
    int startByte = openTable[fd].directoryBlockOffset * DIR_ENTRY_SIZE;
    ((int*)(block + startByte + 64))[0] = openTable[fd].directoryEntry.size;
    ((int*)(block + startByte + 68))[0] = openTable[fd].directoryEntry.fatIndex;
    ((char*)(block + startByte + 72))[0] = 49;
    printf("Used char is %c\n", ((char*)(block + startByte + 72))[0]);
    write_block((void*)block, openTable[fd].directoryBlock + DIR_START);
}

void clearOpenTable(){
    for(int i = 0 ; i < MAX_OPENED_FILE_PER_PROCESS ; i++){
        openTable[i].directoryBlock = -1;
        openedFileCount = 0;
    }
}

void printDisk(){
    char block[BLOCKSIZE];
    read_block((void*)block, 0);
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
        read_block((void*)block, i);
        for(int j = 0 ; j < DIR_ENTRY_PER_BLOCK ; j++){
            printf("offset: %d\n", j);
            int startByte = j * DIR_ENTRY_SIZE;
            char isUsed = ((char*)(block + startByte + 72))[0];
            printf("Is used: %c\n", isUsed);
            if(isUsed == 49){
                printf("\tDirectory entry %d:\n", (i - 1)* DIR_ENTRY_PER_BLOCK + j);
                printf("\t\tName: %s\n", ((char*)(block + startByte)));
                printf("\t\tSize: %d\n", ((int*)(block + startByte + 64))[0]);
                printf("\t\tFat index: %d\n", ((int*)(block + startByte + 68))[0]);
                curFatIndex = ((int*)(block + startByte + 68))[0];
                int blockCount = 0;
                int remainingByte = ((int*)(block + startByte + 64))[0];
                /*while(curFatIndex > -1){
                    printf("\n\t\t\tBlock %d:\n", curFatIndex);
                    blockCount++;
                    printf("\t\t\t\t");
                    getDataBlock((void*)block, curFatIndex);
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
                }*/
            }
        }
    }
}

void printFile(char* filename){
    char block[BLOCKSIZE];
    for(int i = DIR_START ; i < DIR_START + DIR_COUNT ; i++){
        read_block((void*)block, i);
        for(int j = 0 ; j < DIR_ENTRY_PER_BLOCK ; j++){
            //printf("offset: %d\n", j);
            int startByte = j * DIR_ENTRY_SIZE;
            char isUsed = ((char*)(block + startByte + 72))[0];
            char name[64];
            memcpy(name, ((char*)(block)), 64);
            if(isUsed == 49) printf("Is used: %c %s\n", isUsed, name);
            if(isUsed == 49 && strcmp(filename, name) == 0){
                printf("\tDirectory entry %d:\n", (i - 1)* DIR_ENTRY_PER_BLOCK + j);
                printf("\t\tName: %s\n", ((char*)(block + startByte)));
                printf("\t\tSize: %d\n", ((int*)(block + startByte + 64))[0]);
                printf("\t\tFat index: %d\n", ((int*)(block + startByte + 68))[0]);
                int curFatIndex = ((int*)(block + startByte + 68))[0];
                int blockCount = 0;
                int remainingByte = ((int*)(block + startByte + 64))[0];
                while(curFatIndex > -1){
                    printf("\n\t\t\tBlock %d:\n", curFatIndex);
                    blockCount++;
                    printf("\t\t\t\t");
                    getDataBlock((void*)block, curFatIndex);
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
}