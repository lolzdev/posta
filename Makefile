# posta - terminal based email client
# See LICENSE file for copyright and license details.

include config.mk

SRC = posta.c utils.c imap.c
OBJ = ${SRC:.c=.o}

all: options posta

options:
	@echo posta build options:
	@echo "CFLAGS   = ${CFLAGS}"
	@echo "LDFLAGS  = ${LDFLAGS}"
	@echo "CC       = ${CC}"

.c.o:
	${CC} -c ${CFLAGS} $<

${OBJ}: config.h config.mk

config.h:
	cp config.def.h $@

posta: ${OBJ}
	${CC} -o $@ ${OBJ} ${LDFLAGS}

clean:
	rm -f posta ${OBJ} posta-${VERSION}.tar.gz

dist: clean
	mkdir -p posta-${VERSION}
	cp -R LICENSE Makefile README config.def.h config.mk\
		posta.1 util.h ${SRC} posta.png transient.c posta-${VERSION}
	tar -cf posta-${VERSION}.tar posta-${VERSION}
	gzip posta-${VERSION}.tar
	rm -rf posta-${VERSION}

install: all
	mkdir -p ${DESTDIR}${PREFIX}/bin
	cp -f posta ${DESTDIR}${PREFIX}/bin
	chmod 755 ${DESTDIR}${PREFIX}/bin/posta
	mkdir -p ${DESTDIR}${MANPREFIX}/man1
	sed "s/VERSION/${VERSION}/g" < posta.1 > ${DESTDIR}${MANPREFIX}/man1/posta.1
	chmod 644 ${DESTDIR}${MANPREFIX}/man1/posta.1

uninstall:
	rm -f ${DESTDIR}${PREFIX}/bin/posta\
		${DESTDIR}${MANPREFIX}/man1/posta.1

.PHONY: all options clean dist install uninstall
