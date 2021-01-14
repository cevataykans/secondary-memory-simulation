#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "vsimplefs.h"

int main(int argc, char **argv)
{
    int ret;
    char vdiskname[200];
    int m;

    if (argc != 3)
    {
        exit(1);
    }

    strcpy(vdiskname, argv[1]);
    m = atoi(argv[2]);

    ret = create_format_vdisk(vdiskname, m);
    if (ret != 0)
    {
        exit(-1);
    }
}
