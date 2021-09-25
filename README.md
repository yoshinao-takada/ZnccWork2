# What Is This Project
* Objective: Perfromance comparison of ZNCC block matching implemented by C (without special technique) and ISPC.
* Target platform : Intel Clear Linux, ISPC 1.16.1, gcc 11.2.1

# Directory Structure
* [ZnccWork2](.)
  - [PerfTest](./PerfTest) : includes main() and helper functions for performance test
  - [UnitTest](./UnitTest) : Unit tests for low level components in ZnccC and ZnccISPC
  - [ZnccC](./ZnccC) : C11 implementation
  - [ZnccISPC](./ZnccISPC) : ISPC implementation
  - [makefile](./makefile)
* Tests creating directories
  - [WorkData](./WorkData) : Test data, intermediate work data, visualized 2-D map, etc.

# Dependencies
* OpenCV 4: core.hpp, img_proc.hpp, img_codes.hpp, posix standard APIs
* ISPC is assumed to be installed at a location in PATH variable. The code is tested with ver. 1.16.1
* C compiler is clang ver. 11.1.0.

# Subsidiary documents
* [Image.md](docs/Image.md) : Details of [C struct ImageC_t](ZnccC/ImageC.h)
and [ISPC struct Image](ZnccISPC/Image.isph)
* [ZnccHalf.md](docs/ZnccHalf.md) : Details of [C struct ZnccHalfC_t](ZnccC/ZnccHalfC.h)
and [ISPC struct ZnccHalf](ZnccISPC/ZnccHalf.isph)
* [Summed-area-table.md](docs/Summed-area-table.md) : Details usage of summed area table

