echo "Sprawdzanie dostepnosci biblioteki Xerces-C++"
g++ -lxerces-c test_xerces.cpp -o test_xerces
if [ -e test_xerces ]
then
	rm -f test_xerces
	echo "Xerces-C++ jest dostepny" 
else
	echo "Xerces-C++ nie jest dostepny - pomijam kompilacje modulu XMLIO"
fi

