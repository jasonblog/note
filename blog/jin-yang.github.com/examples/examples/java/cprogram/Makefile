#headfile=$(shell rpm -ql `rpm -qa | grep "java.*-openjdk-devel"` | grep 'include/jni.h')
#headdir=$(shell dirname $(headfile))
#@echo ${headdir}
#
# javah HelloWorld

all:
	javac HelloWorld.java
	gcc -o libctest.so -fPIC -shared ctest.c -lc \
		-I/usr/lib/jvm/java-1.8.0-openjdk-1.8.0.131-3.b12.el7_3.x86_64/include \
		-I/usr/lib/jvm/java-1.8.0-openjdk-1.8.0.131-3.b12.el7_3.x86_64/include/linux

clean:
	rm -rf *.class *.so
