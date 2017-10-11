import os,sys,time
cwd=sys.argv[1]
path=sys.argv[2]
ff=os.popen("ls"+" "+cwd+path).read()
ff=ff.split("\n")[0:-1]
print '''<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 3.2 Final//EN">'''
print "<html>"
print "<head>"
print "<title>Index of "+str(path)+"</title>"
print "</head>"
print "<body>"
print "<h1>Index of "+str(path)+"</h1>"
print "<table>"
print '''<tr><th valign="top"></th><th><a href="?C=N;O=D">Name</a></th><th><a href="?C=M;O=A">Last modified</a></th><th><a href="?C=S;O=A">Size</a></th><th><a href="?C=D;O=A">Description</a></th></tr>'''
print '''<tr><th colspan="5"><hr></th></tr>'''
if path!='/':
    print '''<tr><td valign="top"></td><td><a href="..">Parent Directory</a>       </td><td>&nbsp;</td><td align="right">  - </td><td>&nbsp;</td></tr>'''

for i in ff:
    if os.path.isdir(cwd+path+'/'+i):
        name=i+'/'
        size='-'
    else:
        name=i
        size=os.path.getsize(cwd+path+'/'+i)
    times=os.path.getmtime(cwd+path+'/'+i)
    timess=time.strftime("%Y-%m-%d %H:%M:%S", time.localtime(times))
    uuu=path+'/'+str(name)
    uuu=uuu.replace('//','/')
    print '''<tr><td valign="top"></td><td><a href='''+uuu+'>'+name+'''</a></td><td align="right">'''+str(timess)+'''</td><td align="right">'''+str(size)+'''</td><td>&nbsp;</td></tr>'''
print '''<tr><th colspan="5"><hr></th></tr>'''
print "</table>"
print "</body></html>"
