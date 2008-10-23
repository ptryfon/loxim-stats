#ifndef ENDIANNESS_H_
#define ENDIANNESS_H_

#include <stdint.h>

namespace protocol{

int64_t htonll(int64_t src);
uint64_t htonll(uint64_t src);

int64_t ntohll(int64_t src);
uint64_t ntohll(uint64_t src);

double ntohd(double src);
double htond(double src);
}

#endif /*ENDIANNESS_H_*/
