
UNAME_ARCH:=$(shell uname -m)
CC=gcc
CFLAGS=

ifeq ($(UNAME_ARCH), x86_64)
ARCH:=x86
else ifeq ($(UNAME_ARCH), i686)
ARCH:=x86
else
ARCH:=$(UNAME_ARCH)
endif

PHONY := all clean arch-clean lib-clean
CFLAGS = -std=gnu99 -O2
CPPFLAGS = -Iarch/include -Ilib/include
LDFLAGS = -lrt

bench := nhm_fetch_access pingpong
lib-o :=
arch-o :=

all: $(bench)

include arch/$(ARCH)/Makefile lib/Makefile
include $(arch-o:.o=.d)
include $(lib-o:.o=.d)
include $(addsuffix .d, $(bench))

%.d: %.c
	@set -e; rm -f $@; \
	$(CC) -M $(CPPFLAGS) $< > $@.$$$$; \
	sed 's,\($(notdir $*)\)\.o[ :]*,$(dir $*)\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$


%: %.o $(lib-o) $(arch-o)
	$(CC) $(LDFLAGS) $^ $(LOADLIBES) $(LDLIBS) -o $@

clean: libclean archclean
	$(RM) $(bench) *.o

.PHONY: $(PHONY)
