# xban - a Xinerama mouse monitor wrapper
# See LICENSE file for copyright and license details.

include config.mk

SRC = xban.c
OBJ = ${SRC:.c=.o}

all: options xban

options:
	@echo xban build options:
	@echo "CFLAGS   = ${CFLAGS}"
	@echo "LDFLAGS  = ${LDFLAGS}"
	@echo "CC       = ${CC}"

.c.o:
	${CC} -c ${CFLAGS} $<

xban: ${OBJ}
	${CC} -o $@ ${OBJ} ${LDFLAGS}

clean:
	rm -f xban ${OBJ} xban-${VERSION}.tar.gz

dist: clean
	mkdir -p xban-${VERSION}
	cp -R LICENSE Makefile config.mk\
		xban.1 ${SRC} xban-${VERSION}
	tar -cf xban-${VERSION}.tar xban-${VERSION}
	gzip xban-${VERSION}.tar
	rm -rf xban-${VERSION}

install: all
	mkdir -p ${DESTDIR}${PREFIX}/bin
	cp -f xban ${DESTDIR}${PREFIX}/bin
	chmod 755 ${DESTDIR}${PREFIX}/bin/xban
	mkdir -p ${DESTDIR}${MANPREFIX}/man1
	sed "s/VERSION/${VERSION}/g" < xban.1 > ${DESTDIR}${MANPREFIX}/man1/xban.1
	chmod 644 ${DESTDIR}${MANPREFIX}/man1/xban.1

uninstall:
	rm -f ${DESTDIR}${PREFIX}/bin/xban\
		${DESTDIR}${MANPREFIX}/man1/xban.1

.PHONY: all options clean dist install uninstall
