#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "vsimplefs.h"
void create(){
    int ret;
    
    printf ("started\n"); 
    
    ret  = create_format_vdisk ("example", 21); 
    if (ret != 0) {
	printf ("there was an error in creating the disk\n");
	exit(1); 
    }

    printf ("disk created and formatted.\n"); 
}

int main(int argc, char **argv)
{
    create();
    int ret;
    int fd1, fd2, fd; 
    int i;
    char c; 
    char buffer[1024];
    char buffer2[8] = {50, 50, 50, 50, 50, 50, 50, 50};
    int size;
    char vdiskname[200]; 

    printf ("started\n");

    /*if (argc != 2) {
	printf ("usage: app  <vdiskname>\n"); 
	exit(0); 
    }*/
    //strcpy (vdiskname, argv[1]); 
    strcpy (vdiskname, "example");
    ret = vsfs_mount (vdiskname); 
    if (ret != 0) {
	printf ("could not mount \n");
	exit (1); 
    }

    printf ("creating files\n"); 
    vsfs_create ("file1.bin");
    vsfs_create ("file2.bin");
    vsfs_create ("file3.bin");
    
    fd1 = vsfs_open ("file1.bin", MODE_APPEND);
    fd2 = vsfs_open ("file2.bin", MODE_APPEND); 
    printf("%d %d\n", fd1, fd2);

    /*for (i = 0; i < 10000; ++i) {
        buffer[0] =   (char) 65;  
        vsfs_append (fd1, (void *) buffer, 1);
    }
    //printf("Check1\n");
    for (i = 0; i < 10000; ++i) {
        buffer[0] = (char) 65;
        buffer[1] = (char) 66;
        buffer[2] = (char) 67;
        buffer[3] = (char) 68;
        //printf("Check2 %d\n", i);
        vsfs_append(fd2, (void *) buffer, 4);
        //printf("Check3\n");
    }*/

    
    vsfs_close(fd1); 
    vsfs_close(fd2); 

    fd = vsfs_open("file3.bin", MODE_APPEND);
    /*for (i = 0; i < 5; ++i) {
	memcpy (buffer, buffer2, 8); // just to show memcpy
	vsfs_append(fd, (void *) buffer, 8); 
        printf("We wrote:\n");
        for(int j = 0 ; j < 8 ; j++ ){
            printf("%s", ((char*)(buffer+j)));
        }
        printf("\n");
    }*/
    char* ptr = "Cevat sanirim oldu aahhahaha";
    printf("Len: %lu\n", strlen(ptr));
    vsfs_append(fd, (void*) ptr, 28);
    vsfs_close (fd); 

    fd = vsfs_open("file3.bin", MODE_READ);
    size = vsfs_getsize (fd);
    printf("Size of file3 is %d\n", size);
    //for (i = 0; i < size; ++i) {
	//vsfs_read (fd, (void *) buffer, 5);
    //for(int i = 0 ; i < 5 ; i++){
    //    printf("%s\n", ((char*)(buffer + i)));
    //}
	//c = (char) buffer[0];
	//c = c + 1;
    //}
    //vsfs_close (fd); 
    
    void* fileData = (void*) malloc(28);
    //fd = vsfs_open("file3.bin", MODE_READ);
    vsfs_read(fd, fileData, 28);
    printf("Data is:\n");
    //for(int i = 0 ; i < 28 ; i++){
        printf("%s\n", ((char*)(fileData)));
    //}
    vsfs_delete("file3.bin");
    vsfs_close(fd);
    vsfs_delete("file3.bin");
    printf("checkpoint\n");
    vsfs_open("file3.bin", MODE_APPEND);
    printf("checkpoint\n");
    ret = vsfs_umount();
    printf("checkpoint\n");
    //char a = (char)50;
    //printf("%s\n", &a);


    return 0;
}
