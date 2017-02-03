#include <mapbox/glyph_foundry.hpp>
#include <mapbox/glyph_foundry_impl.hpp>

#include <cassert>
#include <fstream>
#include <sstream>
#include <iostream>

struct ft_library_guard {
    ft_library_guard(FT_Library lib) :
        library_(lib) {}

    ~ft_library_guard()
    {
        if (library_) FT_Done_FreeType(library_);
    }

    FT_Library library_;
};

struct ft_face_guard {
    ft_face_guard(FT_Face f) :
        face_(f) {}

    ~ft_face_guard()
    {
        if (face_) FT_Done_Face(face_);
    }

    FT_Face face_;
};

std::string renderGlyph(FT_ULong code_point) {
    FT_Library library = nullptr;
    ft_library_guard library_guard(library);
    FT_Error error = FT_Init_FreeType(&library);
    if (error) {
        fprintf(stderr,"could not open FreeType library");
        return "";
    }

    FT_Face ft_face = 0;
    ft_face_guard face_guard(ft_face);
    FT_Error face_error = FT_New_Face(library, "test/fonts/OpenSans-Regular.ttf", 0, &ft_face);
    if (face_error) {
        fprintf(stderr,"could not open font");
        return "";
    }

    const double scale_factor = 1.0;

    // Set character sizes.
    double size = 24 * scale_factor;
    FT_Set_Char_Size(ft_face,0,(FT_F26Dot6)(size * (1<<6)),0,0);

    sdf_glyph_foundry::glyph_info glyph;

    // Get FreeType face from face_ptr.
    FT_UInt char_index = FT_Get_Char_Index(ft_face, code_point);

    if (!char_index) {
        fprintf(stderr,"could not find a glyph for this code point");
        return "";
    }

    glyph.glyph_index = char_index;
    sdf_glyph_foundry::RenderSDF(glyph, 24, 3, 0.25, ft_face);
    return glyph.bitmap;
};

void testGlyph(FT_ULong code_point, const std::string& name) {
    std::string bitmap = renderGlyph(code_point);

    std::ifstream t(("test/fixtures/" + name).c_str());
    std::stringstream buffer;
    buffer << t.rdbuf();

    assert(buffer.str() == bitmap);
};

int main() {
    testGlyph(41, "A");
    return 0;
}
