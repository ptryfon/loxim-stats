include make.defs

SUBDIRS	=	Backup \
		Log \
		QueryExecutor \
		QueryParser \
		Store \
		TransactionManager \
		Errors \
		Config \
		Driver \
		Server \
		SBQLCli
#		Lock \

all: subdirs

subdirs:
	@for i in $(SUBDIRS); do \
		make -C $$i || exit 1; \
	done

clean:
	@for i in $(SUBDIRS); do \
		make -C $$i clean || exit 1; \
	done
