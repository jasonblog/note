#headfile=$(shell rpm -ql `rpm -qa | grep "java.*-openjdk-devel"` | grep 'include/jni.h')
#headdir=$(shell dirname $(headfile))
#@echo ${headdir}

all:
	javac HelloWorld.java
	gcc -o jvm jvm.c                                                                         \
		-I/usr/lib/jvm/java-1.8.0-openjdk-1.8.0.131-3.b12.el7_3.x86_64/include               \
		-I/usr/lib/jvm/java-1.8.0-openjdk-1.8.0.131-3.b12.el7_3.x86_64/include/linux         \
		-L/usr/lib/jvm/java-1.8.0-openjdk-1.8.0.131-3.b12.el7_3.x86_64/jre/lib/amd64/server/ \
		-ljvm

clean:
	rm -rf jvm *.class
