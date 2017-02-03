
CXXFLAGS += -Iinclude -std=c++11 -fPIC -fvisibility=hidden
RELEASE_FLAGS := -O3 -DNDEBUG
WARNING_FLAGS := -Wall -Wextra -pedantic -Wsign-compare -Wconversion -Wfloat-conversion -Wdouble-promotion -Wshadow
DEBUG_FLAGS := -g -O0 -DDEBUG -fno-inline-functions -fno-omit-frame-pointer

COMPILER_VERSION = $(shell $(CXX) -v 2>&1)

ifneq (,$(findstring clang,$(COMPILER_VERSION)))
    WARNING_DISABLE := -Wno-c99-extensions -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-exit-time-destructors
    PROFILING_FLAG := -gline-tables-only
endif

MASON ?= .mason/mason
BOOST_VERSION = 1.63.0
FREETYPE_VERSION = 2.6.5

MASON_INCLUDES = -isystem mason_packages/.link/include -isystem mason_packages/.link/include/freetype2

STATIC_LIBS = $(shell $(MASON) static_libs freetype $(FREETYPE_VERSION))

export BUILDTYPE ?= Release

ifeq ($(BUILDTYPE),Release)
	FINAL_FLAGS := $(PROFILING_FLAG) $(WARNING_FLAGS) $(WARNING_DISABLE) $(RELEASE_FLAGS)
else
	FINAL_FLAGS := -g $(WARNING_FLAGS) $(DEBUG_FLAGS)
endif


default: test

$(MASON):
	git submodule update --init

mason_packages/.link/include/boost: $(MASON)
	$(MASON) install boost $(BOOST_VERSION)
	$(MASON) link boost $(BOOST_VERSION)

mason_packages/.link/include/freetype:
	$(MASON) install freetype $(FREETYPE_VERSION)
	$(MASON) link freetype $(FREETYPE_VERSION)

deps: mason_packages/.link/include/boost mason_packages/.link/include/freetype

build/$(BUILDTYPE):
	mkdir -p build/$(BUILDTYPE)

build/$(BUILDTYPE)/test: test/test.cpp test/fonts/* test/fixtures/* deps include/mapbox/* include/agg/* build/$(BUILDTYPE)
	$(CXX) $(CXXFLAGS) $(FINAL_FLAGS) $(MASON_INCLUDES) $< $(STATIC_LIBS) -lz -o $@

test: build/$(BUILDTYPE)/test
	./build/$(BUILDTYPE)/test

format:
	clang-format include/mapbox/*.hpp include/agg/*.h* src/glyph_foundry.cpp test/*.cpp -i

clean:
	rm -rf build
