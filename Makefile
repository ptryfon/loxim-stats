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
		Indexes \
		SBQLCli \
		Server 		

all: subdirs

mapper:
		make -C Mapper || exit 1; \
	
mapper-clean:
		make -C Mapper clean || exit 1; \

subdirs:
	@for i in $(SUBDIRS); do \
		make -C $$i || exit 1; \
	done

doc:
	doxygen Documentation/doxygen.conf

bin:
	cp Server/Server.conf .
	tar c Server/Listener SBQLCli/SBQLCli Backup/Backup Server.conf server.sh client.sh |gzip -c >loxim.tar.gz

clean:
	@find -name "*.d" | xargs rm -f
	@for i in $(SUBDIRS); do \
		make -C $$i clean || exit 1; \
	done
