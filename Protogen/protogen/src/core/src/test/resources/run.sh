#/bin/bash

#plik u�ywany w wygenerowanym example. W lokalizacji �r�d�owej - podane �cie�ki mog� nie pracowa�

cd ../bin
./protoGen.sh ../example/loxim2.xml ../example/result-cpp cpp ../example/over/cpp
./protoGen.sh ../example/loxim2.xml ../example/result-java java ../example/over/java
