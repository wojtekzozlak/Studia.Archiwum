HADOOP_JAR=/home/mimuw/hadoop/hadoop-core-1.0.4.jar

all: src/BFS.class
	cd src && jar -cf BFS.jar $(HADOOP_JAR) *.class
	mv src/BFS.jar ./


%.class: %.java
	javac -classpath $(HADOOP_JAR) -sourcepath src $<

clean:
	rm src/*.class
