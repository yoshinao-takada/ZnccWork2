CC = clang
INCFLAGS = -I..
DEBUG_TARGET_DIR = ../debug
DEBUG_OBJECT_DIR = $(DEBUG_TARGET_DIR)/o
RELEASE_TARGET_DIR = ../release
RELEASE_OBJECT_DIR = $(RELEASE_TARGET_DIR)/o
DEBUG_CC_FLAGS = -fPIE -c -g -D_DEBUG $(INCFLAGS) -std=c++17 -std=c11
RELEASE_CC_FLAGS = -fPIE -fvectorize -c -O3 -DNDEBUG $(INCFLAGS) -std=c++17 -std=c11
DEBUG_TARGET = $(DEBUG_TARGET_DIR)/libZnccC.a
RELEASE_TARGET = $(RELEASE_TARGET_DIR)/libZnccC.a
DEBUG_OBJECTS = $(DEBUG_OBJECT_DIR)/ImageC.o $(DEBUG_OBJECT_DIR)/SpecialImagesC.o \
	$(DEBUG_OBJECT_DIR)/ZnccHalfC.o $(DEBUG_OBJECT_DIR)/CostMapC.o  $(DEBUG_OBJECT_DIR)/GridPointsC.o \
	$(DEBUG_OBJECT_DIR)/CostMapGenC.o
RELEASE_OBJECTS = $(RELEASE_OBJECT_DIR)/ImageC.o $(RELEASE_OBJECT_DIR)/SpecialImagesC.o \
	$(RELEASE_OBJECT_DIR)/ZnccHalfC.o $(RELEASE_OBJECT_DIR)/CostMapC.o $(RELEASE_OBJECT_DIR)/GridPointsC.o \
	$(RELEASE_OBJECT_DIR)/CostMapGenC.o


# --------------------- debug -----------------------
.PHONY	:	debug
debug	:	$(DEBUG_TARGET)
$(DEBUG_TARGET)	:	$(DEBUG_OBJECTS)
	ar r $@ $^
	ranlib $@

$(DEBUG_OBJECT_DIR)/ImageC.o	:	ImageC.c ImageC.h Basic2DTypesC.h
	mkdir -p $(DEBUG_OBJECT_DIR)
	$(CC) $(DEBUG_CC_FLAGS) -o $@ $<

$(DEBUG_OBJECT_DIR)/SpecialImagesC.o	:	SpecialImagesC.c SpecialImagesC.h ImageC.h Basic2DTypesC.h
	mkdir -p $(DEBUG_OBJECT_DIR)
	$(CC) $(DEBUG_CC_FLAGS) -o $@ $<

$(DEBUG_OBJECT_DIR)/ZnccHalfC.o	:	ZnccHalfC.c ZnccHalfC.h ImageC.h Basic2DTypesC.h
	mkdir -p $(DEBUG_OBJECT_DIR)
	$(CC) $(DEBUG_CC_FLAGS) -o $@ $<

$(DEBUG_OBJECT_DIR)/CostMapC.o	:	CostMapC.c CostMapC.h ImageC.h Basic2DTypesC.h
	mkdir -p $(DEBUG_OBJECT_DIR)
	$(CC) $(DEBUG_CC_FLAGS) -o $@ $<

$(DEBUG_OBJECT_DIR)/CostMapGenC.o	:	CostMapGenC.c CostMapGenC.h ImageC.h Basic2DTypesC.h
	mkdir -p $(DEBUG_OBJECT_DIR)
	$(CC) $(DEBUG_CC_FLAGS) -o $@ $<

$(DEBUG_OBJECT_DIR)/GridPointsC.o	:	GridPointsC.c GridPointsC.h ImageC.h Basic2DTypesC.h
	mkdir -p $(DEBUG_OBJECT_DIR)
	$(CC) $(DEBUG_CC_FLAGS) -o $@ $<


.PHONY	:	clean_debug
clean_debug	:
	rm -f $(DEBUG_TARGET)
	rm -f $(DEBUG_OBJECTS)

# --------------------- release -----------------------
.PHONY	:	release
release	:	$(RELEASE_TARGET)
$(RELEASE_TARGET)	:	$(RELEASE_OBJECTS)
	ar r $@ $^
	ranlib $@

$(RELEASE_OBJECT_DIR)/ImageC.o	:	ImageC.c ImageC.h Basic2DTypesC.h
	mkdir -p $(RELEASE_OBJECT_DIR)
	$(CC) $(RELEASE_CC_FLAGS) -o $@ $<

$(RELEASE_OBJECT_DIR)/SpecialImagesC.o	:	SpecialImagesC.c SpecialImagesC.h ImageC.h Basic2DTypesC.h
	mkdir -p $(RELEASE_OBJECT_DIR)
	$(CC) $(RELEASE_CC_FLAGS) -o $@ $<

$(RELEASE_OBJECT_DIR)/ZnccHalfC.o	:	ZnccHalfC.c ZnccHalfC.h ImageC.h Basic2DTypesC.h
	mkdir -p $(RELEASE_OBJECT_DIR)
	$(CC) $(RELEASE_CC_FLAGS) -o $@ $<

$(RELEASE_OBJECT_DIR)/CostMapC.o	:	CostMapC.c CostMapC.h ImageC.h Basic2DTypesC.h
	mkdir -p $(RELEASE_OBJECT_DIR)
	$(CC) $(RELEASE_CC_FLAGS) -o $@ $<

$(RELEASE_OBJECT_DIR)/CostMapGenC.o	:	CostMapGenC.c CostMapGenC.h ImageC.h Basic2DTypesC.h
	mkdir -p $(RELEASE_OBJECT_DIR)
	$(CC) $(RELEASE_CC_FLAGS) -o $@ $<

$(RELEASE_OBJECT_DIR)/GridPointsC.o	:	GridPointsC.c GridPointsC.h ImageC.h Basic2DTypesC.h
	mkdir -p $(RELEASE_OBJECT_DIR)
	$(CC) $(RELEASE_CC_FLAGS) -o $@ $<

.PHONY	:	clean_release
clean_release	:
	rm -f $(RELEASE_TARGET)
	rm -f $(RELEASE_OBJECTS)
