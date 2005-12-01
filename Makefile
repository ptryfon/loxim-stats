include make.defs

SUBDIRS	=	Backup \
		QueryExecutor \
		QueryParser \
		Store \
		TransactionManager \
		Errors \
		Config \
		TCPProto \
		Driver \
		Lock \
		Log \
		SBQLCli \
		Server 		

all: subdirs

subdirs:
	@for i in $(SUBDIRS); do \
		make -C $$i || exit 1; \
	done

clean:
	@for i in $(SUBDIRS); do \
		make -C $$i clean || exit 1; \
	done
