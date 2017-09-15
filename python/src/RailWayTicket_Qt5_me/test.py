import requests
req = requests.get('http://railway.hinet.net/ImageOut.jsp')
print(req.status_code)
print(req.status_code is not requests.codes.ok)