


all: libvsimplefs.a create_format app

libvsimplefs.a: 	vsimplefs.c
	gcc -Wall -c vsimplefs.c
	ar -cvq libvsimplefs.a vsimplefs.o
	ranlib libvsimplefs.a

create_format: create_format.c
	gcc -Wall -o create_format  create_format.c   -L. -lvsimplefs

app: 	app.c
	gcc -Wall -o app app.c  -L. -lvsimplefs

clean: 
	rm -fr *.o *.a *~ a.out app  vdisk create_format
