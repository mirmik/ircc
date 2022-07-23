#include <iostream>
#include <map>
#include <string>

extern "C" const char *ircc_c_string(const char *key, size_t *sz);

int main()
{
    std::string hello = ircc_c_string("another_key", NULL);
    std::cout << hello << std::endl;
    return 0;
}