INSTALL=install
prefix=/opt/diet
# Set the following to install to a different root
#DESTDIR=/tmp/fefix
# Use "make DEBUG=1" to compile a debug version.

LIBDIR=${prefix}/lib
BINDIR=${prefix}/bin
MAN1DIR=${prefix}/man/man1

MYARCH=$(shell uname -m | sed -e 's/i[4-9]86/i386/' -e 's/armv[3-6][lb]/arm/')

# This extra-ugly cruft is here so make will not run uname and sed each
# time it looks at $(OBJDIR).  This alone sped up running make when
# nothing has to be done from 1 sec to 0.12 sec on a 900 MHz Athlon.
# We don't use ARCH:=$(MYARCH) so we can detect unknown architectures.
ifeq ($(MYARCH),i386)
ARCH=i386
else
ifeq ($(MYARCH),mips)
ARCH=mips
else
ifeq ($(MYARCH),alpha)
ARCH=alpha
else
ifeq ($(MYARCH),ppc)
ARCH=ppc
else
ifeq ($(MYARCH),arm)
ARCH=arm
else
ifeq ($(MYARCH),sparc)
ARCH=sparc
else
ifeq ($(MYARCH),s390)
ARCH=s390
else
ifeq ($(MYARCH),mipsel)
ARCH=mipsel
else
ifeq ($(MYARCH),parisc)
ARCH=parisc
else
$(error unknown architecture, please fix Makefile)
endif
endif
endif
endif
endif
endif
endif
endif
endif

# ARCH=$(MYARCH)

OBJDIR=bin-$(ARCH)
ILIBDIR=$(LIBDIR)-$(ARCH)

HOME=$(shell pwd)

all: $(OBJDIR) $(OBJDIR)/start.o $(OBJDIR)/dyn_start.o $(OBJDIR)/dyn_stop.o \
	$(OBJDIR)/dietlibc.a $(OBJDIR)/liblatin1.a $(OBJDIR)/librpc.a $(OBJDIR)/libpthread.a \
	$(OBJDIR)/libcompat.a $(OBJDIR)/libm.a $(OBJDIR)/diet $(OBJDIR)/diet-i $(OBJDIR)/elftrunc

CFLAGS=-pipe -nostdinc
CROSS=

CC=gcc

VPATH=lib:libstdio:libugly:libcruft:libcrypt:libshell:liblatin1:libcompat:libdl:librpc:libregex:libm

