import os,sys
word=sys.argv[1]
name=sys.argv[2]
if True:
    f=open("his.txt","a")
    f.write("<center><strong>"+name+":"+word+"</strong></center>")
    f.write('\n')
    f.close()

