#ifndef SMARTPTR_H
#define SMARTPTR_H


/* 
 * As of time of writing, smart pointers are in std::tr1, when tr1
 * becomes a standard, an appropriate macro should appear here to
 * choose the appropriate namespace dependingly on the gcc version 
 */
#include <tr1/memory>
#include <tr1/functional>
#define _smptr std::tr1

#endif /* SMARTPTR_H */
