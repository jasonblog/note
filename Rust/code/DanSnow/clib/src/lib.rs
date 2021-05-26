use std::{ffi::CStr, os::raw::c_char, mem};

#[no_mangle]
pub extern "C" fn say_hello(message: *const c_char) {
    let message = unsafe { CStr::from_ptr(message) };
    println!("{} from Rust", message.to_str().unwrap());
}

#[repr(C)]
pub struct Point {
    x: i32,
    y: i32,
}

#[no_mangle]
pub extern "C" fn create_point(x: i32, y: i32) -> Point {
    Point { x, y }
}

#[no_mangle]
pub extern "C" fn alloc_memory() -> *mut i32 {
    Box::into_raw(Box::new(42))
}

#[no_mangle]
pub extern "C" fn free_memory(x: *mut i32) {
    drop(unsafe { Box::from_raw(x) });
}

#[no_mangle]
pub extern "C" fn create_vec(size: *mut usize) -> *mut i32 {
  let mut vec = Vec::new();
  vec.push(1);
  vec.push(2);
  vec.push(3);

  vec.shrink_to_fit();
  assert!(vec.capacity() == vec.len());
  unsafe { *size = vec.len() };
  let p = vec.as_mut_ptr();
  mem::forget(vec);
  p
}

#[no_mangle]
pub extern "C" fn free_vec(vec: *mut i32, size: usize) {
  drop(unsafe { Vec::from_raw_parts(vec, size, size) });
}
