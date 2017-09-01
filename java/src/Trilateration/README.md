javac *.java -cp ../jars/commons-math3-3.6.1.jar:../jars/junit-4.12.jar -d ../classes/

cd ../classes

java  -cp .:../jars/commons-math3-3.6.1.jar:../jars/junit-4.12.jar  test
