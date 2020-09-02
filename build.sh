cd build
rm CMake* cmake_install.cmake Makefile compiler -r;
cmake ../ && make VERBOSE=1