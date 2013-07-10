# makefile for open source (LGPL) lastest
#
#COPTS    = -g -Wall -Wno-deprecated -DDEBUG 
COPTS     = -O3 -Wall -Wno-deprecated -DNDEBUG 
#COMPILER  = CC
COMPILER  = g++
LINKER  = g++
#BITS     = -64

#LIBS     = -L/usr/lib64
#LIBS     = -L/usr/lib32
#INCLUDE  = -I/usr/include

LASLIBS     = -L../lasread/lib -L../lascheck/lib 
LASINCLUDE  = -I../lasread/inc -I../lascheck/inc 

all: lastest

lastest: lastest.o
	${LINKER} ${BITS} ${COPTS} lastest.o -llasread -llascheck -o $@ ${LIBS} ${LASLIBS} $(INCLUDE) $(LASINCLUDE)
	cp $@ ../bin

.cpp.o: 
	${COMPILER} ${BITS} -c ${COPTS} ${INCLUDE} $(LASINCLUDE) $< -o $@

.c.o: 
	${COMPILER} ${BITS} -c ${COPTS} ${INCLUDE} $(LASINCLUDE) $< -o $@

clean:
	rm -rf *.o
	rm -rf lastest

clobber:
	rm -rf *.o
	rm -rf lastest
	rm -rf *~
