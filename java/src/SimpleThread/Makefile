JFLAGS = -g -cp ./classes/ -d ./classes
JC = javac
JVM= java
CLASSESDIR = classes
RM = rm -rf 
SIMPLETHREAD = SimpleThread

.SUFFIXES: .java .class
.java.class:
	$(JC) $(JFLAGS) $*.java

CLASSES = \
	ChirpControllerDevice.java \
	SimpleThread.java \

default: dir classes

dir:
	mkdir -p $(CLASSESDIR)


classes: $(CLASSES:.java=.class)
	@- echo "Done Compiling!!"

run:
	$(JVM) -cp classes $(SIMPLETHREAD)


clean:
	$(RM) *.class classes
