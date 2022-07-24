#ifndef IRCC_H_
#define IRCC_H_

#ifdef __cplusplus
#include <stdlib.h>
#include <string>
#include <vector>
extern std::string ircc_string(const char *key);
extern std::vector<uint8_t> ircc_vector(const char *key);
extern std::pair<const char*, size_t> ircc_pair(const char *key);
extern "C" const char *ircc_c_get(const char *key, size_t *sizeptr);
#else
#include <stdlib.h>
extern const char *ircc_c_get(const char *key, size_t *sizeptr);
#endif

#endif