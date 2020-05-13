extern crate cmake;

use std::env;
use std::path::PathBuf;
use std::process::Command;

fn cpp_stdlib_name() -> Option<String> {
    if let Ok(stdlib) = env::var("CXXSTDLIB") {
        if stdlib.is_empty() {
            None
        } else {
            Some(stdlib)
        }
    } else {
        let target = env::var("TARGET").unwrap();
        if target.contains("msvc") {
            None
        } else if target.contains("apple") {
            Some("c++".to_string())
        } else if target.contains("freebsd") {
            Some("c++".to_string())
        } else if target.contains("openbsd") {
            Some("c++".to_string())
        } else {
            Some("stdc++".to_string())
        }
    }
}

fn main() {
    let out_dir = PathBuf::from(env::var_os("OUT_DIR").unwrap());
    // assert!(Command::new("./minikin/build-third-party.sh").arg(out_dir.display().to_string()).status().unwrap().success());

    assert!(Command::new("make")
        .args(&["-f", "makefile.cargo", "-j", &env::var("NUM_JOBS").unwrap()])
        .status()
        .unwrap()
        .success());

    println!("cargo:rustc-link-search=native={}",
             out_dir
                 .join("lib")
                 .display());
    println!("cargo:rustc-link-lib=static=harfbuzz");
    println!("cargo:rustc-link-lib=static=harfbuzz-icu");
    println!("cargo:rustc-link-lib=static=icuuc");
    println!("cargo:rustc-link-lib=static=icudata");

    let minikin_dst = cmake::Config::new("minikin").build();
    println!("Place minikin in: {}", minikin_dst.display());
    println!("cargo:rustc-link-search=native={}", minikin_dst.display());
    cpp_stdlib_name().map(|name| {
        println!("cargo:rustc-link-lib={}", name);
    });
    println!("cargo:rustc-link-lib=static=minikin");
}