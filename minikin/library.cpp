#include "library.h"

#include <minikin/Layout.h>
#include <bindings/UnicodeUtils.h>
#include <bindings/MinikinFontForTest.h>

using namespace minikin;


std::shared_ptr<FontFamily> getFontFamily(std::string path) {
  std::vector<Font> fonts;
  auto minikinFont = std::make_shared<MinikinFontForTest>(path);
  fonts.emplace_back(minikinFont, FontStyle(400, false));
  int variant = VARIANT_DEFAULT;
//  auto languageList = FontStyle::registerLanguageList(std::string("und-Zsye"));
  auto family = std::make_shared<FontFamily>(variant, std::move(fonts));
  return family;
}

std::vector<std::shared_ptr<FontFamily>> getFontFamilies() {
  std::vector<std::shared_ptr<FontFamily>> families;
  families.push_back(getFontFamily("/System/Library/Fonts/Supplemental/Verdana.ttf"));
  families.push_back(getFontFamily("/System/Library/Fonts/Apple Color Emoji.ttc"));
  return families;
}

std::shared_ptr<FontCollection> getFontCollection() {
  return std::make_shared<FontCollection>(getFontFamilies());
}


void dump_layout() {
  auto layout = minikin::Layout();
  auto paint = minikin::MinikinPaint();
  auto mCollection = std::shared_ptr<FontCollection>(getFontCollection());
  auto text = utf8ToUtf16("hello world 😀");
  layout.doLayout(text.data(), 0, text.size(), text.size(), kBidi_LTR,
                  FontStyle(), paint, mCollection);
  layout.dump();
}
