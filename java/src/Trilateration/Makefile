JFLAGS = -g -cp ./jars/commons-math3-3.6.1.jar:./jars/junit-4.12.jar:./classes/ -d ./classes
JC = javac 
JVM= java

CLASSESDIR = classes

.SUFFIXES: .java .class

.java.class:
	$(JC) $(JFLAGS) $*.java

CLASSES = \
		src/TrilaterationFunction.java \
		src/LinearLeastSquaresSolver.java \
		src/NonLinearLeastSquaresSolver.java \
		src/TrilaterationTest.java \
		src/test.java

TEST = test

default: dir classes

dir:
	mkdir -p $(CLASSESDIR) 

classes: $(CLASSES:.java=.class)

run: 
	$(JVM) -cp classes:./jars/commons-math3-3.6.1.jar:./jars/junit-4.12.jar $(TEST)

clean:
	$(RM) -rf ./classes
