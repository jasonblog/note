use std::ffi::CString;

// extern "C" {
//     fn say_hello(message: *const c_char);
// }

include!(concat!(env!("OUT_DIR"), "/bindings.rs"));

fn main() {
    let msg = CString::new("Hello").unwrap();
    unsafe { say_hello(msg.as_ptr()) };
}
