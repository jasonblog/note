#[macro_use]
extern crate name_derive;
extern crate name;

use name::Name;

#[derive(Name)]
struct Foo;

fn main() {
    println!("{}", Foo::name());
}
