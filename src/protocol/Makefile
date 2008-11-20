include ../make.defs

SUBDIRS	=	auth \
		layers \
		packages \
		pstreams \
		ptools \
		sockets \
		streams

all: subdirs

subdirs:
	@for i in $(SUBDIRS); do \
		make -C $$i || exit 1; \
	done

clean:
	@find -name "*.d" | xargs rm -f
	@for i in $(SUBDIRS); do \
		make -C $$i clean || exit 1; \
	done
