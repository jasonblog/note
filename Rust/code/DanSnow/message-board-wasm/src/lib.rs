#[macro_use]
extern crate serde_derive;
extern crate console_error_panic_hook;
extern crate futures;
extern crate js_sys;
extern crate serde;
extern crate wasm_bindgen;
extern crate wasm_bindgen_futures;
extern crate web_sys;

use futures::Future;
use wasm_bindgen::{prelude::*, JsCast};
use wasm_bindgen_futures::{future_to_promise, JsFuture};

#[derive(Serialize, Deserialize)]
struct Post {
    author: String,
    title: String,
    body: String,
}

#[derive(Deserialize)]
struct Posts {
    posts: Vec<Post>,
}

fn render_posts(posts: Posts) -> Result<JsValue, JsValue> {
    let window = web_sys::window().unwrap();
    let document = window.document().unwrap();
    let fragment = document.create_document_fragment();
    let frag_node = AsRef::<web_sys::Node>::as_ref(&fragment);
    for post in posts.posts.iter().rev() {
        let card = document.create_element("div").unwrap();
        card.set_class_name("card");
        card.set_inner_html(&format!(
            r#"<div class="card-content">
            <p class="title is-5">{}</p>
            <p class="subtitle is-5">{}</p>
            <div class="content">
              {}
            </div>
          </div>"#,
            post.title, post.author, post.body
        ));
        frag_node.append_child(card.as_ref())?;
    }
    let container = document.query_selector(".posts")?.unwrap();
    container.set_inner_html("");
    AsRef::<web_sys::Node>::as_ref(&container).append_child(fragment.as_ref())?;
    Ok(JsValue::UNDEFINED)
}

fn fetch_posts(window: &web_sys::Window) {
    future_to_promise(
        JsFuture::from(window.fetch_with_str("/api/posts"))
            .and_then(|res: JsValue| res.dyn_into::<web_sys::Response>().unwrap().json())
            .and_then(|json: js_sys::Promise| JsFuture::from(json))
            .and_then(|json| render_posts(json.into_serde::<Posts>().unwrap())),
    );
}

fn handle_submit(event: web_sys::Event) {
    event.prevent_default();
    web_sys::console::log_1(&JsValue::from("In handle submit"));
    let form = event
        .target()
        .unwrap()
        .dyn_into::<web_sys::HtmlFormElement>()
        .unwrap();
    web_sys::console::log_1(form.as_ref());
    let collection = form.elements();
    web_sys::console::log_1(collection.as_ref());
    let author_el = collection
        .named_item("author")
        .unwrap()
        .dyn_into::<web_sys::HtmlInputElement>()
        .unwrap();
    let title_el = collection
        .named_item("title")
        .unwrap()
        .dyn_into::<web_sys::HtmlInputElement>()
        .unwrap();
    let body_el = collection
        .named_item("body")
        .unwrap()
        .dyn_into::<web_sys::HtmlTextAreaElement>()
        .unwrap();
    let author = author_el.value();
    let title = title_el.value();
    let body = body_el.value();
    author_el.set_value("");
    title_el.set_value("");
    body_el.set_value("");
    let data = JsValue::from_serde(&Post {
        author,
        title,
        body,
    })
    .unwrap();
    web_sys::console::log_1(&data);
    let req = web_sys::Request::new_with_str_and_init(
        "/api/posts/create",
        web_sys::RequestInit::new()
            .body(Some(js_sys::JSON::stringify(&data).unwrap().as_ref()))
            .method("POST"),
    )
    .unwrap();
    req.headers()
        .set("Content-Type", "application/json")
        .unwrap();
    let window = web_sys::window().unwrap();
    let cb = Closure::wrap(Box::new(|_| {
        let window = web_sys::window().unwrap();
        fetch_posts(&window);
    }) as Box<FnMut(_)>);
    window.fetch_with_request(&req).then(&cb);
    cb.forget();
}

#[wasm_bindgen]
pub fn main() -> Result<(), JsValue> {
    console_error_panic_hook::set_once();
    let window = web_sys::window().unwrap();
    let document = window.document().unwrap();
    let create_post = document.query_selector("#create-post")?.unwrap();
    web_sys::console::log_1(create_post.as_ref());

    let handler = Closure::wrap(Box::new(handle_submit) as Box<Fn(_)>);
    AsRef::<web_sys::EventTarget>::as_ref(&create_post)
        .add_event_listener_with_callback("submit", handler.as_ref().unchecked_ref())?;
    handler.forget();
    fetch_posts(&window);
    Ok(())
}
