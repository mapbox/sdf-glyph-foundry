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

std::string renderGlyph(FT_ULong code_point, std::string font_name) {
    FT_Library library = nullptr;
    ft_library_guard library_guard(library);
    FT_Error error = FT_Init_FreeType(&library);
    if (error) {
        fprintf(stderr,"could not open FreeType library");
        return "";
    }

    FT_Face ft_face = 0;
    ft_face_guard face_guard(ft_face);
    std::string path = "test/fonts/" + font_name + ".ttf";
    FT_Error face_error = FT_New_Face(library, path.c_str() , 0, &ft_face);
    if (face_error) {
        fprintf(stderr,"could not open font");
        return "";
    }

    const double scale_factor = 4.0;

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
    sdf_glyph_foundry::RenderSDF(glyph, size, 3, 0.25, ft_face);
    return glyph.bitmap;
};

void testGlyph(FT_ULong code_point, std::string const& name, std::string const& font_name) {
    std::string bitmap = renderGlyph(code_point, font_name);
    std::ifstream t(("test/fixtures/" + font_name + "/" + name).c_str());
    if (!t) {
        std::ofstream output(("test/fixtures/" + font_name + "/" + name).c_str(), std::ios::binary);
        output << bitmap;
        output.close();
    }
    else {
        std::stringstream buffer;
        buffer << t.rdbuf();
        assert(buffer.str() == bitmap);
    }
};

int main(int argc, char** argv)
{
    // test some printable ASCII
    for (int i = 64; i <= 90; ++i)
    {
        testGlyph(i, std::string(1, char(i)), "OpenSans-Regular");
        testGlyph(i, std::string(1, char(i)), "Jost-Bold");
    }
    for (int i = 97; i <= 122; ++i)
    {
        testGlyph(i, std::string(1, char(i)), "OpenSans-Regular");
        testGlyph(i, std::string(1, char(i)), "Jost-Bold");
    }
    std::cerr << "Done."<< std::endl;
    return 0;
}
