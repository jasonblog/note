import os
import threading
import Queue
import socket
import select
import urllib
import time
import logging
import sys
R={}
W={}
X={}
S={}
EOL1=b'\n\n'
EOL2=b'\n\r\n'
cwd=os.getcwd()+"/www"
LOG=os.getcwd()+"/log.txt"
STATUS=os.getcwd()+"/status/"
HTTPIP="127.0.0.1"
HTTPLISTEN=80

def readconf():
    global cwd,LOG,HTTPIP,HTTPLISTEN,STATUS,R,W,X,S
    try:
        f=open("myhttps.conf","r")
        conf=f.read()
        f.close()
    except:
        pass
    try:
        PATHconf=conf.split("PATH{")[-1]
        PATHconf=PATHconf.split("}")[0]
        PATHconf=PATHconf.split("\n")[1:-1]
        for i in PATHconf:
            name=i.split(":")[0]
            path=i.split(":")[1]
            if name == "CWD":
                cwd=path
            elif name == "LOG":
                LOG=path
            elif name == "STATUS":
                STATUS=path
    except Exception,e:
        logging.error(e)
    try:
        HTTPconf=conf.split("HTTP{")[-1]
        HTTPconf=HTTPconf.split("}")[0]
        HTTPconf=HTTPconf.split("\n")[1:-1]
        for i in HTTPconf:
            name=i.split(":")[0]
            path=i.split(":")[1]
            if name == "IP":
                HTTPIP=path
            elif name == "LISTEN":
                HTTPLISTEN=int(path)

    except Exception,e:
        logging.error(e)

    try:
        Rconf=conf.split("R{")[-1]
        Rconf=Rconf.split("}")[0]
        Rconf=Rconf.split("\n")[1:-1]
        Wconf=conf.split("W{")[-1]
        Wconf=Wconf.split("}")[0]
        Wconf=Wconf.split("\n")[1:-1]
        Xconf=conf.split("X{")[-1]
        Xconf=Xconf.split("}")[0]
        Xconf=Xconf.split("\n")[1:-1]
        Sconf=conf.split("S{")[-1]
        Sconf=Sconf.split("}")[0]
        Sconf=Sconf.split("\n")[1:-1]
    except:
        return

    #R
    for i in Rconf:
        try:
            path=i.split(":")[0]
            types=i.split(":")[1]
            types=types.split(" ")
            R[path]=types
        except:
            pass
    #W
    for i in Wconf:
        try:
            path=i.split(":")[0]
            types=i.split(":")[1]
            types=types.split(" ")
            W[path]=types
        except:
            pass
    #X
    for i in Xconf:
        try:
            path=i.split(":")[0]
            types=i.split(":")[1]
            types=types.split(" ")
            X[path]=types
        except:
            pass

    #S
    S[1]=[]
    S[2]=[]
    S[4]=[]
    for i in Sconf:
        try:
            quan=i.split(":")[0]
            names=i.split(":")[1]
            names=names.split(" ")
            if quan == "r":
                S[4]=names
            elif quan == "w":
                S[2]=names
            elif quan == "x":
                S[1]=names
        except:
            pass


def deal200head(path):
    head="HTTP/1.1 200 OK\r\n"
    head+="Date:"+time.strftime("%Y-%m-%d %H:%M:%S", time.localtime()) +"\r\n"
    head+="Server:server of qiutian\r\n"
    head+="Content-Length:"+str(os.path.getsize(cwd+path))+"\r\n"
    return head
def deal400():
    try:
        f=open(STATUS+"400.html","r")
    except:
        if os.path.exists(STATUS+"400.html"):
            return deal403()
        else:
            return deal404()
    ff=f.read()
    f.close()
    head="HTTP/1.1 400 Bad Request\r\n"
    head+="Date:"+time.strftime("%Y-%m-%d %H:%M:%S", time.localtime()) +"\r\n"
    head+="Server:server of qiutian\r\n"
    head+="Content-Length:"+str(os.path.getsize(STATUS+"400.html"))+"\r\n"
    head+="Content-Type: text/html\r\n\r\n"
    return head+ff
def deal404():
    try:
        f=open(STATUS+"404.html","r")
    except:
        print STATUS+"404.html"
        if os.path.exists(STATUS+"404.html"):
            return deal403()
        else:
            return deal404()
    ff=f.read()
    f.close()
    head="HTTP/1.1 404 Not Found\r\n"
    head+="Date:"+time.strftime("%Y-%m-%d %H:%M:%S", time.localtime()) +"\r\n"
    head+="Server:server of qiutian\r\n"
    head+="Content-Length:"+str(os.path.getsize(STATUS+"404.html"))+"\r\n"
    head+="Content-Type: text/html\r\n\r\n"
    return head+ff
