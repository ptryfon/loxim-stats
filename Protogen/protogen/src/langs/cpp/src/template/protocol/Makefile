include ../make.defs

SUBDIRS	=	auth \
	layers \
	base_packages \
	packages \
	enums \
	pstreams \
	ptools \
	sockets \
	streams 

all: subdirs build_tests

subdirs:
	@for i in $(SUBDIRS); do \
		make -C $$i || exit 1; \
	done

clean:
	@find -name "*.d" | xargs rm -f
	@for i in $(SUBDIRS); do \
		make -C $$i clean || exit 1; \
	done
	make -C tests clean

build_tests: subdirs
	make -C tests 
	