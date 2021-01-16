# Secondary Memory Simulator

I collaborated with my dear colleague [Hakan](https://github.com/hakansivuk "Hakan's Github") for the last operating system concepts' project.

Secondary Memory simulator is a C application that provides a secondary memory management library for processes. This library implements a virtual file system using a file allocation of a virtual disk. The virtual disk is essentially a regular linux file of certain size, controlled by the library. The library is not thread-safe. 

## My Contributions

All the functionalities are made by [Hakan](https://github.com/hakansivuk "Hakan's Github"). I tested the library using white-box techniques and achieved a code coverage of 100%. I found and reported bugs to [Hakan](https://github.com/hakansivuk "Hakan's Github").

## How to Use

> Compile your application with libvsimplefs.a. Import "vsimplefs.h" to use the library. Here is the basic documentation:

* **int create_format_vdisk (char \*vdiskname, int m):** This function is used to create and format a virtual disk. The virtual disk will simply be a Linux file of certain size. The name of the Linux file is vdiskname. The parameter m is used to set the size of the file. Size is 2^m bytes. If success, 0 is returned. If error, -1 is returned.
* **int vsfs_mount (char \*vdiskname):** This function is used to mount the file system, i.e., to prepare the file system to be used. You must call this before accessing the virtual disk. If success, 0 is returned; if error, -1 is returned.
* **int vsfs_umount ():** This function is used to unmount the file system: flush the cached data to disk and close the virtual disk (Linux file) file descriptor. If success, 0 is returned, if error, -1 is returned.
* **int vsfs_create (char \*filename):** With this, an applicaton can create a new file with name filename. If success, 0 is returned. If error, -1 is returned.
* **int vsfs_open (char \*filename, int mode):** With this function an application can open a file. The name of the file to open is filename. The mode paramater specifies if the file is opened in read-only mode or in append-only mode. If error, -1 is returned else the unique file descriptor (fd) for the file is returned. Mode is either _MODE_READ_ or _MODE_APPEND_.
* **int vsfs_getsize (int fd):** With this an application learns the size of a file whose descriptor is fd. The file must be opened first. Returns the number of data bytes in the file. A file with no data in it (no content) has size 0. If error, returns -1.
* **int vsfs_close (int fd):** With this function an application can close a file whose descriptor is fd. The related open file table entry should be marked as free.
* **int vsfs_read (int fd, void \*buf, int n):** With this, an application can read data from a file. fd is the file descriptor. buf is pointing to a memory area for which space is allocated earlier with malloc. n is the number of bytes of data to read. Upon failure, -1 is returned. Otherwise, number of bytes sucessfully read is returned.
* **int vsfs_append (int fd, void \*buf, int n):** With this, an application can append new data to the file. The parameter fd is the file descriptor. The parameter buf is pointing to (i.e., is the address of) a dynamically allocated memory space holding the data. The parameter n is the size of the data to write (append) into the file. If error, -1 is returned. Otherwise, the number of bytes successfully appended is returned.
* **int vsfs_delete (char \*filename):** With this, an application can delete a file. The name of the file to be deleted is filename. If succesful, 0 is returned. In case of an error, -1 is returned.

> For further techical information and some constraints about the file system specifications or the project, please refer to the **project4.pdf** file.

## Example

	#include <stdio.h>
	#include <stdlib.h>
	#include <unistd.h>
	#include <string.h>
	#include "vsimplefs.h"

	int main(int argc, char **argv)
	{
		int ret;

		int fd1, fd2, fd;
		int i;
		char c;
		char buffer[1024];
		char buffer2[8] = {50, 50, 50, 50, 50, 50, 50, 50};
		int size;
		char vdiskname[200];

		printf( "started\n");

		if (argc != 2) 
		{
			printf( "usage: app <vdiskname>\n");
			exit(0);
		}

		strcpy( vdiskname, argv[1]);
		ret = vsfs_mount( vdiskname);

		if (ret != 0)
		{
			printf ("could not mount \n");
			exit (1);
		}

		printf( "creating files\n");
		vsfs_create( "file1.bin");
		vsfs_create( "file2.bin");
		vsfs_create( "file3.bin");

		fd1 = vsfs_open( "file1.bin", MODE_APPEND);
		fd2 = vsfs_open( "file2.bin", MODE_APPEND);
		for (i = 0; i < 10000; ++i) 
		{
			buffer[0] = (char) 65;
			vsfs_append( fd1, (void *) buffer, 1);
		}

		for (i = 0; i < 10000; ++i) 
		{
			buffer[0] = (char) 65;
			buffer[1] = (char) 66;
			buffer[2] = (char) 67;
			buffer[3] = (char) 68;
			vsfs_append( fd2, (void *) buffer, 4);
		}

		vsfs_close( fd1);
		vsfs_close( fd2);

		fd = vsfs_open( "file3.bin", MODE_APPEND);
		for (i = 0; i < 10000; ++i)
		{
			memcpy( buffer, buffer2, 8); // just to show memcpy
			vsfs_append( fd, (void *) buffer, 8);
		}

		vsfs_close( fd);
		fd = vsfs_open( "file3.bin", MODE_READ);
		size = vsfs_getsize( fd);
		for (i = 0; i < size; ++i)
		{
			vsfs_read( fd, (void *) buffer, 1);
			c = (char)buffer[ 0];
			c = c + 1;
		}

		vsfs_close( fd);
		ret = vsfs_umount();
	}

## Issues

When using vsfs_read, please use dynamically allocated pointers for reading. Static pointers holding memory on the program stack could cause strange side-effects.
