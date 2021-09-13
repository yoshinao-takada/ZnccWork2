# What Is This Project
* Objective: Perfromance comparison of ZNCC block matching implemented by C (no special technique) and ISPC.
* Target platform : Intel Clear Linux, ISPC 1.16.1, gcc 11.2.1

# Directory Structure
ZnccWork2
  |
  \__ PerfTest : includes main() and helper methods for performance test
  |
  \__ UnitTest : Unit tests for low level components in ZnccC and ZnccISPC
  |
  \__ ZnccC : C11 implementation
  |
  \__ ZnccISPC : ISPC implementation
  |
  \__ makefile : Invoke like "make release", "make debug", "make release_run", "make debug_run"
  |
  \__ README.md : This file

  Tests creating directories
  \__ WorkData : Test data, intermediate work data, visualized 2-D map, etc.

# Dependencies
OpenCV 4: core.hpp, img_proc.hpp, img_codes.hpp, posix standard APIs
ISPC is assumed to be installed at a location in PATH variable.

