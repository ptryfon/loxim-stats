mvn -o install -Dmaven.test.skip=true

(cd core
mvn exec:java -Dlang=java
cd target/java-result/
mvn clean install 
mvn -X -e exec:java -Dexec.classpathScope="test" -Dexec.mainClass="pl.edu.mimuw.loxim.protocol.tests.TestRunnerRec" &
pid=$!
sleep 3
mvn -X -e exec:java -Dexec.classpathScope="test" -Dexec.mainClass="pl.edu.mimuw.loxim.protocol.tests.TestRunnerSender"
wait $pid
)
