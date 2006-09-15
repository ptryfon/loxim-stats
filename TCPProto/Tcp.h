#ifndef _TCP_H_
#define _TCP_H_

#include "Package.h"

namespace TCPProto {

int bufferReceive(char** buffer, int* receiveDataSize, int Sock);
int bufferSend(const char* buf,  int buf_size, int Sock);
int packageReceive(Package** package, int sock, void* param = NULL, Package::packageType pType=Package::RESERVED);
int packageSend(Package* package, int sock);
int getConnection(const char* url, int port, int* newsock);

} //namespace

#endif //_TCP_H_
