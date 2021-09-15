CC=clang
INCFLAGS=-I../ZnccISPC -I..
DEBUG_CC_FLAGS = -fPIE -c -g -D_DEBUG $(INCFLAGS) -std=c++17
RELEASE_CC_FLAGS = -fPIE -c -O3 -DNDEBUG $(INCFLAGS) -std=c++17
DEBUG_TARGET_DIR = ../debug
RELEASE_TARGET_DIR = ../release
DEBUG_TARGET = $(DEBUG_TARGET_DIR)/UnitTest.exe
RELEASE_TARGET = $(RELEASE_TARGET_DIR)/UnitTest.exe
DEBUG_TARGET_LIB = -L$(DEBUG_TARGET_DIR) -lZnccC -lZnccISPC -lUtil
RELEASE_TARGET_LIB = -L$(RELEASE_TARGET_DIR) -lZnccC -lZnccISPC -lUtil
ISPC_LIB = -L/home/yoshinao/.local/intel/ispc-v1.16.1-linux/lib64 -lispcrt_static -lomp
OPENCV_LIB = `pkg-config --libs opencv`
DEBUG_LIB = $(DEBUG_TARGET_LIB) $(OPENCV_LIB) $(ISPC_LIB) -lpthread -lm -lstdc++
RELEASE_LIB = $(RELEASE_TARGET_LIB) $(OPENCV_LIB) $(ISPC_LIB) -lpthread -lm -lstdc++
DEBUG_OBJECT_DIR = $(DEBUG_TARGET_DIR)/o
RELEASE_OBJECT_DIR = $(RELEASE_TARGET_DIR)/o
DEBUG_OBJECTS = $(DEBUG_OBJECT_DIR)/TestMain.o $(DEBUG_OBJECT_DIR)/UtilImageSourceUT.o \
	$(DEBUG_OBJECT_DIR)/MeanScatteringUT.o $(DEBUG_OBJECT_DIR)/BasicsUT.o $(DEBUG_OBJECT_DIR)/CostMapUT.o \
	$(DEBUG_OBJECT_DIR)/BasicsCUT.o
RELEASE_OBJECTS = $(RELEASE_OBJECT_DIR)/TestMain.o $(RELEASE_OBJECT_DIR)/UtilImageSourceUT.o \
	$(RELEASE_OBJECT_DIR)/MeanScatteringUT.o $(RELEASE_OBJECT_DIR)/BasicsUT.o $(RELEASE_OBJECT_DIR)/CostMapUT.o \
	$(RELEASE_OBJECT_DIR)/BasicsCUT.o

# -------------------- debug -----------------------------
.PHONY	:	debug
debug	:	$(DEBUG_TARGET)
$(DEBUG_TARGET)	:	$(DEBUG_OBJECTS) $(DEBUG_TARGET_DIR)/libUtil.a $(DEBUG_TARGET_DIR)/libZnccISPC.a
	$(CC) -o $@ $(DEBUG_OBJECTS) $(DEBUG_LIB)

$(DEBUG_OBJECT_DIR)/TestMain.o	:	TestMain.cpp
	$(CC) $(DEBUG_CC_FLAGS) -o $@ $<

$(DEBUG_OBJECT_DIR)/UtilImageSourceUT.o	:	UtilImageSourceUT.cpp
	$(CC) $(DEBUG_CC_FLAGS) -o $@ $<

$(DEBUG_OBJECT_DIR)/MeanScatteringUT.o	:	MeanScatteringUT.cpp
	$(CC) $(DEBUG_CC_FLAGS) -o $@ $<

$(DEBUG_OBJECT_DIR)/BasicsUT.o	:	BasicsUT.cpp
	$(CC) $(DEBUG_CC_FLAGS) -o $@ $<

$(DEBUG_OBJECT_DIR)/CostMapUT.o	:	CostMapUT.cpp
	$(CC) $(DEBUG_CC_FLAGS) -o $@ $<

$(DEBUG_OBJECT_DIR)/BasicsCUT.o	:	BasicsCUT.cpp
	$(CC) $(DEBUG_CC_FLAGS) -o $@ $<

.PHONY	:	clean_debug
clean_debug	:
	rm -f $(DEBUG_TARGET)
	rm -f $(DEBUG_OBJECTS)

# -------------------- release -----------------------------
.PHONY	:	release
release	:	$(RELEASE_TARGET)
$(RELEASE_TARGET)	:	$(RELEASE_OBJECTS) $(RELEASE_TARGET_DIR)/libUtil.a $(RELEASE_TARGET_DIR)/libZnccISPC.a
	$(CC) -o $@ $(RELEASE_OBJECTS) $(RELEASE_LIB)

$(RELEASE_OBJECT_DIR)/TestMain.o	:	TestMain.cpp
	$(CC) $(RELEASE_CC_FLAGS) -o $@ $<

$(RELEASE_OBJECT_DIR)/UtilImageSourceUT.o	:	UtilImageSourceUT.cpp
	$(CC) $(RELEASE_CC_FLAGS) -o $@ $<

$(RELEASE_OBJECT_DIR)/MeanScatteringUT.o	:	MeanScatteringUT.cpp
	$(CC) $(RELEASE_CC_FLAGS) -o $@ $<

$(RELEASE_OBJECT_DIR)/BasicsUT.o	:	BasicsUT.cpp
	$(CC) $(RELEASE_CC_FLAGS) -o $@ $<

$(RELEASE_OBJECT_DIR)/CostMapUT.o	:	CostMapUT.cpp
	$(CC) $(RELEASE_CC_FLAGS) -o $@ $<

$(RELEASE_OBJECT_DIR)/BasicsCUT.o	:	BasicsCUT.cpp
	$(CC) $(RELEASE_CC_FLAGS) -o $@ $<

.PHONY	:	clean_release
clean_release	:
	rm -f $(RELEASE_TARGET)
	rm -f $(RELEASE_OBJECTS)
