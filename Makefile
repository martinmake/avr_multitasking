LDIR :=$(HOME)/Documents/libs/AVR
LADIR:=$(LDIR)/arc
LIDIR:=$(LDIR)/inc
LMDIR:=$(LDIR)/man

######END#OF#CONFIGURATION#VARIABLES######

export LDIR LADIR LIDIR LMDIR

MAKE_PATHS:=$(dir $(shell find -mindepth 3 -maxdepth 3 -name Makefile))

.PHONY: all
all:
	+$(foreach PATH,$(MAKE_PATHS),make -e -C $(PATH);)

.PHONY: setup
setup:
	+$(foreach PATH,$(MAKE_PATHS),make -e -C $(PATH) setup;)

.PHONY: clean
clean:
	+$(foreach PATH,$(MAKE_PATHS),make -e -C $(PATH) clean;)
