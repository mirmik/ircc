#ifndef IRCC_H_
#define IRCC_H_

#include <stdlib.h>
#include <string>

extern "C" const char *ircc_c_string(const char *key, size_t *sizeptr);
std::string ircc_string(const char *key);

#endif