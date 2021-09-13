DEBUG_TARGET_DIR = ../debug
RELEASE_TARGET_DIR = ../release
DEBUG_TARGET = $(DEBUG_TARGET_DIR)/libZnccISPC.a
RELEASE_TARGET = $(RELEASE_TARGET_DIR)/libZnccISPC.a
DEBUG_OBJECT_DIR = $(DEBUG_TARGET_DIR)/o
RELEASE_OBJECT_DIR = $(RELEASE_TARGET_DIR)/o
DEBUG_OBJECTS = $(DEBUG_OBJECT_DIR)/Image.o $(DEBUG_OBJECT_DIR)/ZnccHalf.o \
	 $(DEBUG_OBJECT_DIR)/SpecialImages.o $(DEBUG_OBJECT_DIR)/CostMap.o $(DEBUG_OBJECT_DIR)/CostMapGen.o
RELEASE_OBJECTS = $(RELEASE_OBJECT_DIR)/Image.o $(RELEASE_OBJECT_DIR)/ZnccHalf.o \
	$(RELEASE_OBJECT_DIR)/SpecialImages.o $(RELEASE_OBJECT_DIR)/CostMap.o $(RELEASE_OBJECT_DIR)/CostMapGen.o

.PHONY	:	debug
debug	:	$(DEBUG_TARGET)
$(DEBUG_TARGET)	:	$(DEBUG_OBJECTS)
	mkdir -p $(DEBUG_TARGET_DIR)
	ar r $@ $^
	ranlib $@

$(DEBUG_OBJECT_DIR)/Image.o	:	Image.ispc Image.isph Basic2DTypes.isph SumKernel.isph
	mkdir -p $(DEBUG_OBJECT_DIR)
	ispc -I. -h Image.hpp -o $@ --target=avx1-i32x8 -g -O0 $<

$(DEBUG_OBJECT_DIR)/ZnccHalf.o	:	ZnccHalf.ispc ZnccHalf.isph Basic2DTypes.isph SumKernel.isph Image.isph
	mkdir -p $(DEBUG_OBJECT_DIR)
	ispc -I. -h ZnccHalf.hpp -o $@ --target=avx1-i32x8 -g -O0 $<

$(DEBUG_OBJECT_DIR)/SpecialImages.o	:	SpecialImages.ispc SpecialImages.isph Basic2DTypes.isph SumKernel.isph Image.isph
	mkdir -p $(DEBUG_OBJECT_DIR)
	ispc -I. -h SpecialImages.hpp -o $@ --target=avx1-i32x8 -g -O0 $<

$(DEBUG_OBJECT_DIR)/CostMap.o	:	CostMap.ispc CostMap.isph Basic2DTypes.isph
	mkdir -p $(DEBUG_OBJECT_DIR)
	ispc -I. -h CostMap.hpp -o $@ --target=avx1-i32x8 -g -O0 $<

$(DEBUG_OBJECT_DIR)/CostMapGen.o	:	CostMapGen.ispc CostMapGen.isph Basic2DTypes.isph
	mkdir -p $(DEBUG_OBJECT_DIR)
	ispc -I. -h CostMapGen.hpp -o $@ --target=avx1-i32x8 -g -O0 $<

.PHONY	:	clean_debug
clean_debug	:
	rm -f $(DEBUG_OBJECTS)
	rm -f ./*.hpp
	rm -f $(DEBUG_TARGET)

# --------------- release -----------------------
.PHONY	:	release
release	:	$(RELEASE_TARGET)
$(RELEASE_TARGET)	:	$(RELEASE_OBJECTS)
	mkdir -p $(RELEASE_TARGET_DIR)
	ar r $@ $^
	ranlib $@

$(RELEASE_OBJECT_DIR)/Image.o	:	Image.ispc Image.isph Basic2DTypes.isph SumKernel.isph
	mkdir -p $(RELEASE_OBJECT_DIR)
	ispc -I. -h Image.hpp -o $@ --target=avx1-i32x8 -O3 $<

$(RELEASE_OBJECT_DIR)/ZnccHalf.o	:	ZnccHalf.ispc ZnccHalf.isph Basic2DTypes.isph SumKernel.isph Image.isph
	mkdir -p $(RELEASE_OBJECT_DIR)
	ispc -I. -h ZnccHalf.hpp -o $@ --target=avx1-i32x8 -O3 $<

$(RELEASE_OBJECT_DIR)/SpecialImages.o	:	SpecialImages.ispc SpecialImages.isph Basic2DTypes.isph SumKernel.isph Image.isph
	mkdir -p $(RELEASE_OBJECT_DIR)
	ispc -I. -h SpecialImages.hpp -o $@ --target=avx1-i32x8 -O3 $<

$(RELEASE_OBJECT_DIR)/CostMap.o	:	CostMap.ispc CostMap.isph Basic2DTypes.isph
	mkdir -p $(RELEASE_OBJECT_DIR)
	ispc -I. -h CostMap.hpp -o $@ --target=avx1-i32x8 -O3 $<

$(RELEASE_OBJECT_DIR)/CostMapGen.o	:	CostMapGen.ispc CostMapGen.isph Basic2DTypes.isph
	mkdir -p $(RELEASE_OBJECT_DIR)
	ispc -I. -h CostMapGen.hpp -o $@ --target=avx1-i32x8 -O3 $<

.PHONY	:	clean_release
clean_release	:
	rm -f $(RELEASE_OBJECTS)
	rm -f ./*.hpp
	rm -f $(RELEASE_TARGET)
