#[macro_use]
extern crate diesel;
#[macro_use]
extern crate serde_derive;
#[macro_use]
extern crate serde_json;
#[macro_use]
extern crate tower_web;
extern crate dotenv;
extern crate futures;
extern crate serde;
extern crate tokio;

mod models;
mod schema;

use crate::models::{NewPost, Post};
use diesel::{prelude::*, sqlite::SqliteConnection};
use dotenv::dotenv;
use futures::Future;
use std::{env, io, path::PathBuf};
use tokio::fs::File;
use tower_web::ServiceBuilder;

fn establish_connection() -> SqliteConnection {
    let url = env::var("DATABASE_URL").expect("找不到資料庫位置");
    SqliteConnection::establish(&url).expect("連線失敗")
}

fn create_post(conn: &SqliteConnection, author: &str, title: &str, body: &str) {
    use self::schema::posts;

    let new_post = NewPost {
        author,
        title,
        body,
    };

    diesel::insert_into(posts::table)
        .values(&new_post)
        .execute(conn)
        .expect("新增貼文失敗");
}

fn list_posts(conn: &SqliteConnection) -> Vec<Post> {
    use self::schema::posts::dsl::*;

    posts.load::<Post>(conn).expect("取得貼文列表失敗")
}

fn get_post(conn: &SqliteConnection, id: i32) -> Post {
    use self::schema::posts::dsl::posts;

    posts.find(id).first(conn).expect("取得貼文失敗")
}

fn delete_post(conn: &SqliteConnection, id: i32) {
    use self::schema::posts::dsl::posts;

    diesel::delete(posts.find(id))
        .execute(conn)
        .expect("刪除貼文失敗");
}

#[derive(Clone, Debug)]
pub struct Service;

#[derive(Response, Debug)]
struct PostsResponse {
    posts: Vec<Post>,
}

#[derive(Extract, Debug)]
struct CreatePostRequest {
    author: String,
    title: String,
    body: String,
}

#[derive(Response, Debug)]
#[web(status = "201")]
struct CreatePostResponse {
    ok: bool,
}

#[derive(Response, Debug)]
struct PostResponse {
    post: Post,
}

impl_web! {
    impl Service {
        #[get("/")]
        #[content_type("html")]
        fn index(&self) -> impl Future<Item = File, Error = io::Error> + Send {
            let mut path = PathBuf::from(env!("CARGO_MANIFEST_DIR"));
            path.push("static");
            path.push("index.html");
            File::open(path)
        }

        #[get("/api/posts")]
        #[content_type("json")]
        fn list_posts(&self) -> Result<PostsResponse, ()> {
            let conn = establish_connection();
            Ok(PostsResponse {
                posts: list_posts(&conn)
            })
        }

        #[post("/api/posts/create")]
        #[content_type("json")]
        fn create_post(&self, body: CreatePostRequest) -> Result<CreatePostResponse, ()>{
            let conn = establish_connection();
            create_post(&conn, &body.author, &body.title, &body.body);
            Ok(CreatePostResponse { ok: true })
        }

        #[get("/api/posts/:id")]
        #[content_type("json")]
        fn get_post(&self, id: i32) -> Result<PostResponse, ()>{
            let conn = establish_connection();
            Ok(PostResponse { post: get_post(&conn, id) })
        }

        #[delete("/api/posts/:id")]
        fn delete_post(&self, id: i32) -> Result<serde_json::Value, ()>{
            let conn = establish_connection();
            delete_post(&conn, id);
            Ok(json!({
                "ok": true
            }))
        }
    }
}

fn main() {
    dotenv().ok();

    let addr = "127.0.0.1:8080".parse().unwrap();
    println!("Listen on {}", addr);
    ServiceBuilder::new()
        .resource(Service)
        .run(&addr)
        .expect("啟動 Server 失敗");
}
