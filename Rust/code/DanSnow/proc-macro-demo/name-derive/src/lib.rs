#[macro_use]
extern crate quote;
extern crate proc_macro;
extern crate proc_macro2;
extern crate syn;

use self::proc_macro::TokenStream;

#[proc_macro_derive(Name)]
pub fn name_derive(input: TokenStream) -> TokenStream {
    let ast = syn::parse(input).unwrap();
    impl_name(&ast).into()
}

fn impl_name(ast: &syn::DeriveInput) -> proc_macro2::TokenStream {
    let name = &ast.ident;
    let name_str = name.to_string();
    quote! {
        impl ::name::Name for #name {
            fn name() -> &'static str {
                #name_str
            }
        }
    }
}
