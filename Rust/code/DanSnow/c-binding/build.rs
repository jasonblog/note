extern crate bindgen;
extern crate cc;

use std::{env, path::PathBuf};

fn main() {
    // println!("cargo:rustc-link-lib=static=hello");
    // println!(
    //     "cargo:rustc-link-search={}",
    //     concat!(env!("CARGO_MANIFEST_DIR"), "/lib")
    // );
    cc::Build::new().file("lib/hello.c").compile("hello");
    let bindings = bindgen::Builder::default()
        .header("lib/hello.h")
        .generate()
        .expect("Unable to generate bindings");

    let out_dir = PathBuf::from(env::var("OUT_DIR").unwrap());
    bindings
        .write_to_file(out_dir.join("bindings.rs"))
        .expect("Couldn't write bindings!");
}
