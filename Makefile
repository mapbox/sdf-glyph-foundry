CXXFLAGS += -I include -std=c++14 -Wall -Wextra -Wshadow -O3 -fPIC

MASON ?= .mason/mason
BOOST = boost 1.61.0
FREETYPE = freetype 2.6

DEPS = `$(MASON) cflags $(BOOST)` `$(MASON) cflags $(FREETYPE)`
STATIC_LIBS = `$(MASON) static_libs $(FREETYPE)`

default: test

mason_packages/headers/boost:
	$(MASON) install $(BOOST)
	$(MASON) install $(FREETYPE)

build:
	mkdir -p build

CFLAGS += -fvisibility=hidden

build/test: test/test.cpp test/fonts/* test/fixtures/* mason_packages/headers/boost include/mapbox/* include/agg/* build
	$(CXX) $(CFLAGS) $(CXXFLAGS) $(DEPS) $< $(STATIC_LIBS) -lz -o $@

test: build/test
	./build/test

format:
	clang-format include/mapbox/*.hpp include/agg/*.h* src/glyph_foundry.cpp test/*.cpp -i

clean:
	rm -rf build
