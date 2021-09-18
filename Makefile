

#CFLAGS=-O2 -Wall -g -lpthread   -lm

CFLAGS=  -lpthread   -lm
NAME=mbw
TARFILE=${NAME}.tar.gz

mbw: mbw.c json.c

clean:
	rm -f mbw
	rm -f ${NAME}.tar.gz

${TARFILE}: clean
	 tar cCzf .. ${NAME}.tar.gz --exclude-vcs ${NAME} || true

rpm: ${TARFILE}
	 rpmbuild -ta ${NAME}.tar.gz 
