ircc resources.txt -o ircc_resources.gen1.cpp
ircc resources.txt -o ircc_resources.gen2.cpp --c_only 
g++ -o runtest1 main.cpp ircc_resources.gen1.cpp
g++ -o runtest2 main.cpp ircc_resources.gen2.cpp