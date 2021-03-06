CC=clang
INCFLAGS=-I../ZnccISPC -I..
DEBUG_CC_FLAGS = -fPIE -c -g -D_DEBUG $(INCFLAGS) -std=c++17
RELEASE_CC_FLAGS = -fPIE -c -O3 -DNDEBUG $(INCFLAGS) -std=c++17
DEBUG_TARGET_DIR = ../debug
RELEASE_TARGET_DIR = ../release
DEBUG_TARGET = $(DEBUG_TARGET_DIR)/PerfTest.exe
RELEASE_TARGET = $(RELEASE_TARGET_DIR)/PerfTest.exe
DEBUG_TARGET_LIB = -L$(DEBUG_TARGET_DIR) -lUtil -lZnccC -lZnccISPC
RELEASE_TARGET_LIB = -L$(RELEASE_TARGET_DIR) -lUtil -lZnccC -lZnccISPC
ISPC_LIB = -L/home/yoshinao/.local/intel/ispc-v1.16.1-linux/lib64 -lispcrt_static -lomp
OPENCV_LIB = `pkg-config --libs opencv`
DEBUG_LIB = $(DEBUG_TARGET_LIB) $(OPENCV_LIB) $(ISPC_LIB) -lpthread -lm -lstdc++
RELEASE_LIB = $(RELEASE_TARGET_LIB) $(OPENCV_LIB) $(ISPC_LIB) -lpthread -lm -lstdc++
DEBUG_OBJECT_DIR = $(DEBUG_TARGET_DIR)/o
RELEASE_OBJECT_DIR = $(RELEASE_TARGET_DIR)/o
DEBUG_OBJECTS = $(DEBUG_OBJECT_DIR)/PerfTestMain.o $(DEBUG_OBJECT_DIR)/PerfTestC.o \
	$(DEBUG_OBJECT_DIR)/PerfTestISPC.o $(DEBUG_OBJECT_DIR)/PerfTestCWithoutCostMap.o
RELEASE_OBJECTS = $(RELEASE_OBJECT_DIR)/PerfTestMain.o $(RELEASE_OBJECT_DIR)/PerfTestC.o \
	$(RELEASE_OBJECT_DIR)/PerfTestISPC.o $(RELEASE_OBJECT_DIR)/PerfTestCWithoutCostMap.o

# -------------------- debug -----------------------------
.PHONY	:	debug
debug	:	$(DEBUG_TARGET)
$(DEBUG_TARGET)	:	$(DEBUG_OBJECTS) $(DEBUG_TARGET_DIR)/libUtil.a $(DEBUG_TARGET_DIR)/libZnccISPC.a $(DEBUG_TARGET_DIR)/libZnccC.a
	$(CC) -o $@ $(DEBUG_OBJECTS) $(DEBUG_LIB)

$(DEBUG_OBJECT_DIR)/PerfTestMain.o	:	PerfTestMain.cpp
	$(CC) $(DEBUG_CC_FLAGS) -o $@ $<

$(DEBUG_OBJECT_DIR)/PerfTestC.o	:	PerfTestC.cpp
	$(CC) $(DEBUG_CC_FLAGS) -o $@ $<

$(DEBUG_OBJECT_DIR)/PerfTestISPC.o	:	PerfTestISPC.cpp
	$(CC) $(DEBUG_CC_FLAGS) -o $@ $<

$(DEBUG_OBJECT_DIR)/PerfTestCWithoutCostMap.o	:	PerfTestCWithoutCostMap.cpp
	$(CC) $(DEBUG_CC_FLAGS) -o $@ $<

.PHONY	:	clean_debug
clean_debug	:
	rm -f $(DEBUG_TARGET)
	rm -f $(DEBUG_OBJECTS)

# -------------------- release -----------------------------
.PHONY	:	release
release	:	$(RELEASE_TARGET)
$(RELEASE_TARGET)	:	$(RELEASE_OBJECTS) $(RELEASE_TARGET_DIR)/libUtil.a $(RELEASE_TARGET_DIR)/libZnccISPC.a $(RELEASE_TARGET_DIR)/libZnccC.a
	$(CC) -o $@ $(RELEASE_OBJECTS) $(RELEASE_LIB)

$(RELEASE_OBJECT_DIR)/PerfTestMain.o	:	PerfTestMain.cpp
	$(CC) $(RELEASE_CC_FLAGS) -o $@ $<

$(RELEASE_OBJECT_DIR)/PerfTestC.o	:	PerfTestC.cpp
	$(CC) $(RELEASE_CC_FLAGS) -o $@ $<

$(RELEASE_OBJECT_DIR)/PerfTestISPC.o	:	PerfTestISPC.cpp
	$(CC) $(RELEASE_CC_FLAGS) -o $@ $<

$(RELEASE_OBJECT_DIR)/PerfTestCWithoutCostMap.o	:	PerfTestCWithoutCostMap.cpp
	$(CC) $(RELEASE_CC_FLAGS) -o $@ $<

.PHONY	:	clean_release
clean_release	:
	rm -f $(RELEASE_TARGET)
	rm -f $(RELEASE_OBJECTS)
