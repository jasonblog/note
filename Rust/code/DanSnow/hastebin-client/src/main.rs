#[macro_use]
extern crate serde_derive;
extern crate atty;
extern crate clap;
extern crate reqwest;
extern crate serde;

use atty::Stream;
use clap::{App, Arg};
use reqwest::{Body, Client, Url};
use std::{
    borrow::Cow,
    env,
    fs::{self, File},
    io::{stdin, Read},
    process::{self, Command},
};

const URL: &str = "https://hastebin.com";

#[derive(Clone, Debug, Deserialize)]
struct Data {
    key: String,
    #[serde(default)]
    message: Option<String>,
}

fn main() -> reqwest::Result<()> {
    let matches = App::new("haste-client")
        .author("DanSnow")
        .version("0.1.0")
        .arg(
            Arg::with_name("HOST")
                .short("h")
                .long("host")
                .takes_value(true),
        )
        .arg(Arg::with_name("RAW").long("raw"))
        .arg(Arg::with_name("FILE").index(1))
        .get_matches();
    let body = if let Some(file) = matches.value_of("FILE") {
        match File::open(file) {
            Ok(f) => Body::from(f),
            Err(err) => {
                eprintln!("開啟檔案失敗： {}", err);
                process::exit(1);
            }
        }
    } else if atty::is(Stream::Stdin) {
        let path = env::temp_dir().join("haste-client-tempfile");
        let mut child = Command::new("vim")
            .arg(path.as_os_str())
            .spawn()
            .expect("開啟 vim 失敗");
        child.wait().expect("等待 vim 結束失敗");
        let file = File::open(&path).expect("開啟暫存檔失敗");
        fs::remove_file(path).expect("刪除暫存檔失敗");
        Body::from(file)
    } else {
        let mut buf = String::new();
        stdin()
            .lock()
            .read_to_string(&mut buf)
            .expect("讀輸入失敗");
        Body::from(buf)
    };
    let base = Url::parse(
        &matches
            .value_of("HOST")
            .map(Cow::from)
            .or(env::var("HASTE_SERVER").ok().map(Cow::from))
            .unwrap_or(Cow::from(URL)),
    )
    .unwrap();
    let client = Client::new();
    let res = client
        .post(base.join("documents").unwrap())
        .body(body)
        .send()?
        .json::<Data>()?;
    let mut url = base;
    if matches.is_present("RAW") {
        url = url.join("raw/").unwrap();
    }
    println!("{}", url.join(&res.key).unwrap());
    Ok(())
}
