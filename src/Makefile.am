SUBDIRS=AdminParser ARIESLog Config Errors Indexes Lock Log Client Server QueryExecutor QueryParser Store SystemStats TransactionManager TypeCheck Util Protocol
bin_PROGRAMS = loxim_server lsbql
INCLUDES=-I$(top_srcdir)/src
noinst_LTLIBRARIES = libserver.la libclient.la 

libserver_la_SOURCES = 
libserver_la_LIBADD = $(top_srcdir)/src/Log/libLog.la $(top_srcdir)/src/Lock/libLock.la $(top_srcdir)/src/Config/libConfig.la $(top_srcdir)/src/Errors/libErrors.la $(top_srcdir)/src/Store/libStore.la $(top_srcdir)/src/ARIESLog/libARIESLog.la $(top_srcdir)/src/Indexes/libIndexes.la $(top_srcdir)/src/QueryParser/libQueryParser.la $(top_srcdir)/src/TransactionManager/libTransactionManager.la $(top_srcdir)/src/SystemStats/libSystemStats.la $(top_srcdir)/src/QueryExecutor/libQueryExecutor.la $(top_srcdir)/src/AdminParser/libAdminParser.la $(top_srcdir)/src/TypeCheck/libTypeCheck.la $(top_srcdir)/src/Server/libServer.la  $(top_srcdir)/src/Util/libUtil.la $(top_srcdir)/src/Protocol/Enums/libProtocolEnums.la $(top_srcdir)/src/Protocol/libProtocol.la $(top_srcdir)/src/Protocol/Streams/libProtocolStreams.la $(top_srcdir)/src/Protocol/Packages/Data/libProtocolPackagesData.la $(top_srcdir)/src/Protocol/Packages/libProtocolPackages.la

libclient_la_SOURCES = 
libclient_la_LIBADD =   $(top_srcdir)/src/Errors/libErrors.la  $(top_srcdir)/src/Client/libClient.la $(top_srcdir)/src/Util/libUtil.la $(top_srcdir)/src/Protocol/Enums/libProtocolEnums.la $(top_srcdir)/src/Protocol/libProtocol.la $(top_srcdir)/src/Protocol/Streams/libProtocolStreams.la $(top_srcdir)/src/Protocol/Packages/Data/libProtocolPackagesData.la $(top_srcdir)/src/Protocol/Packages/libProtocolPackages.la


loxim_server_SOURCES = 
nodist_EXTRA_loxim_server_SOURCES = dummy.cpp
loxim_server_LDADD = libserver.la

lsbql_SOURCES = 
nodist_EXTRA_lsbql_SOURCES = dummy.cpp
lsbql_LDADD = libclient.la