SYSCALLOBJ=$(patsubst syscalls.s/%.S,$(OBJDIR)/%.o,$(wildcard syscalls.s/*.S))

LIBOBJ=$(patsubst lib/%.c,$(OBJDIR)/%.o,$(wildcard lib/*.c))
LIBUGLYOBJ=$(patsubst libugly/%.c,$(OBJDIR)/%.o,$(wildcard libugly/*.c))
LIBSTDIOOBJ=$(patsubst libstdio/%.c,$(OBJDIR)/%.o,$(wildcard libstdio/*.c))
LIBCRUFTOBJ=$(patsubst libcruft/%.c,$(OBJDIR)/%.o,$(wildcard libcruft/*.c))
LIBCRYPTOBJ=$(patsubst libcrypt/%.c,$(OBJDIR)/%.o,$(wildcard libcrypt/*.c))
LIBSHELLOBJ=$(patsubst libshell/%.c,$(OBJDIR)/%.o,$(wildcard libshell/*.c))
LIBCOMPATOBJ=$(patsubst libcompat/%.c,$(OBJDIR)/%.o,$(wildcard libcompat/*.c))
LIBMATH=$(patsubst libm/%.c,%.o,$(wildcard libm/*.c))

LIBRPCOBJ=$(patsubst librpc/%.c,$(OBJDIR)/%.o,$(wildcard librpc/*.c))
LIBREGEXOBJ=$(patsubst libregex/%.c,$(OBJDIR)/%.o,$(wildcard libregex/*.c))

LIBDLOBJ=$(patsubst libdl/%.c,$(OBJDIR)/%.o,$(wildcard libdl/*.c)) $(OBJDIR)/_dl_jump.o

LIBPTHREAD_OBJS=$(patsubst libpthread/%.c,$(OBJDIR)/%.o,$(shell ./threadsafe.sh)) $(OBJDIR)/__testandset.o

include $(ARCH)/Makefile.add

LIBMATHOBJ=$(patsubst %,$(OBJDIR)/%,$(LIBMATH))

ifeq ($(CFLAGS),-pipe -nostdinc)
CFLAGS+=-O -fomit-frame-pointer
endif

ifneq ($(DEBUG),)
CFLAGS = -g
COMMENT = :
endif
CFLAGS += -Wall -W -Wchar-subscripts -Wmissing-prototypes -Wmissing-declarations -Wno-switch -Wredundant-decls

PWD=$(shell pwd)

.SUFFIXES:
.SUFFIXES: .S .c

$(OBJDIR):
	mkdir $@

% :: %,v

$(OBJDIR)/%.o: %.S
	$(CROSS)$(CC) -I. -Iinclude $(CFLAGS) -c $< -o $@

$(OBJDIR)/pthread_%.o: libpthread/pthread_%.c
	$(CROSS)$(CC) -I. -Iinclude $(CFLAGS) -c $< -o $@
	$(COMMENT) -$(CROSS)strip -x -R .comment -R .note $@

$(OBJDIR)/%.o: %.c
	$(CROSS)$(CC) -I. -Iinclude $(CFLAGS) -c $< -o $@
	$(COMMENT) -$(CROSS)strip -x -R .comment -R .note $@

DIETLIBC_OBJ = $(OBJDIR)/unified.o \
$(SYSCALLOBJ) $(LIBOBJ) $(LIBSTDIOOBJ) $(LIBUGLYOBJ) \
$(LIBCRUFTOBJ) $(LIBCRYPTOBJ) $(LIBSHELLOBJ) $(LIBREGEXOBJ) \
$(OBJDIR)/__longjmp.o $(OBJDIR)/setjmp.o \
$(OBJDIR)/mmap.o $(OBJDIR)/clone.o

$(OBJDIR)/dietlibc.a: $(DIETLIBC_OBJ) $(OBJDIR)/start.o
	$(CROSS)ar cru $@ $(DIETLIBC_OBJ)

$(OBJDIR)/librpc.a: $(LIBRPCOBJ)
	$(CROSS)ar cru $@ $(LIBRPCOBJ)

LIBLATIN1_OBJS=$(patsubst liblatin1/%.c,$(OBJDIR)/%.o,$(wildcard liblatin1/*.c))
$(OBJDIR)/liblatin1.a: $(LIBLATIN1_OBJS)
	$(CROSS)ar cru $@ $^

$(OBJDIR)/libpthread.a: $(LIBPTHREAD_OBJS) dietfeatures.h
	$(CROSS)ar cru $@ $(LIBPTHREAD_OBJS)

$(OBJDIR)/libcompat.a: $(LIBCOMPATOBJ)
	$(CROSS)ar cru $@ $(LIBCOMPATOBJ)

$(OBJDIR)/libm.a: $(LIBMATHOBJ)
	$(CROSS)ar cru $@ $(LIBMATHOBJ)

$(OBJDIR)/libdietc.so: $(OBJDIR)/dietlibc.a
	$(CROSS)ld -whole-archive -shared -o $@ $^

dyn: dynlinker/diet-linux.so dyn_lib

# added dynamic linker
$(OBJDIR)/libdl.a: $(LIBDLOBJ)
	$(CROSS)ar cru $@ $(LIBDLOBJ)

dynlinker/diet-linux.so: $(OBJDIR)/libdl.a
	make -C dynlinker

# added real dynamic dietlibc.so
PICODIR = pic-$(ARCH)

$(PICODIR):
	mkdir $@

dyn_lib: $(PICODIR) $(PICODIR)/libdietc.so $(PICODIR)/dstart.o \
	$(PICODIR)/dyn_so_start.o $(PICODIR)/dyn_start.o $(PICODIR)/dyn_stop.o \
	$(PICODIR)/libpthread.so $(PICODIR)/libdl.so $(PICODIR)/libcompat.so \
	$(PICODIR)/diet-dyn $(PICODIR)/diet-dyn-i

$(PICODIR)/%.o: %.S
	$(CROSS)$(CC) -I. -Iinclude $(CFLAGS) -fPIC -D__DYN_LIB -c $< -o $@

$(PICODIR)/pthread_%.o: libpthread/pthread_%.c
	$(CROSS)$(CC) -I. -Iinclude $(CFLAGS) -fPIC -D__DYN_LIB -c $< -o $@
	$(COMMENT) $(CROSS)strip -x -R .comment -R .note $@

$(PICODIR)/%.o: %.c
	$(CROSS)$(CC) -I. -Iinclude $(CFLAGS) -fPIC -D__DYN_LIB -c $< -o $@
	$(COMMENT) $(CROSS)strip -x -R .comment -R .note $@

$(PICODIR)/dstart.o: start.S
	$(CROSS)$(CC) -I. -Iinclude $(CFLAGS) -fPIC -D__DYN_LIB -c $< -o $@

$(PICODIR)/dyn_so_start.o: dyn_start.c
	$(CROSS)$(CC) -I. -Iinclude $(CFLAGS) -fPIC -D__DYN_LIB -D__DYN_LIB_SHARED -c $< -o $@
	$(COMMENT) $(CROSS)strip -x -R .comment -R .note $@

DYN_LIBC_PIC = $(LIBOBJ) $(LIBSTDIOOBJ) $(LIBUGLYOBJ) \
$(LIBCRUFTOBJ) $(LIBCRYPTOBJ) $(LIBSHELLOBJ) $(LIBREGEXOBJ)

DYN_LIBC_OBJ = $(PICODIR)/dyn_syscalls.o $(PICODIR)/errlist.o \
	$(patsubst $(OBJDIR)/%.o,$(PICODIR)/%.o,$(DYN_LIBC_PIC))

DYN_PTHREAD_OBJS = $(patsubst $(OBJDIR)/%.o,$(PICODIR)/%.o,$(LIBPTHREAD_OBJS))

DYN_LIBDL_OBJS = $(patsubst $(OBJDIR)/%.o,$(PICODIR)/%.o,$(LIBDLOBJ))

DYN_LIBCOMPAT_OBJS = $(patsubst $(OBJDIR)/%.o,$(PICODIR)/%.o,$(LIBCOMPATOBJ))

$(PICODIR)/libdietc.so: $(PICODIR) $(DYN_LIBC_OBJ)
	$(CROSS)$(CC) -nostdlib -shared -o $@ $(CFLAGS) -fPIC $(DYN_LIBC_OBJ) -Wl,-soname=libc.so

$(PICODIR)/libpthread.so: $(DYN_PTHREAD_OBJS) dietfeatures.h
	$(CROSS)$(CC) -nostdlib -shared -o $@ $(CFLAGS) -fPIC $(DYN_PTHREAD_OBJS) -L$(PICODIR) -ldietc -Wl,-soname=libpthread.so

$(PICODIR)/libdl.so: $(DYN_LIBDL_OBJS) dietfeatures.h
	$(CROSS)$(CC) -nostdlib -shared -o $@ $(CFLAGS) -fPIC $(DYN_LIBDL_OBJS) -L$(PICODIR) -ldietc -Wl,-soname=libdl.so

$(PICODIR)/libcompat.so: $(DYN_LIBCOMPAT_OBJS) dietfeatures.h
	$(CROSS)$(CC) -nostdlib -shared -o $@ $(CFLAGS) -fPIC $(DYN_LIBCOMPAT_OBJS) -L$(PICODIR) -ldietc -Wl,-soname=libcompat.so



$(SYSCALLOBJ): syscalls.h

$(OBJDIR)/elftrunc: $(OBJDIR)/diet contrib/elftrunc.c
	bin-$(MYARCH)/diet $(CROSS)$(CC) $(CFLAGS) -o $@ contrib/elftrunc.c

VERSION=dietlibc-$(shell head -1 CHANGES|sed 's/://')
CURNAME=$(notdir $(shell pwd))

$(OBJDIR)/diet: $(OBJDIR)/start.o $(OBJDIR)/dyn_start.o diet.c $(OBJDIR)/dietlibc.a $(OBJDIR)/dyn_stop.o
	$(CROSS)$(CC) -Iinclude $(CFLAGS) -nostdlib -o $@ $^ -DDIETHOME=\"$(HOME)\" -DVERSION=\"$(VERSION)\"
	$(CROSS)strip -R .comment -R .note $@

$(OBJDIR)/diet-i: $(OBJDIR)/start.o $(OBJDIR)/dyn_start.o diet.c $(OBJDIR)/dietlibc.a $(OBJDIR)/dyn_stop.o
	$(CROSS)$(CC) -Iinclude $(CFLAGS) -nostdlib -o $@ $^ -DDIETHOME=\"$(prefix)\" -DVERSION=\"$(VERSION)\" -DINSTALLVERSION
	$(CROSS)strip -R .comment -R .note $@

$(PICODIR)/diet-gen: $(OBJDIR)/start.o $(OBJDIR)/dyn_start.o diet.c $(OBJDIR)/dietlibc.a $(OBJDIR)/dyn_stop.o
	$(CROSS)$(CC) -Iinclude $(CFLAGS) -nostdlib -o $@ $^ -DDIETHOME=\"$(HOME)\" -D__DYN_LIB -DVERSION=\"$(VERSION)\"
	$(CROSS)strip -R .command -R .note $@

$(PICODIR)/diet-dyn: $(PICODIR)/start.o $(PICODIR)/dyn_start.o diet.c
	$(CROSS)$(CC) -Iinclude $(CFLAGS) -nostdlib -o $@ $^ -DDIETHOME=\"$(HOME)\" -D__DYN_LIB -DVERSION=\"$(VERSION)\" -L$(PICODIR) -ldietc -lgcc $(PICODIR)/dyn_stop.o -Wl,-dynamic-linker=$(HOME)/dynlinker/diet-linux.so
	$(CROSS)strip -R .command -R .note $@

$(PICODIR)/diet-dyn-i: $(PICODIR)/start.o $(PICODIR)/dyn_start.o diet.c
	$(CROSS)$(CC) -Iinclude $(CFLAGS) -nostdlib -o $@ $^ -DDIETHOME=\"$(prefix)\" -D__DYN_LIB -DVERSION=\"$(VERSION)\" -L$(PICODIR) -ldietc -lgcc $(PICODIR)/dyn_stop.o -Wl,-dynamic-linker=$(ILIBDIR)/diet-linux.so -DINSTALLVERSION
	$(CROSS)strip -R .command -R .note $@

$(OBJDIR)/djb: $(OBJDIR)/compile $(OBJDIR)/load

$(OBJDIR)/compile:
	echo 'exec' $(CC) '$(CFLAGS) -I$(PWD)/$(OBJDIR)/include -c $${1+"$$@"}' > $@
	chmod 755 $@

$(OBJDIR)/load:
	echo 'main="$$1"; shift; exec' $(CC) '-nostdlib -o "$$main" $(PWD)/$(OBJDIR)/start.o "$$main".o $${1+"$$@"} $(PWD)/$(OBJDIR)/dietlibc.a -lgcc'  > $@
	chmod 755 $@

clean:
	rm -f *.o *.a t t1 compile load elftrunc exports mapfile libdietc.so
	rm -rf bin-* pic-*
	$(MAKE) -C examples clean
	$(MAKE) -C dynlinker clean
	$(MAKE) -C libdl clean

tar: clean rename
	cd ..; tar cvvf $(VERSION).tar.bz2 $(VERSION) --use=bzip2 --exclude CVS

rename:
	if test $(CURNAME) != $(VERSION); then cd .. && mv $(CURNAME) $(VERSION); fi

$(OBJDIR)/exports: $(OBJDIR)/dietlibc.a
	nm -g $(OBJDIR)/dietlibc.a | grep -w T | awk '{ print $$3 }' | sort -u > $(OBJDIR)/exports

.PHONY: t t1
t:
	$(CROSS)$(CC) -g $(CFLAGS) -fno-builtin -nostdlib -Iinclude -o t t.c $(OBJDIR)/start.o $(OBJDIR)/dyn_start.o $(OBJDIR)/dietlibc.a -lgcc $(OBJDIR)/dyn_stop.o -Wl,-Map,mapfile

t1:
	$(CROSS)$(CC) -g -o t1 t.c

install: $(OBJDIR)/start.o $(OBJDIR)/dietlibc.a $(OBJDIR)/librpc.a $(OBJDIR)/liblatin1.a $(OBJDIR)/libcompat.a $(OBJDIR)/elftrunc $(OBJDIR)/diet-i
	$(INSTALL) -d $(DESTDIR)$(ILIBDIR) $(DESTDIR)$(MAN1DIR) $(DESTDIR)$(BINDIR)
	$(INSTALL) $(OBJDIR)/start.o $(DESTDIR)$(ILIBDIR)/start.o
	$(INSTALL) -m 644 $(OBJDIR)/libm.a $(OBJDIR)/libpthread.a $(OBJDIR)/librpc.a $(OBJDIR)/liblatin1.a \
$(OBJDIR)/libcompat.a $(DESTDIR)$(ILIBDIR)
	$(INSTALL) -m 644 $(OBJDIR)/dietlibc.a $(DESTDIR)$(ILIBDIR)/libc.a
	$(INSTALL) $(OBJDIR)/diet-i $(DESTDIR)$(BINDIR)/diet
	-$(INSTALL) $(OBJDIR)/dyn_start.o $(OBJDIR)/dyn_stop.o $(DESTDIR)$(ILIBDIR)
	-$(INSTALL) $(PICODIR)/diet-dyn-i $(DESTDIR)$(BINDIR)/diet-dyn
	-$(INSTALL) $(PICODIR)/libdietc.so $(DESTDIR)$(ILIBDIR)/libc.so
	-$(INSTALL) $(PICODIR)/libpthread.so $(DESTDIR)$(ILIBDIR)/libpthread.so
	-$(INSTALL) $(PICODIR)/libdl.so $(DESTDIR)$(ILIBDIR)/libdl.so
	-$(INSTALL) $(PICODIR)/libcompat.so $(DESTDIR)$(ILIBDIR)/libcompat.so
	-$(INSTALL) $(PICODIR)/dyn_start.o $(DESTDIR)$(ILIBDIR)/dyn_dstart.o
	-$(INSTALL) $(PICODIR)/dyn_stop.o  $(DESTDIR)$(ILIBDIR)/dyn_dstop.o
	-$(INSTALL) $(PICODIR)/dstart.o $(PICODIR)/dyn_so_start.o $(DESTDIR)$(ILIBDIR)
	-$(INSTALL) dynlinker/diet-linux.so $(DESTDIR)$(ILIBDIR)/diet-linux.so
	$(INSTALL) -m 644 diet.1 $(DESTDIR)$(MAN1DIR)/diet.1
	if test -f $(PICODIR)/libdietc.so -a ! -f $(DESTDIR)/etc/diet.ld.conf; then echo "$(ILIBDIR)" > $(DESTDIR)/etc/diet.ld.conf; fi
	for i in `find include -name \*.h`; do install -m 644 -D $$i $(DESTDIR)$(prefix)/$$i; done

.PHONY: sparc ppc mips arm alpha i386 parisc mipsel powerpc s390

arm sparc ppc alpha i386 mips parisc s390:
	$(MAKE) ARCH=$@ CROSS=$@-linux- all t

# Cross compile for little endian MIPS
mipsel:
	$(MAKE) ARCH=$@ CROSS=mips-linux- all t

mips-gnu:
	$(MAKE) ARCH=$@ CROSS=$@-linux-gnu- all t

# Some people named their cross compiler toolchain powerpc-linux-gcc
powerpc:
	$(MAKE) ARCH=ppc CROSS=powerpc-linux- all t

cross:
	$(MAKE) arm sparc ppc alpha i386 mips


# these depend on dietfeatures.h for large file backward compatibility
$(OBJDIR)/__fstat64.o $(OBJDIR)/__lstat64.o $(OBJDIR)/__stat64.o $(OBJDIR)/lseek64.o $(OBJDIR)/readdir64.o: dietfeatures.h

# these depend on dietfeatures.h for thread support
$(OBJDIR)/alloc.o $(OBJDIR)/perror.o $(OBJDIR)/logging.o $(OBJDIR)/unified.o $(OBJDIR)/clone.o $(OBJDIR)/set_errno.o: dietfeatures.h

# these depend on dietfeatures.h for linker warnings
$(OBJDIR)/assert_fail.o $(OBJDIR)/sprintf.o $(OBJDIR)/vsnprintf.o $(OBJDIR)/___div.o $(OBJDIR)/fflush.o $(OBJDIR)/setvbuf.o $(OBJDIR)/system.o $(OBJDIR)/sendfile.o $(OBJDIR)/setenv.o: dietfeatures.h

# these depend on dietfeatures.h for buffered stdio
fclose.o $(OBJDIR)/fdglue.o $(OBJDIR)/fflush.o $(OBJDIR)/fgetc.o $(OBJDIR)/fputc.o $(OBJDIR)/fread.o $(OBJDIR)/fseek.o $(OBJDIR)/printf.o $(OBJDIR)/setvbuf.o $(OBJDIR)/stderr.o $(OBJDIR)/stdin.o $(OBJDIR)/stdout.o $(OBJDIR)/fwrite.o $(OBJDIR)/puts.o: dietfeatures.h

# these depend on dietfeatures.h for fast string routines
strcasecmp.o $(OBJDIR)/strcat.o $(OBJDIR)/strchr.o $(OBJDIR)/strcmp.o $(OBJDIR)/strcpy.o $(OBJDIR)/strlen.o $(OBJDIR)/strncasecmp.o $(OBJDIR)/strncat.o $(OBJDIR)/strrchr.o: dietfeatures.h

# these depend on dietfeatures.h for /proc
$(OBJDIR)/ttyname.o $(OBJDIR)/sysconf_cpus.o: dietfeatures.h

# these depend on dietfeatures.h for WANT_TZFILE_PARSER
$(OBJDIR)/localtime_r.o: dietfeatures.h

# these depend on dietfeatures.h for WANT_BUGGY_GLIBC_STRSEP
$(OBJDIR)/strsep.o: dietfeatures.h

# these depend on dietfeatures.h for WANT_SMALL_STDIO_BUFS
$(LIBSTDIOOBJ): dietfeatures.h

# these depend on dietfeatures.h for WANT_FULL_RESOLV_CONF
$(OBJDIR)/dnscruft.o $(OBJDIR)/dnscruft2.o: dietfeatures.h

# these depend on dietfeatures.h for WANT_THREAD_SAFE and errno
$(LIBRPCOBJ) $(OBJDIR)/logging.o $(OBJDIR)/alloc.o $(OBJDIR)/cfsetospeed.o $(OBJDIR)/cfsetispeed.o \
$(OBJDIR)/execl.o $(OBJDIR)/execlp.o $(OBJDIR)/execv.o $(OBJDIR)/execvp.o $(OBJDIR)/isatty.o \
$(OBJDIR)/lockf.o $(OBJDIR)/perror.o $(OBJDIR)/remove.o $(OBJDIR)/set_errno.o $(OBJDIR)/sigemptyset.o \
$(OBJDIR)/tcsetattr.o $(OBJDIR)/dnscruft2.o $(OBJDIR)/dnscruft.o $(OBJDIR)/entlib.o \
$(OBJDIR)/gethostbyaddr.o $(OBJDIR)/gethostbyaddr_r.o $(OBJDIR)/gethostbyname.o \
$(OBJDIR)/gethostbyname_r.o $(OBJDIR)/gethostbyname2.o $(OBJDIR)/gethostbyname2_r.o \
$(OBJDIR)/inet_pton.o $(OBJDIR)/mkstemp.o $(OBJDIR)/mktemp.o $(OBJDIR)/tempnam.o \
$(OBJDIR)/glob.o $(OBJDIR)/realpath.o $(OBJDIR)/fdglue.o $(OBJDIR)/fdglue2.o \
$(OBJDIR)/getaddrinfo.o $(OBJDIR)/getnameinfo.o $(OBJDIR)/getprotoent.o \
$(OBJDIR)/getservent.o $(OBJDIR)/iconv.o $(OBJDIR)/iconv_open.o \
$(OBJDIR)/netent.o $(OBJDIR)/system.o $(OBJDIR)/stdin.o $(OBJDIR)/stdout.o \
$(OBJDIR)/stderr.o: dietfeatures.h

# these depend on dietfeatures.h for WANT_CRYPT_MD5
$(OBJDIR)/crypt.o: dietfeatures.h

# these depend on dietfeatures.h for WANT_FREAD_OPTIMIZATION
$(OBJDIR)/fread.o $(OBJDIR)/fwrite.o: dietfeatures.h

# these depend on dietfeatures.h for WANT_DYNAMIC
$(OBJDIR)/start.o $(OBJDIR)/dyn_start.o $(OBJDIR)/dyn_stop.o: dietfeatures.h

$(OBJDIR)/unified.o: dietuglyweaks.h

$(OBJDIR)/adjtimex.o: include/sys/timex.h

$(OBJDIR)/fgetc_unlocked.o $(OBJDIR)/fread.o $(OBJDIR)/ungetc.o: dietstdio.h

# these depend on dietfeatures.h for WANT_LINKER_WARNINGS
$(OBJDIR)/setlinebuf.o $(OBJDIR)/bzero.o $(OBJDIR)/setegid.o \
$(OBJDIR)/seteuid.o: dietfeatures.h

# CFLAGS+=-W -Wshadow -Wid-clash-31 -Wpointer-arith -Wcast-align -Wstrict-prototypes -Wwrite-strings
