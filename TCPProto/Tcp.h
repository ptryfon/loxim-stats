#ifndef _TCP_H_
#define _TCP_H_

namespace TCPProto {

int bufferReceive(char** buffer, int* receiveDataSize, int Sock);
int bufferSend(char* buf, int buf_size, int Sock);

} //namespace

#endif //_TCP_H_
