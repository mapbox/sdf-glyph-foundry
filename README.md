# sdf-glyph-foundry

A library that delivers glyphs rendered as SDFs (signed distance fields). We use these encoded glyphs as the basic blocks of font rendering in [Mapbox GL](https://github.com/mapbox/mapbox-gl-js). SDF encoding is superior to traditional fonts for our usecase in terms of scaling, rotation, and quickly deriving halos - WebGL doesn't have built-in font rendering, so the decision is between vectorization, which tends to be slow, and SDF generation.

The approach this library takes is to parse and rasterize the font with Freetype (hence the C++ requirement), and then generate a distance field from that rasterized image.

This library is used by [node-fontnik](https://github.com/mapbox/node-fontnik) to generate SDF glyphs on the server side, and may be used in the future in [Mapbox GL Native](https://github.com/mapbox/mapbox-gl-native) to locally generate SDFs when a system font is available and appropriate.

## Using sdf-glyph-foundry

The glyph foundry is a header-only library. To successfully build it within your project you must also include Boost, Freetype, and Zlib. After pulling the include paths in to your project, use it like this:

    #include <mapbox/glyph_foundry.hpp>
    #include <mapbox/glyph_foundry_impl.hpp>

    ...

    sdf_glyph_foundry::glyph_info glyph; // Fill with glyph request information
    sdf_glyph_foundry::RenderSDF(glyph_info &glyph,
                   int size,
                   int buffer,
                   float cutoff,
                   FT_Face ft_face);

    // SDF bitmap is returned in glyph.bitmap

## Testing

Building the sdf-glyph-foundry tests depend on:

  - make
  - git
  - curl
  - a c++11 capable compiler

Once you have these things then you can run:

    make test

This will automatically install `boost` and `freetype` locally using [mason](https://github.com/mapbox/mason), and build a simple test harness that compares the library results to known SDFs.

## Background reading
- [Drawing Text with Signed Distance Fields in Mapbox GL](https://www.mapbox.com/blog/text-signed-distance-fields/)
- [State of Text Rendering](http://behdad.org/text/)
- [Pango vs HarfBuzz](http://mces.blogspot.com/2009/11/pango-vs-harfbuzz.html)
- [An Introduction to Writing Systems & Unicode](http://rishida.net/docs/unicode-tutorial/)
