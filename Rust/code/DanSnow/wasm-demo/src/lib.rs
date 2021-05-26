extern crate wasm_bindgen;
extern crate web_sys;

use wasm_bindgen::prelude::*;

#[wasm_bindgen]
pub fn main() -> Result<(), JsValue> {
    let window = web_sys::window().unwrap();
    let document = window.document().unwrap();
    let body = document.body().unwrap();
    let el = document.create_element("p")?;
    el.set_inner_html("Hello from Rust");
    AsRef::<web_sys::Node>::as_ref(&body).append_child(el.as_ref())?;
    Ok(())
}
