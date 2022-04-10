# libvector

libvector is a C library providing support for heap-allocated, dynamically-sized arrays.

## Usage

If you are using CMake simply clone this repo and add the following to your CMakeLists.txt file:

```cmake
add_subdirectory(libvector)
target_link_libraries(YOUR_TARGET vector)
```

Otherwise you may simply copy [vector.h](./vector.h) and [vector.c](vector.c) over to your project.

## Building

### Library

To build the library build the vector target using CMake. You should find the library in the build directory. At least a C99 compatible compiler is required.

### Documentation

To build the documentation [Doxygen](https://www.doxygen.nl/index.html) and [Doxybook2](https://github.com/matusnovak/doxybook2) are needed. After installing them, build the documentation target. You will then find the documentation in the build directory in doc/man and doc/md.

## License

[MIT](./LICENSE.txt)
