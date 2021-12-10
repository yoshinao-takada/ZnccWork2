CC = clang
INCFLAGS = -I.. -I/usr/include/opencv4
DEBUG_TARGET_DIR = ../debug
DEBUG_OBJECT_DIR = $(DEBUG_TARGET_DIR)/o
RELEASE_TARGET_DIR = ../release
RELEASE_OBJECT_DIR = $(RELEASE_TARGET_DIR)/o
DEBUG_CXX_FLAGS = -fPIE -c -g -D_DEBUG $(INCFLAGS) -std=c++17
RELEASE_CXX_FLAGS = -fPIE -c -O3 -DNDEBUG $(INCFLAGS) -std=c++17
DEBUG_CC_FLAGS = -fPIE -c -g -D_DEBUG $(INCFLAGS) -std=c17
RELEASE_CC_FLAGS = -fPIE -c -O3 -DNDEBUG $(INCFLAGS) -std=c17
DEBUG_TARGET = $(DEBUG_TARGET_DIR)/libUtil.a
RELEASE_TARGET = $(RELEASE_TARGET_DIR)/libUtil.a
DEBUG_OBJECTS = $(DEBUG_OBJECT_DIR)/imagelog.o $(DEBUG_OBJECT_DIR)/stopwatch.o \
	$(DEBUG_OBJECT_DIR)/imagesource.o $(DEBUG_OBJECT_DIR)/costtablelog.o $(DEBUG_OBJECT_DIR)/histogram.o \
	$(DEBUG_OBJECT_DIR)/FastHistogram.o
RELEASE_OBJECTS = $(RELEASE_OBJECT_DIR)/imagelog.o $(RELEASE_OBJECT_DIR)/stopwatch.o \
	$(RELEASE_OBJECT_DIR)/imagesource.o $(RELEASE_OBJECT_DIR)/costtablelog.o $(RELEASE_OBJECT_DIR)/histogram.o \
	$(RELEASE_OBJECT_DIR)/FastHistogram.o

# ---------------- debug -----------------
.PHONY	:	debug
debug	:	$(DEBUG_TARGET)
$(DEBUG_TARGET)	:	$(DEBUG_OBJECTS)
	ar r $@ $^
	ranlib $@

$(DEBUG_OBJECT_DIR)/imagelog.o	:	imagelog.cpp imagelog.h
	mkdir -p $(DEBUG_OBJECT_DIR)
	$(CC) $(DEBUG_CXX_FLAGS) -o $@ $<

$(DEBUG_OBJECT_DIR)/stopwatch.o	:	stopwatch.cpp stopwatch.h
	mkdir -p $(DEBUG_OBJECT_DIR)
	$(CC) $(DEBUG_CXX_FLAGS) -o $@ $<

$(DEBUG_OBJECT_DIR)/imagesource.o	:	imagesource.cpp imagesource.h
	mkdir -p $(DEBUG_OBJECT_DIR)
	$(CC) $(DEBUG_CXX_FLAGS) -o $@ $<

$(DEBUG_OBJECT_DIR)/costtablelog.o	:	costtablelog.cpp costtablelog.h
	mkdir -p $(DEBUG_OBJECT_DIR)
	$(CC) $(DEBUG_CXX_FLAGS) -o $@ $<

$(DEBUG_OBJECT_DIR)/histogram.o	:	histogram.c histogram.h
	mkdir -p $(DEBUG_OBJECT_DIR)
	$(CC) $(DEBUG_CC_FLAGS) -o $@ $<

$(DEBUG_OBJECT_DIR)/FastHistogram.o	:	FastHistogram.c FastHistogram.h
	mkdir -p $(DEBUG_OBJECT_DIR)
	$(CC) $(DEBUG_CC_FLAGS) -o $@ $<

.PHONY	:	clean_debug
clean_debug	:
	rm -f $(DEBUG_TARGET)
	rm -f $(DEBUG_OBJECTS)

# ---------------- release -----------------
.PHONY	:	release
release	:	$(RELEASE_TARGET)
$(RELEASE_TARGET)	:	$(RELEASE_OBJECTS)
	ar r $@ $^
	ranlib $@

$(RELEASE_OBJECT_DIR)/imagelog.o	:	imagelog.cpp imagelog.h
	mkdir -p $(RELEASE_OBJECT_DIR)
	$(CC) $(RELEASE_CXX_FLAGS) -o $@ $<

$(RELEASE_OBJECT_DIR)/stopwatch.o	:	stopwatch.cpp stopwatch.h
	mkdir -p $(RELEASE_OBJECT_DIR)
	$(CC) $(RELEASE_CXX_FLAGS) -o $@ $<

$(RELEASE_OBJECT_DIR)/imagesource.o	:	imagesource.cpp imagesource.h
	mkdir -p $(RELEASE_OBJECT_DIR)
	$(CC) $(RELEASE_CXX_FLAGS) -o $@ $<

$(RELEASE_OBJECT_DIR)/costtablelog.o	:	costtablelog.cpp costtablelog.h
	mkdir -p $(RELEASE_OBJECT_DIR)
	$(CC) $(RELEASE_CXX_FLAGS) -o $@ $<

$(RELEASE_OBJECT_DIR)/histogram.o	:	histogram.c histogram.h
	mkdir -p $(RELEASE_OBJECT_DIR)
	$(CC) $(RELEASE_CC_FLAGS) -o $@ $<

$(RELEASE_OBJECT_DIR)/FastHistogram.o	:	FastHistogram.c FastHistogram.h
	mkdir -p $(RELEASE_OBJECT_DIR)
	$(CC) $(RELEASE_CC_FLAGS) -o $@ $<

.PHONY	:	clean_release
clean_release	:
	rm -f $(RELEASE_TARGET)
	rm -f $(RELEASE_OBJECTS)
