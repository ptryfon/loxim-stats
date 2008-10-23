mvn -o install -Dmaven.test.skip=true

(cd core
mvn exec:java -Dlang=cpp
cd target/cpp-result/protocol
make
)
