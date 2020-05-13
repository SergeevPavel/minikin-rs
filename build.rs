extern crate cmake;

use std::env;
use std::path::PathBuf;
use std::process::Command;

fn main() {
    let out_dir = PathBuf::from(env::var_os("OUT_DIR").unwrap());
    assert!(Command::new("./minikin/build-third-party.sh").arg(out_dir.display().to_string()).status().unwrap().success());
    println!("cargo:rustc-link-search=native={}",
             out_dir
                 .join("packages")
                 .join("lib")
                 .display());
    println!("cargo:rustc-link-lib=static=harfbuzz");
    println!("cargo:rustc-link-lib=static=harfbuzz-icu");
    println!("cargo:rustc-link-lib=static=icuuc");
    println!("cargo:rustc-link-lib=static=icudata");

    let minikin_dst = cmake::Config::new("minikin").build();
    println!("Place minikin in: {}", minikin_dst.display());
    println!("cargo:rustc-link-search=native={}", minikin_dst.display());
    println!("cargo:rustc-link-lib=c++"); // look at cc crate for propper library name
    println!("cargo:rustc-link-lib=static=minikin");
}