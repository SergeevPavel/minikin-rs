use std::env;
use std::path::PathBuf;
use std::process::Command;
use std::collections::HashMap;
use path_slash::PathBufExt;
use std::str::FromStr;

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
    // https://docs.microsoft.com/en-us/cpp/c-runtime-library/crt-library-features?view=vs-2019

    let target = std::env::var("TARGET").unwrap();
    let target_env = std::env::var("CARGO_CFG_TARGET_ENV").unwrap();
    let out_dir = PathBuf::from(env::var_os("OUT_DIR").unwrap());
    let src_dir = PathBuf::from(env::var_os("CARGO_MANIFEST_DIR").unwrap());

    // rustc always links against non-debug Windows runtime https://github.com/rust-lang/rust/issues/39016
    let build_debug = &target_env != "msvc" && bool::from_str(env::var("DEBUG").unwrap().as_str()).unwrap();

    // https://doc.rust-lang.org/reference/linkage.html#static-and-dynamic-c-runtimes
    let features = env::var("CARGO_CFG_TARGET_FEATURE").unwrap_or(String::new());
    println!("CARGO_CFG_TARGET_FEATURE={}", features);
    let static_crt = features.contains("crt-static");

    {
        let mut env = HashMap::<String, String>::new();
        env.insert(String::from("OUT_DIR"), out_dir.to_slash().unwrap());
        env.insert(String::from("SRC_DIR"), src_dir.to_slash().unwrap());
        env.insert(String::from("DEBUG"), build_debug.to_string());

        if &target_env == "msvc" {
            let tool = cc::windows_registry::find_tool(&target, "cl.exe").expect("Cannot find msvc");
            for (k, v) in tool.env() {
                env.insert(k.clone().into_string().unwrap(), v.clone().into_string().unwrap());
            }

            let msvc_rt_flag = match (static_crt, build_debug) {
                (true, true) => "-MTd",
                (true, false) => "-MT",
                (false, true) => "-MDd",
                (false, false) => "-MD"
            };
            env.insert(String::from("CFLAGS"), String::from(msvc_rt_flag));
            env.insert(String::from("CXXFLAGS"), String::from(msvc_rt_flag));
        }

        println!("cargo:warning=Run make with env: {:?}", env);
        assert!(Command::new("make")
            .args(&["-f", "makefile.cargo", "-j", &env::var("NUM_JOBS").unwrap()])
            .envs(env)
            .status().unwrap()
            .success());

        println!("cargo:rustc-link-search=native={}", out_dir.join("lib").display());
        if &target_env == "msvc" {
            if build_debug {
                println!("cargo:rustc-link-lib=static=sicuucd");
                println!("cargo:rustc-link-lib=static=sicudtd");
            } else {
                println!("cargo:rustc-link-lib=static=sicuuc");
                println!("cargo:rustc-link-lib=static=sicudt");
            }
        } else {
            println!("cargo:rustc-link-lib=static=icuuc");
            println!("cargo:rustc-link-lib=static=icudata");
        }
    }

    {
        let mut cfg = cc::Build::new();
        cfg.cpp(true)
            .flag("-std=c++11")
            .static_crt(static_crt)
            .define("HAVE_ICU", "1")
            .define("HAVE_ICU_BUILTIN", "1");
            // .define("HAVE_OT", "1");

        if !target.contains("windows") {
            cfg.define("HAVE_PTHREAD", "1");
        }

        if target.contains("apple") {
            cfg.define("HAVE_CORETEXT", "1");
        }

        cfg.include(out_dir.join("include"))
            .file("minikin/third-party/harfbuzz/src/harfbuzz.cc")
            .file("minikin/third-party/harfbuzz/src/hb-icu.cc")
            .compile("harfbuzz");
    }

    {
        let mut minikin_cfg = cmake::Config::new("minikin");
        if build_debug {
            minikin_cfg.profile("Debug");
        } else {
            minikin_cfg.profile("Release");
        }
        if !static_crt {
            minikin_cfg.define("MSVC_DYNAMIC_RT", "ON");
        } else {
            minikin_cfg.define("MSVC_DYNAMIC_RT", "OFF");
        }
        let minikin_out = minikin_cfg.build();
        println!("cargo:rustc-link-search=native={}", minikin_out.display());
        println!("cargo:rustc-link-lib=static=minikin");
    }

    if let Some(name) = cpp_stdlib_name() {
        println!("cargo:rustc-link-lib={}", name);
    }
}