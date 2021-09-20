CC = clang
DEBUG_TARGET_DIR = debug
RELEASE_TARGET_DIR = release
DEBUG_TARGET = $(DEBUG_TARGET_DIR)/PerfTest.exe
RELEASE_TARGET = $(RELEASE_TARGET_DIR)/PerfTest.exe

.PHONY	:	debug
debug	:	$(DEBUG_TARGET)

$(DEBUG_TARGET)	:	$(DEBUG_TARGET_DIR)/libUtil.a $(DEBUG_TARGET_DIR)/libZnccISPC.a $(DEBUG_TARGET_DIR)/libZnccC.a $(DEBUG_TARGET_DIR)/UnitTest.exe
	cd PerfTest && $(MAKE) CC=$(CC) debug

$(DEBUG_TARGET_DIR)/UnitTest.exe	:	$(DEBUG_TARGET_DIR)/libUtil.a $(DEBUG_TARGET_DIR)/libZnccISPC.a $(DEBUG_TARGET_DIR)/libZnccC.a
	cd UnitTest && $(MAKE) CC=$(CC) debug

$(DEBUG_TARGET_DIR)/libUtil.a	:	$(DEBUG_TARGET_DIR)/libZnccISPC.a $(DEBUG_TARGET_DIR)/libZnccC.a
	cd Util && $(MAKE) CC=$(CC) debug

$(DEBUG_TARGET_DIR)/libZnccISPC.a	:
	cd ZnccISPC && $(MAKE) debug

$(DEBUG_TARGET_DIR)/libZnccC.a	:
	cd ZnccC && $(MAKE) debug

.PHONY	:	clean_debug
clean_debug	:
#	cd PerfTest && $(MAKE) $@
	cd UnitTest && $(MAKE) $@
	cd Util && $(MAKE) $@
	cd ZnccISPC && $(MAKE) $@
	cd ZnccC && $(MAKE) $@


.PHONY	:	release
release	:	$(RELEASE_TARGET)

$(RELEASE_TARGET)	:	$(RELEASE_TARGET_DIR)/libUtil.a $(RELEASE_TARGET_DIR)/libZnccISPC.a $(RELEASE_TARGET_DIR)/libZnccC.a $(RELEASE_TARGET_DIR)/UnitTest.exe
	cd PerfTest && $(MAKE) CC=$(CC) release

$(RELEASE_TARGET_DIR)/UnitTest.exe	:	$(RELEASE_TARGET_DIR)/libUtil.a $(RELEASE_TARGET_DIR)/libZnccISPC.a $(RELEASE_TARGET_DIR)/libZnccC.a
	cd UnitTest && $(MAKE) CC=$(CC) release

$(RELEASE_TARGET_DIR)/libUtil.a	:	$(RELEASE_TARGET_DIR)/libZnccISPC.a $(RELEASE_TARGET_DIR)/libZnccC.a
	cd Util && $(MAKE) CC=$(CC) release

$(RELEASE_TARGET_DIR)/libZnccISPC.a	:
	cd ZnccISPC && $(MAKE) release

$(RELEASE_TARGET_DIR)/libZnccC.a	:
	cd ZnccC && $(MAKE) release

.PHONY	:	clean_release
clean_release	:
#	cd PerfTest && $(MAKE) $@
	cd UnitTest && $(MAKE) $@
	cd Util && $(MAKE) $@
	cd ZnccISPC && $(MAKE) $@
	cd ZnccC && $(MAKE) $@
