include make.defs

SUBDIRS	=	Backup \
		Lock \
		Log \
		QueryExecutor \
		QueryParser \
		Store \
		TransactionManager \
		Driver \
		SBQLCli \
		Server \
		Errors \
		Config

PROGRAM	=	main

all: subdirs $(PROGRAM)

subdirs:
	@for i in $(SUBDIRS); do \
		make -C $$i ; \
	done

#$(PROGRAM): $(MODULE)
#	$(CPP) $(CXXFLAGS) $@.cpp $< -o $@

%.o: %.cpp
	$(CPP) -c $(CXXFLAGS) $< -o $@

clean:
	rm -f $(PROGRAM)
