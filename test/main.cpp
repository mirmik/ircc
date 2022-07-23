#include <iostream>
#include <map>
#include <string>

// extern std::map<std::string, std::string> IRCC_RESOURCES;
extern std::string ircc_string(const char *key);

int main()
{
    std::string hello = ircc_string("another_key");
    std::cout << hello << std::endl;
    return 0;
}