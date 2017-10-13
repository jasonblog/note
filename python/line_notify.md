# Line notify


```py
#coding:UTF-8
import requests

def main():
    url = "https://notify-api.line.me/api/notify"
    token = "8y8xXyyOgytkS0erZKIcL6ecGPDNLrLdOC8PIOCKGFF"
    headers = {"Authorization" : "Bearer "+ token}

    message =  'ここにメッセージを入れます'
    payload = {"message" :  message}
    files = {"imageFile": open("test.jpg", "rb")} 

    r = requests.post(url ,headers = headers ,params=payload, files=files)

if __name__ == '__main__':
    main()
```

