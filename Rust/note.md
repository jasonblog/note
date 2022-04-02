## Rust Note



### SWAP 


```rust
pub fn swap_a<T: Default>(x: &mut T, y: &mut T) {
    let t = std::mem::take(x);
    *x = std::mem::take(y);
    *y = t;
}

pub fn swap_b<T: Clone>(x: &mut T, y: &mut T) {
    let t = x.clone();
    *x = y.clone();
    *y = t;
}

fn swap_c<T>(x: *mut T, y: *mut T) {
    unsafe {
        let t = std::ptr::read(x);
        std::ptr::write(x, std::ptr::read(y));
        std::ptr::write(y, t);
    }
}

fn main() {
    let (mut a, mut b) = (10, 20);
    println!("a = {}, b = {}", a, b);
    swap_a(&mut a, &mut b);
    println!("a = {}, b = {}", a, b);

    let (mut a, mut b) = (10, 20);
    println!("a = {}, b = {}", a, b);
    swap_b(&mut a, &mut b);
    println!("a = {}, b = {}", a, b);

    let (mut a, mut b) = (10, 20);
    println!("a = {}, b = {}", a, b);
    swap_c(&mut a, &mut b);
    println!("a = {}, b = {}", a, b);
}

```

