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

const char* const IRCC_RESOURCES_0 = 
		"\x48\x65\x6C\x6C\x6F\x57\x6F\x72\x6C\x64";

const char* const IRCC_RESOURCES_1 = 
		"\x48\x65\x6C\x6C\x6F\x55\x6E\x64\x65\x72\x57\x6F\x72\x6C\x64";

/// ... key_value_size type

struct key_value_size IRCC_RESOURCES_[] = {
	{"/hello", IRCC_RESOURCES_0, 10},
	{"another_key", IRCC_RESOURCES_1, 15},
	{"/image", IRCC_RESOURCES_2, 38905},
	{NULL, NULL, 0}};

/// ... helper functions 
```

It can be used in program:
```
#include <iostream>
#include <string>

extern std::string ircc_string(const char *key);

int main()
{
    std::string resource = ircc_string("/hello");
    std::cout << hello << std::endl;
    return 0;
}
``` 

## List of access methods:
```
extern std::string ircc_string(const char *key);
extern std::vector<uint8_t> ircc_vector(const char *key);
extern std::pair<const char*, size_t> ircc_pair(const char *key);
extern "C" const char *ircc_c_get(const char *key, size_t *sizeptr);
```

## C style.
`ircc` can create c-style files with `c_only` key:
```
ircc resources.txt -o ircc_resources.gen.c --c_only 
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