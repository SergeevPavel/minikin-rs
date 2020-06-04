#include "library.h"

#include <minikin/Layout.h>
#include <utils/UnicodeUtils.h>

using namespace minikin;

static uint32_t nextId = 0;

class ProxyMinikinFont : public MinikinFont {
public:
    ProxyMinikinFont(int64_t fontId, const uint8_t* bytes, uint32_t size, std::vector<FontVariation> variations, measure_glyph_t measureGlyph, void* measureGlyphArg)
    : MinikinFont(nextId++)
    , mFontId(fontId)
    , mFontIndex(0)
    , mMeasureGlyph(measureGlyph)
    , mMeasureGlyphArg(measureGlyphArg)
    , mVariations(variations) {
        mHbBlob = hb_blob_create((const char*)bytes, size, HB_MEMORY_MODE_READONLY, nullptr, nullptr);
        mHbFace = hb_face_create(mHbBlob, mFontIndex);
    }

    virtual ~ProxyMinikinFont() {
        hb_face_destroy(mHbFace);
        hb_blob_destroy(mHbBlob);
    }

    float GetHorizontalAdvance(uint32_t glyph_id,
                               const MinikinPaint& paint) const override {
        return mMeasureGlyph(paint.size, glyph_id, this->mMeasureGlyphArg).advance;
    }

    void GetBounds(MinikinRect* bounds,
                   uint32_t glyph_id,
                   const MinikinPaint& paint) const override {
        auto metrics = mMeasureGlyph(paint.size, glyph_id, this->mMeasureGlyphArg);
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

    int64_t fontId() const {

    }

private:
    ProxyMinikinFont() = delete;
    ProxyMinikinFont(const ProxyMinikinFont&) = delete;
    ProxyMinikinFont& operator=(ProxyMinikinFont&) = delete;

    // different fonts can share same fontId
    // e.g. when we load several font instances with different font variations
    // it will use same fontId correspond to WR font id
    const int64_t mFontId;
    const std::vector<FontVariation> mVariations;
    const int mFontIndex;
    hb_blob_t* mHbBlob;
    hb_face_t* mHbFace;
    measure_glyph_t mMeasureGlyph;
    void* mMeasureGlyphArg;
};

MinikinFontToken create_font(uint32_t fontId,
        const uint8_t *bytes,
        uint32_t size,
        uint32_t variationsCount,
        uint32_t* axisTags,
        float_t* variationValues,
        measure_glyph_t measureGlyph,
        void* arg) {
    std::vector<FontVariation> variations;
    for (int i = 0; i < variationsCount; i++) {
        variations.emplace_back(axisTags[i], variationValues[i]);
    }
    return new std::shared_ptr<MinikinFont>(new ProxyMinikinFont(fontId, bytes, size, variations, measureGlyph, arg));
}

void destroy_font(MinikinFontToken font) {
    delete font;
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
    paint.scaleX = 1.0f;
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

float_t get_advance(const Layout* layout) {
    return layout->getAdvance();
}

size_t glyphs_count(const Layout* layout) {
    return layout->nGlyphs();
}

int64_t get_font_id(const Layout* layout, size_t i) {
    return dynamic_cast<const ProxyMinikinFont*>(layout->getFont(i))->fontId();
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

int32_t get_cluster(const Layout* layout, size_t i) {
    return layout->getGlyphCluster(i);
}