def deal403():
    try:
        f=open(STATUS+"403.html","r")
    except:
        if os.path.exists(STATUS+"403.html"):
            return deal403()
        else:
            return deal404()
    ff=f.read()
    f.close()
    head="HTTP/1.1 403 Forbidden\r\n"
    head+="Date:"+time.strftime("%Y-%m-%d %H:%M:%S", time.localtime()) +"\r\n"
    head+="Server:server of qiutian\r\n"
    head+="Content-Length:"+str(os.path.getsize(STATUS+"403.html"))+"\r\n"
    head+="Content-Type: text/html\r\n\r\n"
    return head+ff
def deal405():
    try:
        f=open(STATUS+"405.html","r")
    except:
        if os.path.exists(STATUS+"405.html"):
            return deal403()
        else:
            return deal404()
    ff=f.read()
    f.close()
    head="HTTP/1.1 405 Method Not Allowed\r\n"
    head+="Date:"+time.strftime("%Y-%m-%d %H:%M:%S", time.localtime()) +"\r\n"
    head+="Server:server of qiutian\r\n"
    head+="Content-Length:"+str(os.path.getsize(STATUS+"405.html"))+"\r\n"
    head+="Content-Type: text/html\r\n\r\n"
    return head+ff
def deal501():
    try:
        f=open(STATUS+"501.html","r")
    except:
        if os.path.exists(STATUS+"501.html"):
            return deal403()
        else:
            return deal404()
    ff=f.read()
    f.close()
    head="HTTP/1.1 501 Not Implemented\r\n"
    head+="Date:"+time.strftime("%Y-%m-%d %H:%M:%S", time.localtime()) +"\r\n"
    head+="Server:server of qiutian\r\n"
    head+="Content-Length:"+str(os.path.getsize(STATUS+"501.html"))+"\r\n"
    head+="Content-Type: text/html\r\n\r\n"
    return head+ff

def dealdir(path,method="GET"):
    ff=os.popen("python " +"dealdir.py "+cwd+' '+path).read()
    head="HTTP/1.1 200 OK\r\n"
    head+="Date:"+time.strftime("%Y-%m-%d %H:%M:%S", time.localtime()) +"\r\n"
    head+="Server:server of qiutian\r\n"
    head+="Content-Length:"+str(len(ff))+"\r\n"
    head+="Content-Type: text/html\r\n\r\n"
    if method == "HEAD":
        return head
    else:
        return head+ff
def dealnone(path,method="GET"):
    print "dead"
    try:
        if path in S[4]:
            f=open(cwd+path,"r")
        elif "none" in R[os.path.dirname(path)]:
            f=open(cwd+path,"r")
        else:
            return deal403()
    except:
        return deal403()
    ff=f.read()
    f.close()
    head=deal200head(path)
    head+="Content-Type: text/none\r\n\r\n"
    if method == "HEAD":
        return head
    else:
        return head+ff
def dealhtml(path,method="GET"):
    head=deal200head(path)
    head+="Content-Type: text/html\r\n\r\n"
    try:
        if path in S[4]:
            f=open(cwd+path,"r")
        elif "html" in R[os.path.dirname(path)]:
            f=open(cwd+path,"r")
        else:
            return deal403()
    except:
        return deal403()
    ff=f.read()
    f.close()
    if method == "HEAD":
        return head
    else:
        return head+ff
def dealphp(path,c,method="GET"):
    try:
        if path in S[4] and path in S[1]:
            ff.popen("php "+cwd+path+" "+c).read()
        elif "php" in R[os.path.dirname(path)] and "php" in X[os.path.dirname(path)]:
            ff.popen("php "+cwd+path+" "+c).read()
        else:
            return deal403()
    except:
        return deal403()
    head=deal200head()
    head+="Content-Type: text/html\r\n\r\n"
    if method == "HEAD":
        return head
    else:
        return head+ff
