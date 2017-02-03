CXXFLAGS += -I include -std=c++11 -Wall -Wextra -Wshadow -O3 -fPIC

MASON ?= .mason/mason
BOOST_VERSION = 1.63.0
FREETYPE_VERSION = 2.6.5

MASON_INCLUDES = -isystem mason_packages/.link/include -isystem mason_packages/.link/include/freetype2

STATIC_LIBS = $(shell $(MASON) static_libs freetype $(FREETYPE_VERSION))

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

build:
	mkdir -p build

CFLAGS += -fvisibility=hidden

build/test: test/test.cpp test/fonts/* test/fixtures/* deps include/mapbox/* include/agg/* build
	$(CXX) $(CFLAGS) $(CXXFLAGS) $(MASON_INCLUDES) $< $(STATIC_LIBS) -lz -o $@

test: build/test
	./build/test

format:
	clang-format include/mapbox/*.hpp include/agg/*.h* src/glyph_foundry.cpp test/*.cpp -i

clean:
	rm -rf build
