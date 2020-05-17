extern crate libc;

#[repr(C)]
#[derive(Debug)]
pub struct GlyphDimensions {
    left: i32,
    top: i32,
    width: i32,
    height: i32,
    advance: i32
}

#[repr(C)]
#[derive(Debug)]
pub struct MinikinRect {
    left: f32,
    top: f32,
    right: f32,
    bottom: f32
}

type MinikinFontToken = u64;
type MinikinFontId = u64;

type FontCollectionToken = u64;

type LayoutToken = u64;

pub type MeasureGlyph = unsafe extern "C" fn(MinikinFontId, f32, u32) -> GlyphDimensions;

extern {
    fn create_font(bytes: *const u8, size: u32, measure_glyph: MeasureGlyph) -> MinikinFontToken;
    fn destroy_font(font: MinikinFontToken);
    fn get_font_id(font: MinikinFontToken) -> MinikinFontId;

    fn create_font_collection(fonts: *const MinikinFontToken, count: u32) -> FontCollectionToken;
    fn destroy_font_collection(font_collection: FontCollectionToken);

    fn layout_text(text: *const u8, text_length: u32, is_rtl: bool, font_size: f32, font_collection: FontCollectionToken) -> LayoutToken;
    fn destroy_layout(layout: LayoutToken);
    fn get_bounds(layout: LayoutToken) -> MinikinRect;
    fn glyphs_count(layout: LayoutToken) -> usize;
    fn get_font(layout: LayoutToken, i: usize) -> MinikinFontId;
    fn get_glyph_id(layout: LayoutToken, i: usize) -> u32;
    fn get_x(layout: LayoutToken, i: usize) -> f32;
    fn get_y(layout: LayoutToken, i: usize) -> f32;
}

#[cfg(test)]
mod tests {
    use crate::*;

    use std::io;
    use std::io::prelude::*;
    use std::fs::File;
    use std::path::PathBuf;

    extern "C" fn measure_glyph(font_id: MinikinFontId, font_size: f32, glyph_id: u32) -> GlyphDimensions {
        GlyphDimensions {
            left: 0,
            top: 0,
            width: 10,
            height: 10,
            advance: 10
        }
    }

    #[test]
    fn it_works() {
        let mut root = PathBuf::from(env!("CARGO_MANIFEST_DIR"));
        let mut font_file1 = File::open(root.join("fonts/Roboto-Regular.ttf")).unwrap();
        let mut font_buffer1 = Vec::new();
        let mut font_file2 = File::open(root.join("fonts/OpenSansEmoji.ttf")).unwrap();
        let mut font_buffer2 = Vec::new();
        font_file1.read_to_end(&mut font_buffer1).unwrap();
        font_file2.read_to_end(&mut font_buffer2).unwrap();
        unsafe {
            let font1 = create_font(font_buffer1.as_ptr(), font_buffer1.len() as u32, measure_glyph);
            let font2 = create_font(font_buffer2.as_ptr(), font_buffer2.len() as u32, measure_glyph);
            let fonts = vec![font1, font2];
            let font_collection = create_font_collection(fonts.as_ptr(), fonts.len() as u32);

            println!("Roboto font id: {}", get_font_id(font1));
            println!("Emoji font id: {}", get_font_id(font2));
            let text = "Hello 😀 😀 😀 world Хеллоу ворлд";
            let text_bytes = text.as_bytes();
            let layout = layout_text(text_bytes.as_ptr(),
                                     text_bytes.len() as u32,
                                     false,
                                     20.0,
                                     font_collection);

            println!("Bounds: {:?}", get_bounds(layout));
            println!("Glyphs count: {}", glyphs_count(layout));

            for i in 0..glyphs_count(layout) {
                println!("Glyph id: {} Font id: {}, x: {} y: {}",
                         get_glyph_id(layout, i),
                         get_font(layout, i),
                         get_x(layout, i),
                         get_y(layout, i));
            }

            destroy_layout(layout);
            destroy_font_collection(font_collection);
            destroy_font(font1);
            destroy_font(font2);
        }
    }
}
