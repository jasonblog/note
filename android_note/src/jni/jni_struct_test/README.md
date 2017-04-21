1. javac StructPass.java
2. javah -jni  StructPass
3. gcc -fPIC -Wall -I /usr/lib/jvm/java-8-openjdk-amd64/include/ -I /usr/lib/jvm/java-8-openjdk-amd64/include/linux/ -shared -o libjni.so StructPass.c
4. java -Djava.library.path=. StructPass
