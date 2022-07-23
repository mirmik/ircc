# Very simple c++ resource compiler

The program packages the target files into a key-value store that can be connected to a C++ program.

## Installation
```
cmake .
cmake --build . 
cmake --install .
```

## Example 

hello.txt: 
```
HelloWorld
```

foo.txt: 
```
HelloUnderWorld
```

resources.txt: 
```
/hello ./helloworld.txt
another_key ./foo.txt
```

If you have files `hello.txt`, `foo.txt`, `resources.txt` then the `ircc` command give you cpp file:
```
ircc resources.txt -o ircc_resources.gen.cpp
``` 
``` 
#include <map>
#include <string>

std::map<std::string, std::string> IRCC_RESOURCES = {
	{"/hello",
		"\x48\x65\x6C\x6C\x6F\x57\x6F\x72\x6C\x64\x0A"
	},
	{"another_key",
		"\x48\x65\x6C\x6C\x6F\x55\x6E\x64\x65\x72\x57\x6F\x72\x6C\x64\x0A"
	},
};
```

It can be used in program:
```
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
``` 

## CMake example:
It can be used with cmake with `add_custom_command`:

```
project(ircc)
cmake_minimum_required(VERSION 3.0)
set(CMAKE_BUILD_TYPE RelWithDebInfo)

set(SOURCES 
	main.cpp
    ircc_resources.gen.cpp)

add_executable(cmake_runtest ${SOURCES})

add_custom_command(OUTPUT ircc_resources.gen.cpp
    COMMAND ircc resources.txt ircc_resources.gen.cpp
)
```