def dealcgi(path,types,cgican,method="GET"):
    if types == "py":
        try:
            if path in S[4] and path in S[1]:
                ff=os.popen("python "+cwd+path+" "+cgican).read()
            elif "py" in R[os.path.dirname(path)] and "py" in X[os.path.dirname(path)]:
                ff=os.popen("python "+cwd+path+" "+cgican).read()
            else:
                return deal403()
        except:
            return deal403()
    elif types == "perl":
        try:
            if path in S[4] and path in S[1]:
                ff=os.popen("perl "+cwd+path+" "+cgican).read()
            elif "perl" in R[os.path.dirname(path)] and "perl" in X[os.path.dirname(path)]:
                ff=os.popen("perl "+cwd+path+" "+cgican).read()
            else:
                return deal403()
        except:
            return deal403()
    elif types == "php":
        try:
            if path in S[4] and path in S[1]:
                ff=os.popen("php "+cwd+path+" "+cgican).read()
            elif "php" in R[os.path.dirname(path)] and "php" in X[os.path.dirname(path)]:
                ff=os.popen("php "+cwd+path+" "+cgican).read()
            else:
                return deal403()
        except:
            return deal403()
    else:
        try:
            if path in S[4] and path in S[1]:
                ff=os.popen("."+cwd+path+" "+cgican).read()
            elif types in R[os.path.dirname(path)] and types in X[os.path.dirname(path)]:
                ff=os.popen("."+cwd+path+" "+cgican).read()
            else:
                return deal403()
        except Exception,e:
            logging.error(e)
            f=open(cwd+path,"r")
            ff=f.read()
            f.close()
    head=deal200head(path)
    head+="Content-Type: text/html\r\n\r\n"
    if method == "HEAD":
        return head
    else:
        return head+ff

def dealresponse(request):
    method=request.split(' ')[0]
    try:
        url=request.split(' ')[1]
        if url[0]!='/' or url[0:3]=="/..":
            return deal400()
        path=url.split('?')[0]
    except Exception,e:
        logging.error(e)
        return deal400()
    if method!="GET"and"POST"and"HEAD"and"OPTIONS"and"TRACE":
        return deal501()
    if url == '/':
        try:
            if "/index.html" in S[4]:
                f=open(cwd+"/index.html")
            elif "html" in R["/"]:
                f=open(cwd+"/index.html")
            else:
                return deal403()
        except:
            return deal403()
        ff=f.read()
        f.close()
        head=deal200head("/index.html")
        head+="Content-Type: text/html\r\n\r\n"
        return head+ff

    #WHEN THR WAY IS "GET" or "HEAD":
    if method == "GET" or "HEAD":
        if os.path.exists(cwd+path):
            if os.path.isdir(cwd+path) and path!='/':
                return dealdir(path,method)
            else:
                c = ' '
                cgican = ' '
                if len(url.split('?'))>1 and url.split('?')[1]!='':
                    can=url.split('?')[1]
                    cgican=can.replace("&","\&")
                    cann=can.split('&')
                    a=[i.split('=')[1] for i in cann]
                    i=0
                    while i<len(a):
                        c=c+' '+a[i]
                        i=i+1
                filename=path.split('/')[-1]
                if len(filename.split('.'))>1:
                    types=filename.split('.')[-1]
                else:
                    types="none"
                if len(path)>9 and path[0:9] == "/cgi-bin/":
                    return dealcgi(path,types,cgican,method)
                elif types == "none":
                    return dealnone(path,method)
                elif types == 'html':
                    return dealhtml(path,method)
                elif types == 'php':
                    return dealphp(path,c,method)
                else:
                    try:
                        if path in S[4]:
                            f=open(cwd+path,"r")
                        elif types in R[os.path.dirname(path)]:
                            f=open(cwd+path,"r")
                        else:
                            return deal403()
                    except:
                        return deal403()
                    ff=f.read()
                    f.close()
                    return ff
        else:
            return deal404()

    #WHEN THE WAY IS "POST":
    if method == "POST":
        if os.path.exists(cwd+path):
            if os.path.isdir(cwd+path) and path!='/':
                return dealdir(path)
            else:
                c = ' '
                cgican = ' '
                if len(request.split('\n\r\n'))>1 and request.split('\n\r\n')[1]!='':
                    can=request.split('\n\r\n')[1]
                    cgican=can.replace("&","\&")
                    cann=can.split('&')
                    a=[i.split('=')[1] for i in cann]
                    i=0
                    while i<len(a):
                        c=c+' '+a[i]
                        i=i+1

                filename=path.split('/')[-1]
                if len(filename.split('.'))>1:
                    types=filename.split('.')[-1]
                    print filename.split('.')
                else:
                    types="none"
                if len(path)>9 and path[0:9] == "/cgi-bin/":
                    return dealcgi(path,types,cgican)
                elif types == "none":
                    return dealnone(path)
                elif types == 'html':
                    return dealhtml(path)
                elif types == 'php':
                    return dealphp(path,c)
                else:
                    try:
                        if path in S[4]:
                            f=open(cwd+path,"r")
                        elif types in R[os.path.dirname(path)]:
                            f=open(cwd+path,"r")
                        else:
                            return deal403()
                    except:
                        return deal403()
                    ff=f.read()
                    f.close()
                    return ff
        else:
            return deal404()

    #WHEN THE WAY IS "DELETE":
    if method == "DELETE":
        if os.path.exists(cwd+path):
            try:
                if os.path.isdir(cwd+path):
                    #os.system("rm -Rf "+cwd+path)
                    return deal403()
                else:
                    filename=path.split('/')[-1]
                    if len(filename.split('.'))>1:
                        types=filename.split('.')[-1]
                    else:
                        types="none"
                    if path in S[2]:
                        os.system("rm -f "+cwd+path)
                    elif types in R[os.path.dirname(path)]:
                        os.system("rm -f "+cwd+path)
                    else:
                        return deal403()
            except Exception,e:
                logging.error(e)
                return deal405()
        else:
            return deal404()

    #WHEN THE WAY IS "TRACE":
    if method == "TRACE":
        head=deal200head()
        head+="Content-Type: message/http"
        return head+request




