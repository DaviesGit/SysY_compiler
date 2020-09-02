cmake_minimum_required (VERSION 2.6)

# SET (CMAKE_C_COMPILER        "/g/sb/llvm/llvm-project-llvmorg-10.0.1/bin/clang")
# SET (CMAKE_CXX_COMPILER      "/g/sb/llvm/llvm-project-llvmorg-10.0.1/bin/clang++")
# SET (CMAKE_AR                "/g/sb/llvm/llvm-project-llvmorg-10.0.1/bin/llvm-ar")
# SET (CMAKE_LINKER            "/g/sb/llvm/llvm-project-llvmorg-10.0.1/bin/llvm-ld")
# SET (CMAKE_NM                "/g/sb/llvm/llvm-project-llvmorg-10.0.1/bin/llvm-nm")
# SET (CMAKE_OBJDUMP           "/g/sb/llvm/llvm-project-llvmorg-10.0.1/bin/llvm-objdump")
# SET (CMAKE_RANLIB            "/g/sb/llvm/llvm-project-llvmorg-10.0.1/bin/llvm-ranlib")

# set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)
# set(CMAKE_CXX_EXTENSIONS OFF)

project (compiler)

file(GLOB_RECURSE src_cpp RELATIVE ${CMAKE_SOURCE_DIR}
    "src/*.cpp"
)
file(GLOB_RECURSE src_c RELATIVE ${CMAKE_SOURCE_DIR}
    "src/*.c"
)

message(STATUS ${src})

add_executable(
	compiler 
	${src_cpp}
	${src_c}
)

set_property(TARGET compiler PROPERTY C_STANDARD 11)
# set_property(TARGET compiler PROPERTY CXX_STANDARD 17)
ADD_DEFINITIONS("-std=c++17")
# ADD_DEFINITIONS("-O2")
ADD_DEFINITIONS("-O0")

target_link_libraries(compiler m)

