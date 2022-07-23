#include <iostream>
#include <map>
#include <string>

extern std::map<std::string, std::string> IRCC_RESOURCES;

int main()
{
    std::string hello = IRCC_RESOURCES["/hello"];
    std::cout << hello << std::endl;
    return 0;
}