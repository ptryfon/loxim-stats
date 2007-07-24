/*
 * Dolaczanie hash_set i hash_map zalezne od wersji gcc.
 * Uwaga oprocz dolaczania uzyta zostaje tez odpowiednia przestrzen nazw,
 * tez zalezna od wersji gcc.
 * (Testowane tylko na wersji: c++ (GCC) 4.0.2)
 */

#ifndef __HASHMACRO_H__
#define __HASHMACRO_H__


#if __GNUC__ < 3 && __GNUC__ >= 2 && __GNUC_MINOR__ >= 95
# include <hash_set>
# include <hash_map>
# define gnu_namespace std
#elif __GNUC__ >= 3
# include <ext/hash_set>
# include <ext/hash_map>
# if __GNUC_MINOR__ == 0 and __GNUC__ == 3
# define gnu_namespace std
# else
# define gnu_namespace __gnu_cxx
# endif
#else
# include <hash_set.h>
# include <functional.h>
# define gnu_namespace std
#endif
using namespace gnu_namespace;


#endif /*__HASHMACRO_H__*/
