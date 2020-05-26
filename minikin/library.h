#ifndef MINIKIN_LIBRARY_H
#define MINIKIN_LIBRARY_H

#include <cstdint>

#include <minikin/MinikinFont.h>
#include <minikin/FontCollection.h>
#include <minikin/Layout.h>

using namespace minikin;

struct GlyphDimensions {
    int32_t left;
    int32_t top;
    int32_t width;
    int32_t height;
    float_t advance;
};

typedef const std::shared_ptr<MinikinFont>* MinikinFontToken;
typedef std::shared_ptr<FontCollection>* FontCollectionToken;

// (font_id, font_size, flyph_id) -> glyph_dimensions
typedef GlyphDimensions (*measure_glyph_t)(float_t, uint32_t, void*);

extern "C" MinikinFontToken create_font(uint32_t  fontId, const uint8_t* bytes, uint32_t size, measure_glyph_t measureGlyph, void* arg);
extern "C" void destroy_font(MinikinFontToken font);

extern "C" FontCollectionToken create_font_collection(MinikinFontToken fonts[], uint32_t count);
extern "C" void destroy_font_collection(FontCollectionToken fontCollection);

// Extract the advances from the metrics. The get_glyph_dimensions API
// has a limitation that it can't currently get dimensions for non-renderable
// glyphs (e.g. spaces), so just use a rough estimate in that case.
//let space_advance = size.to_f32_px() / 3.0;

extern "C" Layout* layout_text(const uint8_t* text,
        uint32_t textLength,
                               bool isRtl,
                               float_t fontSize,
                               std::shared_ptr<FontCollection>* fontCollection);

extern "C" void destroy_layout(Layout* layout);

extern "C" MinikinRect get_bounds(const Layout* layout);
extern "C" float_t get_advance(const Layout* layout);
extern "C" size_t glyphs_count(const Layout* layout);
extern "C" uint32_t get_font_id(const Layout* layout, size_t i);
extern "C" uint32_t get_glyph_id(const Layout* layout, size_t i);
extern "C" float_t get_x(const Layout* layout, size_t i);
extern "C" float_t get_y(const Layout* layout, size_t i);
extern "C" int32_t get_cluster(const Layout* layout, size_t i);


#endif //MINIKIN_LIBRARY_H
