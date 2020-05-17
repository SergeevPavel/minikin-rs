#include "library.h"

#include <minikin/Layout.h>
#include <utils/UnicodeUtils.h>

using namespace minikin;

static int32_t uniqueId = 0;

class ProxyMinikinFont : public MinikinFont {
public:
    ProxyMinikinFont(const uint8_t* bytes, uint32_t size, measure_glyph_t measureGlyph)
    : MinikinFont(uniqueId++)
    , mFontIndex(0)
    , measureGlyph(measureGlyph)
    , mVariations() {
        mHbBlob = hb_blob_create((const char*)bytes, size, HB_MEMORY_MODE_READONLY, nullptr, nullptr);
        mHbFace = hb_face_create(mHbBlob, mFontIndex);
    }

    virtual ~ProxyMinikinFont() {
        hb_face_destroy(mHbFace);
        hb_blob_destroy(mHbBlob);
    }

    float GetHorizontalAdvance(uint32_t glyph_id,
                               const MinikinPaint& paint) const override {
        return measureGlyph(this, paint.size, glyph_id).advance;
    }

    void GetBounds(MinikinRect* bounds,
                   uint32_t glyph_id,
                   const MinikinPaint& paint) const override {
        auto metrics = measureGlyph(this, paint.size, glyph_id);
        bounds->mTop = metrics.top;
        bounds->mLeft = metrics.left;
        bounds->mRight = metrics.left + metrics.width;
        bounds->mBottom = metrics.top + metrics.height;
    }

    hb_face_t* CreateHarfBuzzFace() const override {
        return mHbFace;
    }

    const std::vector<minikin::FontVariation>& GetAxes() const override {
        return mVariations;
    }

private:
    ProxyMinikinFont() = delete;
    ProxyMinikinFont(const ProxyMinikinFont&) = delete;
    ProxyMinikinFont& operator=(ProxyMinikinFont&) = delete;

    const std::vector<FontVariation> mVariations;
    const int mFontIndex;
    hb_blob_t* mHbBlob;
    hb_face_t* mHbFace;
    measure_glyph_t measureGlyph;
};

MinikinFontToken create_font(const uint8_t *bytes, uint32_t size, measure_glyph_t measureGlyph) {
    return new std::shared_ptr<MinikinFont>(new ProxyMinikinFont(bytes, size, measureGlyph));
}

void destroy_font(MinikinFontToken font) {
    delete font;
}

MinikinFont* get_font_id(MinikinFontToken font) {
    return font->get();
}

FontCollectionToken create_font_collection(MinikinFontToken fonts[], uint32_t count) {
    std::vector<std::shared_ptr<FontFamily>> families;
    for (int i = 0; i < count; i++) {
        std::vector<Font> family_fonts;
        family_fonts.emplace_back(*fonts[i], FontStyle());
        auto family = std::make_shared<FontFamily>(move(family_fonts));
        families.push_back(family);
    };
    return new std::shared_ptr<FontCollection>(new FontCollection(families));
}

void destroy_font_collection(FontCollectionToken fontCollection) {
    delete fontCollection;
}

Layout* layout_text(const uint8_t* text,
                    uint32_t textLength,
                    bool isRtl,
                    float_t fontSize,
                    std::shared_ptr<FontCollection>* fontCollection) {
    auto layout = new Layout();
    auto utf16Text = utf8ToUtf16(std::string((char*)text, textLength));
    auto paint = MinikinPaint();
    paint.size = fontSize;
    layout->doLayout(utf16Text.data(),
                     0,
                     utf16Text.size(),
                     utf16Text.size(),
                     isRtl,
                     FontStyle(),
                     paint,
                     *fontCollection);
    return layout;
}

void destroy_layout(Layout* layout) {
    delete layout;
}

MinikinRect get_bounds(const Layout* layout) {
    auto bounds = MinikinRect();
    layout->getBounds(&bounds);
    return bounds;
}

size_t glyphs_count(const Layout* layout) {
    return layout->nGlyphs();
}

const MinikinFont* get_font(const Layout* layout, size_t i) {
    return layout->getFont(i);
}

uint32_t get_glyph_id(const Layout* layout, size_t i) {
    return layout->getGlyphId(i);
}

float_t get_x(const Layout* layout, size_t i) {
    return layout->getX(i);
}

float_t get_y(const Layout* layout, size_t i) {
    return layout->getY(i);
}