class Thread(threading.Thread):
    def __init__(self,queue):
        threading.Thread.__init__(self)
        self._queue=queue
    def run(self):
        while True:
            filenoo,eventt=self._queue.get()
            ##WHEN THE WAY IS HTTP:
            if linkway[filenoo] == "http" or False:
                if eventt & select.EPOLLMSG:
                    print 'in'
                    #while True:
                    try:
                        aa=connections[filenoo].recv(1024)
                    except:
                        pass
                    httprequests[filenoo] += aa
                    print('-'*40 + '\n' + httprequests[filenoo])
                    if EOL1 in httprequests[filenoo] or EOL2 in httprequests[filenoo]:
                        print('-'*40 + '\n' + httprequests[filenoo])
                        try:
                            httprespones[filenoo]=dealresponse(httprequests[filenoo])
                            epoll.modify(filenoo,select.EPOLLOUT)
                        except Exception,e:
                            print "fopenerror"
                            logging.error(e)
                            httprespones[filenoo]=''
                    else:
                        if httprequests[filenoo]=='' or aa == '' or EOL1 in aa or EOL2 in aa:
                            epoll.modify(filenoo,0)
                        else:
                            try:
                                epoll.modify(filenoo,select.EPOLLIN)
                            except:
                                pass

                elif eventt & select.EPOLLPRI:
                    print 'o'
                    #while True:
                    try:
                        byteswritten[filenoo] = connections[filenoo].send(httprespones[filenoo])
                    except:
                        pass
                    httprespones[filenoo] = httprespones[filenoo][byteswritten[filenoo]:]
                    if len(httprespones[filenoo]) == 0:
                        try:
                            epoll.modify(filenoo, 0)
                        except:
                            pass
                        try:
                            connections[filenoo].shutdown(socket.SHUT_RDWR)
                        except:
                            pass
                    else:
                        try:
                            epoll.modify(filenoo,select.EPOLLOUT)
                        except:
                            pass
                self._queue.task_done()





if __name__=="__main__":
    readconf()
    logging.basicConfig(filename = os.path.join(os.getcwd(), LOG), level = logging.ERROR)
    queue=Queue.Queue()
    for i in range(100):
        t=Thread(queue)
        t.setDaemon(True)
        t.start()
    print "thread finished"
    serversocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    serversocket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    serversocket.bind((HTTPIP, HTTPLISTEN))
    serversocket.listen(20000)
    #serversocket.setblocking(0)
    epoll = select.epoll()
    epoll.register(serversocket.fileno(), select.EPOLLIN)
    try:
        connections={}
        httprequests={}
        httprespones={}
        connstream={}
        linkway={}
        byteswritten={}
        while True:
            events=epoll.poll(1)
            for fileno,event in events:
                if event & select.EPOLLPRI:
                    pass
                elif event & select.EPOLLMSG:
                    pass
                elif fileno == serversocket.fileno():
                    #print 'a'
                    connection,address=serversocket.accept()
                   # connection.setblocking(0)
                    epoll.register(connection.fileno(),select.EPOLLIN)
                    linkway[connection.fileno()]="http"
                    connections[connection.fileno()] = connection
                    httprequests[connection.fileno()] = b''
                    httprespones[connection.fileno()] = b''
                elif event & select.EPOLLIN:
                    #print 'b'
                    epoll.modify(fileno,select.EPOLLMSG)
                    queue.put((fileno,select.EPOLLMSG))
                elif event & select.EPOLLOUT:
                    #print 'c'
                    epoll.modify(fileno,select.EPOLLPRI)
                    queue.put((fileno,select.EPOLLPRI))
                elif event & select.EPOLLHUP:
                    #print 'd'
                    epoll.unregister(fileno)
                    connections[fileno].close()
                    del connections[fileno]
                    del linkway[fileno]
    finally:
        print 'e'
        epoll.close()
