use super::schema::posts;

#[derive(Queryable, Serialize, Debug)]
pub struct Post {
    pub id: i32,
    pub author: String,
    pub title: String,
    pub body: String,
}

#[derive(Insertable)]
#[table_name = "posts"]
pub struct NewPost<'a> {
    pub author: &'a str,
    pub title: &'a str,
    pub body: &'a str,
}
