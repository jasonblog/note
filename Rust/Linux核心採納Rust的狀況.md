## Linux 核心採納 Rust 的狀況



> 此文於 2021 年間，整理 Rust 現狀在 Linux kernel 中遇到的限制以及問題。
> 　 linD026 Sun, Sep 12, 2021 10:05 PM

------

Rust 撰寫， Linus Torvalds 的想法可以從[此篇訪談](https://www.theregister.com/2020/06/30/hard_to_find_linux_maintainers_says_torvalds/)得知：

> Is C, the language the kernel is for the most part written in, being displaced by the likes of Go and Rust, such that there is “a risk that we’re becoming the COBOL programmers of the 2030s?” Hohndel asked. “C is still one of the top 10 languages,” answered Torvalds. However, he said that for things “not very central to the kernel itself”, like drivers, the kernel team is looking at “having interfaces to do those, for example, in Rust… I’m convinced it’s going to happen. It might not be Rust. But it is going to happen that we will have different models for writing these kinds of things, and C won’t be the only one.”

因此關於 driver 等 kernel 周遭的語言開始考量以 C 語言以外的程式語言開發，如 Rust 。

為什麼會選擇 Rust ？可見此篇 [lkml.org - [PATCH 00/13\] [RFC] Rust support](https://lkml.org/lkml/2021/4/14/1023?fbclid=IwAR04IjpsgkDcrVlQfhmkJFxexp8ea5xAUjtrAbzpmINvkscXai0UThLINO0) 。基本上主要是因為 memory safety 以及基於 system programming language 上相比於 C 有更進階的特徵與功能，使得開發過程中更容易撰寫以及維護。

> \## Why Rust?
>
> Rust is a systems programming language that brings several key advantages over C in the context of the Linux kernel:
>
> \- No undefined behavior in the safe subset (when unsafe code is sound), including memory safety and the absence of data races.
>
> \- Stricter type system for further reduction of logic errors.
>
> \- A clear distinction between safe and `unsafe` code.
>
> \- Featureful language: sum types, pattern matching, generics, RAII, lifetimes, shared & exclusive references, modules & visibility, powerful hygienic and procedural macros…
>
> \- Extensive freestanding standard library: vocabulary types such as `Result` and `Option`, iterators, formatting, pinning, checked/saturating/wrapping integer arithmetic, etc.
>
> \- Integrated out of the box tooling: documentation generator, formatter and linter all based on the compiler itself.
>
> Overall, Rust is a language that has successfully leveraged decades of experience from system programming languages as well as functional ones, and added lifetimes and borrow checking on top.

而在 [lwn.net](https://lwn.net/Articles/829858/) 一篇文章中有提到開發者們對於如何使用 Rust 語言撰寫 Linux 的看法。其中它們強調不會提議用 Rust 重寫整個 kernel ，而是尋找有哪些程式碼適合用 Rust 撰寫：

> The speakers emphasized that they are not proposing a rewrite of the Linux kernel into Rust; they are focused only on moving toward a world where new code may be written in Rust. The ensuing conversation focused on three areas of potential concern for Rust support: making use of the existing APIs in the kernel, architecture support, and a question about ABI compatibility between Rust and C.

因此關於如何使 Rust 與原先的程式碼結合便會成為重點，而上引文末段也提到會著重於這三個部份：

1. 使用現有的 API
2. 架構支援
3. Rust 和 C 的 ABI 兼容性

------

## [lwn.net Supporting Linux kernel development in Rust](https://lwn.net/Articles/829858/)

> [Writing Linux Kernel Modules in Safe Rust - Geoffrey Thomas & Alex Gaynor](https://www.youtube.com/watch?v=RyY01fRyGhM)

### Binding 現有的 C API

Rust 已與 C 語言有很好的互動性，如 [FFI](https://doc.rust-lang.org/nomicon/ffi.html) / [std::ffi](https://doc.rust-lang.org/std/ffi/) 、[ABI](https://doc.rust-lang.org/reference/abi.html) 等，詳細操作可看 [A little Rust with your C](https://docs.rust-embedded.org/book/interoperability/rust-with-c.html) 。

在 Rust 中以 C ABI 連結可以利用操作進行連結， project 當中下 ` cargo init --lib libname` 建立空白的函式庫，之後在其中的 `libname/src/lib.rs` 寫出相對的程式碼：

```rust
use std::os::raw::c_char;
use std::ffi::CString;

fn get_hello_world() -> String {
    return String::from("Hello world C!");
}

#[no_mangle]
pub extern "C" fn c_hello_world() -> *mut c_char {
    let rust_string: String = get_hello_world();

    // Convert the String into a CString
    let c_string: CString = CString::new(rust_string).expect("Could not convert to CString");

    // Instead of returning the CString, we return a pointer for it.
    return c_string.into_raw();
}
```

並且在 `libname/Cargo.toml` 增加：

```
[lib]
name = "your_crate"
crate-type = ["cdylib"]      # Creates dynamic lib
# crate-type = ["staticlib"] # Creates static lib
```

關於 Rust 的 linkage 型態種類請看：[Linkage](https://doc.rust-lang.org/reference/linkage.html)，在此以動態連結函式庫進行。

> A dynamic system library will be produced. This is used when compiling a dynamic library to be loaded from another language. This output type will create `*.so` files on Linux, `*.dylib` files on macOS, and `*.dll` files on Windows.

關於更多如 [Configuration](https://doc.rust-lang.org/cargo/reference/config.html) 細節暫時不作探討。

在之後使用 C 語言撰寫：

```c
#include <stdio.h>

extern char *const c_hello_world();
extern void c_hello_world_free(char *const str);

int main() {
	char *rr = c_hello_world();
	printf("%s", rr);
}
```

最後如一般動態連結函式庫一般：

```bash
$ gcc -c test.c 
$ gcc -o test test.o libname.so
$ LD_LIBRARY_PATH=. ./test
Hello world C!$
```

當然在 Rust 中使用 C 語言所寫的函式庫也是可以的，在此就不再贅述。
可見：

- [用 Rust 呼叫 C 的程式](https://ithelp.ithome.com.tw/articles/10203831)
- 關於連結系統中的函式庫： [pkg-config-rs](https://github.com/rust-lang/pkg-config-rs)
- [Rust FFI (C vs Rust)学习杂记](https://zhuanlan.zhihu.com/p/143179175)

除上述之外，也可以利用 [**rust-bingen**](https://github.com/rust-lang/rust-bindgen) 對 C 語言的標頭檔進行分析進而產生 Rust 的宣告，以此使用 Linux 中現有的 C 語言的 API 而不用重新撰寫。

以下程式碼為例， Linux 的 API 在原先 C 語言的標頭檔形式定義以下程式碼：

```c
typedef struct hello_struct {
    int x;
    int y;
} hs_t;
c
void hello_function(int i, hs_t *h);
```

並在 `build.rs` 以及 `Cargo.toml` 當中撰寫：

```rust
extern crate bindgen;

use std::env;
use std::path::PathBuf;
...
    let bindings = bindgen::Builder::default()
        // The input header we would like to generate
        // bindings for.
        .header("wrapper.h")
        // Tell cargo to invalidate the built crate whenever any of the
        // included header files changed.
        .parse_callbacks(Box::new(bindgen::CargoCallbacks))
        // Finish the builder and generate the bindings.
        .generate()
        // Unwrap the Result and panic on failure.
        .expect("Unable to generate bindings");

    // Write the bindings to the $OUT_DIR/bindings.rs file.
    let out_path = PathBuf::from(env::var("OUT_DIR").unwrap());
    bindings
        .write_to_file(out_path.join("bindings.rs"))
        .expect("Couldn't write bindings!");
[build-dependencies]
bindgen = "0.56.0"
```

最後在下 `cargo build` 轉化成下列 Rust 語言。

```
~/temp$ cargo build
   Compiling temp v0.1.0 (/home/.../temp)
    Finished dev [unoptimized + debuginfo] target(s) in 0.34s
```

以此為例，會在 `/temp/target/debug/build/temp-7da9646fa44d0e3d/out/bindings.rs` 呈現：

```rust
/* automatically generated by rust-bindgen 0.56.0 */

#[repr(C)]
#[derive(Debug, Copy, Clone)]
pub struct hello_struct {
    pub x: ::std::os::raw::c_int,
    pub y: ::std::os::raw::c_int,
}
#[test]
fn bindgen_test_layout_hello_struct() {
    assert_eq!(
        ::std::mem::size_of::<hello_struct>(),
        8usize,
        concat!("Size of: ", stringify!(hello_struct))
    );
    assert_eq!(
        ::std::mem::align_of::<hello_struct>(),
        4usize,
        concat!("Alignment of ", stringify!(hello_struct))
    );
    assert_eq!(
        unsafe { &(*(::std::ptr::null::<hello_struct>())).x as *const _ as usize },
        0usize,
        concat!(
            "Offset of field: ",
            stringify!(hello_struct),
            "::",
            stringify!(x)
        )
    );
    assert_eq!(
        unsafe { &(*(::std::ptr::null::<hello_struct>())).y as *const _ as usize },
        4usize,
        concat!(
            "Offset of field: ",
            stringify!(hello_struct),
            "::",
            stringify!(y)
        )
    );
}
pub type hs_t = hello_struct;
extern "C" {
    pub fn hello_function(i: ::std::os::raw::c_int, h: *mut hs_t);
}
```

> 版本為 `cargo 1.50.0 (f04e7fab7 2021-02-04))` 、`bindgen 0.56.0` 以及 `clang version 10.0.0-4ubuntu1` 。
> 相關教學：[The `bindgen` User Guide](https://rust-lang.github.io/rust-bindgen/introduction.html)

**rustup 版本管理**
關於各工具的版本管理，可以在指定目錄下達如 `rustup override set stable` 等指令來設定此目錄下的工具版本。請見： [Rustup for managing Rust versions](https://doc.rust-lang.org/edition-guide/rust-2018/rustup-for-managing-rust-versions.html)

以下為此 `bindgen` 範例比對另一個目錄下的設定範例：

```
~/linux$ rustup override set beta-2021-06-23
info: using existing install for 'beta-2021-06-23-x86_64-unknown-linux-gnu'
info: override toolchain for '/home/.../linux' set to 'beta-2021-06-23-x86_64-unknown-linux-gnu'

  beta-2021-06-23-x86_64-unknown-linux-gnu unchanged - rustc 1.54.0-beta.1 (bf62f4de3 2021-06-23)
~/temp$ rustup override set stable
info: using existing install for 'stable-x86_64-unknown-linux-gnu'
info: override toolchain for '/home/.../temp' set to 'stable-x86_64-unknown-linux-gnu'

  stable-x86_64-unknown-linux-gnu unchanged - rustc 1.50.0 (cb75ad5db 2021-02-10)
```

然而 Linux 運用了複雜的標頭檔以及 inline function ，因此就算在 [bindgen](https://github.com/rust-lang/rust-bindgen) 以及 [FFI](https://doc.rust-lang.org/nomicon/ffi.html) 的幫助下還是有許多困難之處。

[lwn.net](https://lwn.net/Articles/829858/) 中，有對於如何影響的舉例：

> **The ubiquitous `kmalloc()` function, for instance, is defined as `always_inline`, meaning that it is inlined into all of its callers and no `kmalloc()` symbol exists in the kernel symbol table for Rust to link against.** This problem can be easily worked around — one can define a `kmalloc_for_rust()` symbol containing an un-inlined version — but performing these workarounds by hand would result in a large amount of manual work and duplicated code. This work could potentially be automated by an improved version of bindgen, but such a tool does not yet exist.

以下為 [Rust for Linux](https://github.com/Rust-for-Linux/linux) 當中 [allocator](https://github.com/Rust-for-Linux/linux/blob/rust/rust/kernel/allocator.rs) 的部份程式碼，關於上述 `kmalloc()` 在 Rust 當中改以 `krealloc()` 操作：

```rust
unsafe impl GlobalAlloc for KernelAllocator {
    unsafe fn alloc(&self, layout: Layout) -> *mut u8 {
        // `krealloc()` is used instead of `kmalloc()` because the latter is
        // an inline function and cannot be bound to as a result.
        unsafe { bindings::krealloc(ptr::null(), layout.size(), bindings::GFP_KERNEL) as *mut u8 }
    }

    unsafe fn dealloc(&self, ptr: *mut u8, _layout: Layout) {
        unsafe {
            bindings::kfree(ptr as *const c_types::c_void);
        }
    }
}
```

而在 Github 的 bindgen 也有此 issue 被提及 ： [Generate C code to export static inline functions #1090](https://github.com/rust-lang/rust-bindgen/issues/1090) 。
以 [用 Rust 呼叫 C 的程式](https://ithelp.ithome.com.tw/articles/10203831) 中的程式碼為例，利用綁定 C 語言函式庫以及 [FFI](https://doc.rust-lang.org/std/ffi/) 操作進行，但與其不同的加了 [`__attribute__((always_inline))`](https://www.keil.com/support/man/docs/armcc/armcc_chr1359124974010.htm) :

```c
#include <stdio.h>
static inline void hello_world(const char *ms)   __attribute__((always_inline));
static inline void hello_world(const char *ms) {
	printf("hello world rust %s\n", ms);
}
use std::{ffi::CString, os::raw::c_char};

extern "C" {
	fn hello_world(ms : *const c_char); 
}
fn main() {
	let ms = CString::new("Hey~!").unwrap();
	unsafe { hello_world(ms.as_ptr()) };
}
```

最後輸出結果為：

```
> Executing task: cargo test -- --nocapture test <

   Compiling temp v0.1.0 (../temp)
error: linking with `cc` failed: exit code: 1
  |
  ...
  = note: /usr/bin/ld: /home/../temp/target/debug/deps/temp-cad8c4ff366bbe30.463jzhihe3bo3cfk.rcgu.o: in function `temp::test':
          /home/../temp/src/main.rs:182: undefined reference to `hello_world'
          collect2: error: ld returned 1 exit status
          

error: aborting due to previous error

error: could not compile `temp`

To learn more, run the command again with --verbose.
The terminal process "bash '-c', 'cargo test -- --nocapture test'" terminated with exit code: 101.

Terminal will be reused by tasks, press any key to close it.
```

> 版本為 `cargo 1.50.0 (f04e7fab7 2021-02-04)` 以及 `rustc 1.50.0 (cb75ad5db 2021-02-10)`

撇開上述問題，現在知道 Rust 是如何與 Linux 的 C 程式碼互動了，但實際上要如何使用也是問題。譬如，**文中也提到要花多少心力去包裝 C API 成原生 Rust 程式碼？** 在 [lkml.org - [PATCH 00/13\] [RFC] Rust support](https://lkml.org/lkml/2021/4/14/1023?fbclid=IwAR04IjpsgkDcrVlQfhmkJFxexp8ea5xAUjtrAbzpmINvkscXai0UThLINO0) 中有提到不要直接使用現有的 API ，其原因可以很明顯的從為何會使用 Rust 做開發得知。

> Secondly, modules written in Rust should never use the C kernel APIs directly. The whole point of using Rust in the kernel is that we develop safe abstractions so that modules are easier to reason about and, therefore, to review, refactor, etc.

但要包裝到何種程度？在 [linux-kernel-module-rust](https://github.com/fishinabarrel/linux-kernel-module-rust/) 當中，對於 kernel 與 userspace 之間的資料轉移 `copy_to_user()` 和 `copy_from_user()` 被包裝在 [`UserSlicePtr` type](https://github.com/fishinabarrel/linux-kernel-module-rust/blob/5f50db6300dbeacfeb8a5e6bc277dbef3f48e26c/src/user_ptr.rs) 之中。其呼叫會像是：

```
ser_buf.write(&kernel_buffer)?;
```

`?` 為 Rust 的 [error-handling](https://doc.rust-lang.org/edition-guide/rust-2018/error-handling-and-panics/the-question-mark-operator-for-easier-error-handling.html) 。簡單說，此種方法雖然貼近 Rust 原生撰寫方式也進而有了相對應的保護 (如 memory safety 等) ，但也會產生大量的工作量以及與原先 C 語言不盡相同的 API 。因此如果說包裝得過頭，會使兩個語言之間的開發者們造成困擾。而現今開發者們對於此想法也可以從 [lkml.org - [PATCH 00/13\] [RFC] Rust support](https://lkml.org/lkml/2021/4/14/1023?fbclid=IwAR04IjpsgkDcrVlQfhmkJFxexp8ea5xAUjtrAbzpmINvkscXai0UThLINO0) 中看到：

> \- The abstractions wrapping the kernel APIs. These live inside `rust/kernel/`. **The intention is to make these as safe as possible so that modules written in Rust require the smallest amount of `unsafe` code possible.**

這也是為什麼開頭就說了只在某些適合部份以 Rust 撰寫，因為有些程式碼根本沒有必要用 Rust 寫：

> **There was some agreement at the session that writing Rust wrappers will make sense for some of the most common and critical APIs, but that manually wrapping every kernel API would be infeasible and undesirable.** Thomas mentioned that Google is working on automatically generating idiomatic bindings to C++ code, and pondered whether the kernel could do something similar, perhaps building on top of existing sparse annotations or some new annotations added to the existing C to guide the binding generator.

並且也在 [lkml.org - [PATCH 00/13\] [RFC] Rust support](https://lkml.org/lkml/2021/4/14/1023?fbclid=IwAR04IjpsgkDcrVlQfhmkJFxexp8ea5xAUjtrAbzpmINvkscXai0UThLINO0) 開頭就特別強調 Rust 目前是使用在 kernel 周圍的 module 或 driver 上，且應該在這之上實際驗證是否有比原先以 C 語言的表現得更好。至於主要子系統如記憶體管理、排程等並不會以此重寫：

> Please note that the Rust support is intended to enable writing drivers and similar “leaf” modules in Rust, at least for the foreseeable future. In particular, we do not intend to rewrite the kernel core nor the major kernel subsystems (e.g. `kernel/`, `mm/`, `sched/`…). Instead, the Rust support is built on top of those.

### 架構支援

Rust 程式語言的編譯器 [rustc](https://rustc-dev-guide.rust-lang.org/overview.html) 是經由 [LLVM](https://en.wikipedia.org/wiki/LLVM) 產生程式碼。

> - We then begin what is vaguely called code generation or codegen.
>     - The code generation stage (codegen) is when higher level representations of source are turned into an executable binary. rustc uses LLVM for code generation. The first step is to convert the MIR to LLVM Intermediate Representation (LLVM IR). This is where the MIR is actually monomorphized, according to the list we created in the previous step.
>     - The LLVM IR is passed to LLVM, which does a lot more optimizations on it. It then emits machine code. It is basically assembly code with additional low-level types and annotations added. (e.g. an ELF object or wasm).
>     - The different libraries/binaries are linked together to produce the final binary.

而 Linux kernel 所支援的架構並沒有全部都支援 LLVM 後端，而就算有，也有些沒有支援 rustc 後端。

> Several people said that it would be acceptable to implement drivers in Rust that would never be used on the more obscure architectures anyway. Triplett suggested that adding Rust into the kernel would help drive increased architecture support for Rust, citing his experience with the Debian project. He mentioned that introducing Rust software into Debian helped to motivate enthusiasts and users of niche architectures to improve Rust support, and he expected that adding support to the kernel would have a similar effect. In particular, he was confident that any architecture with an LLVM backend would quickly be supported in rustc.

除了在架構上支援 Rust ，也有 project 如 [mrustc](https://github.com/thepowersgang/mrustc) 直接讓 Rust 以經由與 C 同樣的編譯器來進行編譯。

而除上述所列，也有 [gccrs](https://github.com/Rust-GCC/gccrs) 專案，正如其名此為 GCC 架構支援為目標。然而從文件開頭就可看到現今還在早期版本，並不能實際編譯 Rust 程式：

> **Please note, the compiler is in a very early stage and not usable yet for compiling real Rust programs.**
>
> gccrs is a full alternative implementation of the Rust language ontop of GCC with the goal to become fully upstream with the GNU toolchain.
>
> The origin of this project was a community effort several years ago where Rust was still at version 0.9; the language was subject to so much change that it became difficult for a community effort to play catch up. Now that the language is stable, it is an excellent time to create alternative compilers. The developers of the project are keen “Rustaceans” with a desire to give back to the Rust community and to learn what GCC is capable of when it comes to a modern language.

### LLVM-built Rust 和 GCC-built kernel

如果真的要把 Rust 運用於 GCC 編譯的 Linux kernel 那麼是必要面對 GCC 與 LLVM 混用的困境。

> Greg Kroah-Hartman confirmed that the current kernel rule was that compatibility is only guaranteed if all object files in the kernel are built with the same compiler, using identical flags. **However, he also expressed comfort with linking LLVM-built Rust objects into a GCC-built kernel as long as the objects are built at the same time, with the appropriate options set, and the resulting configurations are fully tested.** He did not feel the need for any additional restrictions until and unless actual problems arise.

------

## [lwn.net](http://lwn.net/) - [Rust heads into the kernel?](https://lwn.net/Articles/853423/) / [Rust for Linux redux](https://lwn.net/Articles/862018/)

此兩篇細部探討了開發者們對於 Rust 使用於 Linux 中實際需要面臨問題，以及對於新語言加入 Linux 之必要性以及加入後的影響。例如 Rust 語言在呼叫 `BUG()` 使得核心崩潰； `panic!()` 破壞了 memory-safe ； Rust 標準函式庫在核心不夠完善 ； memory model 在未來可能與 C 語言不一致等。

並且在理論上因 `memory safety` 等特性，一般來說會比 C 語言有更好的保護以及效能提升，這些都還需要實際去證實。

> But the Binder “driver” is not really a good example to use for a few different reasons, Greg Kroah-Hartman said. It is missing a fairly large piece of functionality (binderfs) for one thing, but it also does little to help show how Rust will fit in with the rest of the kernel. As before, he strongly recommended working on something that would help clear up some of the questions that kernel developers have about Rust:
>
> > **Not to say that it doesn’t have its usages, but the interactions between binder and the rest of the kernel are very small and specific. Something that almost no one else will ever write again.**
> >
> > **Please work on a real driver to help prove, or disprove, that this all is going to be able to work properly. There are huge unanswered questions that need to be resolved that you will run into when you do such a thing.**

從 [rust for linux - mail list](https://lore.kernel.org/rust-for-linux/20210705043532.GA30964@1wt.eu/T/#e894c7ddfa66d4a11df5059470b56e2a2471bf138) 的一篇回信中也有提到：

> As Christoph said, and I and others have said before, binder is in no
> way shape or form anything that resembles any sort of “driver” at all.
> It is a crazy IPC mechanism that is tacked onto the side of the kernel.
> Not to say that it doesn’t have its usages, but the interactions between
> binder and the rest of the kernel are very small and specific.
> Something that almost no one else will ever write again.
>
> Please work on a real driver to help prove, or disprove, that this all
> is going to be able to work properly. There are huge unanswered
> questions that need to be resolved that you will run into when you do
> such a thing.

除此之外，也有人對於 `unsafe block` 能否真的完全限制那些 unsafe 操作感到疑問，例如後篇下方討論有講到的 `unsafe block` 對其他 `safe block` 的影響。

> Aside from things related to the unsafe keyword itself (e.g. calling unsafe functions) I believe Rust only lets you do 3 things in unsafe code that you weren’t anyway allowed to do in safe code. You can dereference a raw pointer. You can access fields in a union. You can mutate statics. That’s all. Those are indeed potential footguns (who knows what, if anything, that raw pointer is pointing at; maybe that field in the union isn’t the one with valid data in it; hey, another thread is reading that static variable, changing it seems like a bad idea) but that’s a much smaller set than you might have expected.

而這些 `unsafe block` 的影響範圍，也可能不單只是一小部分：

> You sound just like von Jolly. Who said to Max Planck back in the 1878: in this field, almost everything is already discovered, and all that remains is to fill a few unimportant holes. **Same with Rust: yes, unsafe gives you a tiny number of superpowers, but abuse of these superpowers can easily cause effects which will infect the whole program and would cause undefined behaviors god know where.**
>
> That’s why is important to see how many unsafe blocks real drivers would requite and what kind of code would you need to include in such unsafe blocks.
>
> Very few unsafe blocks (but tricky and complex to use ones) may herald the return of majority of C-style footguns. Just the ability to transmute to the value which doesn’t exist in enum may lead to very misterious crashes in completely superficially unrelated code, e.g.

以下程式碼為例：

```rust
#[derive(Debug)]
#[repr(i8)]
pub enum Foo {
    A = 0,
    B = 1,
    C = 2,
    D = 3
}

fn gimme_a_enum() -> Foo {
  unsafe { std::mem::transmute::<i8, Foo>(4) }
}

/// - debug build get: 
///   None 
///   None
/// - optimized build get:
///   Some
///   None
pub fn main() { 
    let var: Foo = gimme_a_enum();
    let var: Option<Foo> = Some(var);
    match var {
        Some(_) => println!("Some"),
        None => println!("None")
    }
    println!("{:?}", var)
 }
```

在 [debug build](https://godbolt.org/z/r9qxs3Y8Y) 和 [optimized build](https://godbolt.org/z/8zPc9onGc) 下，因 `unsafe block` 裡的操作導致其他 `safe block` 會有不同結果。

在前篇也提及對於 Rust 提供的功能是否足夠有說服力讓 Linux 接納，對此有一些開發者倒是希望以 C 語言來提供 Rust 所具有的特徵，例如 [ownership](https://doc.rust-lang.org/book/ch04-01-what-is-ownership.html) 。

而前篇在下方留言區也有實際提到對於不同語法的提供的問題，例如上述所提到的 error-handling 在 C 語言中會是：

```c
r = func();
if (r < 0)
goto some_label;
...
some_label:
// chain of kfree and unlocks here
return r;
```

Rust ：

```rust
func()?;
```

其中 Rust 的 error-handling 具體來說是做了哪些事，不同狀態下的 error 又有需要哪種型態，如 `Result<T, Errno>` 不同型態的 `T` 有可能在特定情況下會與 `Errno` 的數值重疊到，都是需要考量。

------

## [linux kernel module rust](https://github.com/fishinabarrel/linux-kernel-module-rust/)

根據 Github 的說明，開發者們在 kernel headers 上使用 [bingen](https://github.com/rust-lang/rust-bindgen) 進行 Rust [FFI binding](https://en.wikipedia.org/wiki/Foreign_function_interface) ，因此得以利用 kernel 自己的 build system 去指定適當的 [CFLAGS](https://en.wikipedia.org/wiki/CFLAGS) 。在之這後開發者們再對這些 types 撰寫 safe bindings 。

[**gentoo linux - GCC optimization**](https://wiki.gentoo.org/wiki/GCC_optimization)

**What are CFLAGS and CXXFLAGS?**
CFLAGS and CXXFLAGS are among the environment variables conventionally used to specify compiler options to a build system when compiling C and C++ code. While these variables are not standardized, their use is essentially ubiquitous and any correctly written build should understand these for passing extra or custom options when it invokes the compiler. See the GNU make info page for a list of some of the commonly used variables in this category.

Because such a large proportion of the packages that make up most Gentoo systems are written in C and C++, these are two variables administrators will definitely want to set correctly as they will greatly influence the way much of the system is built.

They can be used to decrease the amount of debug messages for a program, increase error warning levels and, of course, to optimize the code produced. The GCC manual maintains a complete list of available options and their purposes.

**How are they used?**
Normally, CFLAGS and CXXFLAGS would be set in the environment when invoking a configure script or with makefiles generated by the automake program. In Gentoo-based systems, set the CFLAGS and CXXFLAGS variables in /etc/portage/make.conf. Variables set in this file will be exported to the environment of programs invoked by portage such that all packages will be compiled using these options as a base.

每個 kernel module 會在 `staticlib` crate 中，而這會產生 `.a` 檔。之後將這物件傳給 kernel 的 build system 去連結 `.ko`。

> The kernel is inherently multi-threaded: kernel resources can be accessed from multiple userspace processes at once, which causes multiple threads of execution inside the kernel to handle system calls (or interrupts). Therefore, the KernelModule type is [Sync](https://doc.rust-lang.org/book/ch16-04-extensible-concurrency-sync-and-send.html), so all data shared by a kernel module must be safe to access concurrently (such as by implementing locking).

在 2021 年 7 月有人提出以 Rust 撰寫的 module ： [A GPIO driver in Rust](https://lwn.net/Articles/863459/)

------

## [Rust library for building and running BPF/eBPF modules](https://github.com/foniod/redbpf)

> The redbpf project is a collection of tools and libraries to build eBPF programs using Rust. It includes:

- **redbpf** - a user space library that can be used to load eBPF programs
- **redbpf-probes** - an idiomatic Rust API to write eBPF programs that can be loaded by the linux kernel
- **redbpf-macros** - companion crate to redbpf-probes which provides convenient procedural macros useful when writing eBPF programs
- **cargo-bpf** - a cargo subcommand for creating, building and debugging eBPF programs

從 GitHub 的說明可以看到此支援最早到 4.19 版本。

> The minimum kernel version supported is 4.19. Kernel headers are discovered automatically, or you can use the KERNEL_SOURCE environment variable to point to a specific location. Building against a linux source tree is supported as long as you run make prepare first.

------

## TODO [Rust for Linux](https://github.com/Rust-for-Linux)

[What is rustdoc?](https://doc.rust-lang.org/rustdoc/)

[Rust for linux](https://rust-for-linux.github.io/docs/kernel/index.html) 可以在網站上看到目前 Rust 開發者們在核心當中的進展，比如前敘的 [`alloc`](https://rust-for-linux.github.io/docs/alloc/index.html) 問題已予以核心自己的實作、[紅黑樹](https://rust-for-linux.github.io/docs/kernel/rbtree/index.html)等。

**2021 年 9 月， Rust for Linux 的其中一位維護者 Miguel Ojeda 的演講**

- [LVC21F-317 Rust for Linux](https://youtu.be/VlSkZYBeK8Q) / [PDF](https://static.linaro.org/connect/lvc21f/presentations/LVC21F-317.pdf)

------

## Reference

[Supporting Linux kernel development in Rust](https://lwn.net/Articles/829858/)
[lkml.org - [PATCH 00/13\] [RFC] Rust support](https://lkml.org/lkml/2021/4/14/1023?fbclid=IwAR04IjpsgkDcrVlQfhmkJFxexp8ea5xAUjtrAbzpmINvkscXai0UThLINO0)
[lwn.net - Rust heads into the kernel?](https://lwn.net/Articles/853423/)
[Writing Linux Kernel Modules in Safe Rust - Geoffrey Thomas & Alex Gaynor](https://www.youtube.com/watch?v=RyY01fRyGhM)
[GitHub - fishinabarrel/linux-kernel-module-rust](https://github.com/fishinabarrel/linux-kernel-module-rust/)
[GitHub - foniod/redbpf](https://github.com/foniod/redbpf)