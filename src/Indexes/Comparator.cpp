#include <Indexes/Comparator.h>

#include <Indexes/ToStringBuilder.h>
#include <assert.h>
#include <sstream>
#include <QueryExecutor/QueryResult.h>

using namespace QExecutor;
namespace Indexes {

	
	//************************************************
	// Comparator
	//************************************************
	
	/*
	void Comparator::putPtr(int childNumber, Node* parent, Node* child) {
		if (childNumber == 0) {
			// skrajnie lewe dziecko
			parent->firstChild = child->id;
		} else {
			// trzeba zmiejszyc o 1 bo n-te NodeEntry ma n+1szy wskaznik  
			NodeEntry *entry = getItem(--childNumber, parent, entry);
			entry->child = child->id;
		}
	}
	*/
	
	bool Comparator::canTakeExtraKey(Node* node) {
		return node->left >= maxEntrySize();
	}
	/*
	void Comparator::putItem(I* itemPlace) {
		*entry = *rootEntry;
		putValue((char*)(item+1), sizeof(I));
		putKey(entry + 1, sizeof(RootEntry));
	}
	*/
	template <class I>
	int Comparator::compare (I* item, bool useLid) {
		lid_t lidDiff = useLid && rootEntry ? rootEntry->logicalID - item->logicalID : 0;
		return compare((char*)item, sizeof(I), lidDiff);
	}
	
	template <class I>
	string Comparator::keyToString (I* item) {
		return keyToString((char*)item, sizeof(I));
	}
	
	template <class I>
	bool Comparator::lessThan (I* item) {
		return compare(item, false) < 0;
	}
	
	template <class I>
	bool Comparator::equal (I* item) {
		return compare(item, false) == 0;
	}
	
	template <class I>
	bool Comparator::greaterThan (I* item) {
		return compare(item, true) > 0;
	}
	
	template <class I>
	bool Comparator::greaterOrEqual (I* item) {
		return compare(item, false) >= 0;
	}
	
	template <class I>
	bool Comparator::lessOrEqual (I* item) {
		return compare(item, false) <= 0;
	}
			
	template <class I>
	bool Comparator::infinityReached(I* item) {
		return false;
	}
	
	void Comparator::removeEntry(Node* node, RootEntry* entry) {  
		char* src = (char*) getNextItem(entry);
		long bytes2copy = node->afterAddr() - src;
		//node->left += getSpaceNeeded(entry);
		node->left += src - (char*)entry;
		memmove(entry, src, bytes2copy);
	}
	
	template <class I>
	I* Comparator::getNextItem(I* item) {
		return (I*) getNextItem(((char*) item), sizeof(I));
	}
	
	template <class I>
	I* Comparator::getItem(int itemNumber, Node* node, I* item) {
		return (I*) getItem(itemNumber, ((char*) (node + 1)), sizeof(I));
	}
	
	template <class I>
	nodeEntry_off_t Comparator::getSpaceNeeded(I* item) {
		return sizeof(I) + getSpaceNeeded();
	}
	
	void Comparator::putValue(RootEntry* where) {
		assert(rootEntry);
		*where = *rootEntry;
		putKey(where);
		//cout << keyToString((char*)where, sizeof(RootEntry)) << endl;
	}
	
	template <class I>
	void Comparator::putKey(I* where) {
		char* whereKey = (char*) where;
		whereKey += sizeof(I);
		putKey(whereKey, sizeof(I));;
	}
	
	nodeSize_t Comparator::constInit() {
		//return (MAX_NODE_CAPACITY - 3 * this->maxEntrySize() + 2) / 2;
		return (MAX_NODE_CAPACITY /2)  - 2* this->maxEntrySize();
	}
	
	Comparator::Comparator() : rootEntry(NULL) {};
	
	bool Comparator::isInitialized() {
		return hasValue && rootEntry;
	}
	
	size_t Comparator::normLen(size_t valLen) {
		return min(valLen, (size_t)MAX_STRING_LEN);
	}
	
	void Comparator::setEntry(RootEntry* entry) {
		this->rootEntry = entry;
	}
	
	RootEntry* Comparator::getEntry() {
		return rootEntry;
	}
	
	int Comparator::setValue(double value) {
		return EBadValue | ErrIndexes;
	}
	
	int Comparator::setValue(int value) {
		return EBadValue | ErrIndexes;
	}
	
	int Comparator::setValue(string value) {
		return EBadValue | ErrIndexes;
	}
	
	int Comparator::setValue(Store::DataValue* value) {
		Store::DataType type = value->getType();
		if (!((type == Store::Integer) || (type == Store::String) || (type == Store::Double))) {
			//typ nie jest indeksowany
			return EBadValue | ErrIndexes;
		}
		
		switch (type) {
			case Store::Integer:
				return setValue(value->getInt());
			case Store::String:
				return setValue(value->getString());
			case Store::Double:
				return setValue(value->getDouble());
			default:
			 	assert(false);
		}
		
	}
	
	int Comparator::setValue(QExecutor::QueryResult* tnode) {
		int err;
		while (true) {
			switch(tnode->type()) {
				case QueryResult::QINT:
					return setValue((dynamic_cast<QueryIntResult*>(tnode))->getValue());
				case QueryResult::QDOUBLE:
					return setValue((dynamic_cast<QueryDoubleResult*>(tnode))->getValue());
				case QueryResult::QSTRING:
					return setValue((dynamic_cast<QueryStringResult*>(tnode))->getValue());
				case QueryResult::QBAG: 
				case QueryResult::QSTRUCT:
				case QueryResult::QSEQUENCE:
					if (tnode->size() == 1) {
						if ((err = dynamic_cast<QueryContainerResult*>(tnode)->at(0, tnode))) {
							return err;
						}
						continue;
					}
				default:
					return EOperNotDefined | ErrIndexes;
			}
		}
	}
	
	string Comparator::toString() {
		return ToStringBuilder("Comparator", this)
			.append("rootEntry", rootEntry ? rootEntry->toString() : "NULL")
			.toString();
	}
	
	template<class I>
	string Comparator::nodeToString(Node* node, I* item) {
		I* after = (I*) node->afterAddr();
		I* entry = getItem(0, node, entry);
		int count = 0;
		stringstream content, all;
		all << node->headerToString();
		
		int i = 1;
		while(entry < after) {
			count++;
			content << "#" << i <<  ": klucz=" << keyToString(entry) << " wartosc=" << entry->toString() << "; ";
			entry = getNextItem(entry);
			i++;
		}
		all << "can take more=" << canTakeExtraKey(node) << " ilosc wpisow=" << count << " zawartosc: " << content.str() << "| nextEntry = " << entry << "; afterAddr = " << after << "|";
		string result = all.str();
		assert(entry == after);
		return result;
	}
	
	string Comparator::nodeToString(Node* node) {
		if (node->leaf) {
			RootEntry* e;
			return nodeToString(node, e);
		} else {
			NodeEntry* n;
			return nodeToString(node, n);
		}
	}
	
	template<class I>
	void Comparator::getValue(I* item) {
		getValue((char*)item, sizeof(I));
	}
	
	void Comparator::checkSize(Node* node, int additionalSize) {
		assert((node->size() + additionalSize >= MIN_NODE_SIZE_ALLOWED) && (node->size() + additionalSize <= MAX_NODE_CAPACITY));
	}
	
	Comparator* Comparator::deserialize(int type) {
		switch (type) {
			case STRING_C:
				return new StringComparator();
			case INT_C:
				return new IntComparator();
			case DOUBLE_C:
				return new DoubleComparator();
			default:
				//w bazie znajduja sie niewlasciwe dane
				return NULL;
		}
	}
	
	int Comparator::serialize() const {
		return getType();
	}
	
/*	
	template<class I>
	nodeSize_t Comparator::spaceForCopy(I* from) {
		distanceInNode(from, getNextItem(from)) - sizeof(I) + sizeof(NodeEntry);
	}
	
	template<class I>
	nodeSize_t Comparator::copyToParent(I* from, char* to) {
		nodeSize_t size = spaceForCopy(from);
		memcpy(to + sizeof(NodeEntry), ((char*)from) + sizeof(I) size);
		return size - 
	}
	*/
	
	//************************************************
	// StringComparator
	//************************************************
/*	
	StringComparator::StringComparator(char* value, RootEntry* entry) : Comparator(entry), value(value) {
		init();
	}
	*/
	StringComparator::StringComparator() {
		value = NULL;
		MIN_NODE_SIZE_ALLOWED = constInit();
	}
	
	StringComparator::~StringComparator() {
		if (value) {
			delete[] value;
		}
	}
	
	nodeEntry_off_t StringComparator::getSpaceNeeded() {
		return spaceNeeded;
	}
	
	int StringComparator::compare(char* item, unsigned int itemSize, lid_t lidDiff) {
		nodeEntry_off_t keySize = getKeySize(item, itemSize);
		int result = 0;
		if ((strSize == 0) && (keySize != 0)) {
			//zawartosc comparatora jest mniejsza niz zawartosc pamieci
			return -1;
		}
		if ((keySize == 0) && (strSize != 0)) {
			//zawartosc comparatora jest wieksza niz zawartosc pamieci
			return 1;
		}
		nodeEntry_off_t keyCmpSize = min(keySize, strSize); 
		result = strncmp (value, getKeyAddr(item, itemSize), keyCmpSize);
		if (result == 0) {
			//na tej czesci napisy sa takie same
			//jesli zawartsosc comparatora jest mniejsza to -k
			result = strSize - keySize;
		}
		return result ? result : lidDiff;
	}
	
	string StringComparator::keyToString(char* item, unsigned int itemSize) {
		char* addr = getKeyAddr(item, itemSize);
		nodeEntry_off_t size = getKeySize(item, itemSize); 
		return string(addr, size);
	}
	
	char* StringComparator::getKeyAddr(char* item, unsigned int itemSize) {
		//entry + rozmiar + wielkosc offsetu
		return item + itemSize + sizeof(nodeEntry_off_t);
	}

	nodeEntry_off_t StringComparator::getKeySize(char* item, unsigned int itemSize) {
		//odleglosc do nastepnego od entry - rozmiar entry - rozmiar offsetu
		return (*(nodeEntry_off_t*)(item + itemSize)) - itemSize - sizeof(nodeEntry_off_t);
	}
	
	char* StringComparator::getNextItem(char* itemAddress, unsigned int itemSize) {
		//entry + offset do nastepnego (offset zaraz za naglowkiem)
		return itemAddress + (*(nodeEntry_off_t*)(itemAddress + itemSize)); 
	}
	
	nodeEntry_off_t StringComparator::maxEntrySize() {
		// entry + wskaznik konca + string
		return MAX_INDEX_STRUCT_SIZE + sizeof(nodeEntry_off_t) + MAX_STRING_LEN;
	}	
	
	char* StringComparator::getItem(int itemNumber, char* item0Address, unsigned int itemSize) {
		//item0Address = (item0Address + itemSize);
		for (; itemNumber > 0; itemNumber--) {
			item0Address = getNextItem(item0Address, itemSize);
		}
		return item0Address;
	}

	void StringComparator::putKey(char* where, nodeEntry_off_t itemSize) {
		*((nodeEntry_off_t*) where) = itemSize + strSize + sizeof(nodeEntry_off_t);
		strncpy(where + sizeof(nodeEntry_off_t), value, strSize);
	}
	
	Comparator* StringComparator::emptyClone() {
		return new StringComparator();
	}
	
	int StringComparator::setValue(string value) {
		setValueConverted(&value);	
		return 0;
	}
	
	template<typename T>
	void StringComparator::setValueConverted(T* value) {
		stringstream temp;
		temp << *value;
		string v = temp.str();
		strSize = normLen(v.size());
		spaceNeeded = strSize + sizeof(nodeEntry_off_t);
		
		this->value = new char[strSize];
		strncpy(this->value, v.c_str(), strSize);
	}
	
	int StringComparator::setValue(int value) {
		setValueConverted(&value);
		return 0;
	}
	
	int StringComparator::setValue(double value) {
		setValueConverted(&value);
		return 0;
	}
	
	void StringComparator::getValue(char* item, unsigned int itemSize) {
		strSize = getKeySize(item, itemSize);
		value = new char[strSize];
		spaceNeeded = strSize + sizeof(nodeEntry_off_t);
		memcpy(value, getKeyAddr(item, itemSize), strSize);
	}
		
	Comparator::comparatorType StringComparator::getType() const {
		return Comparator::STRING_C;
	}
	
	string StringComparator::typeToString() const {
		return "string"; 
	}
	
	//FixedLengthComparator
	
	template <class T>
	FixedLengthComparator<T>::FixedLengthComparator(T value, RootEntry* entry) : Comparator(entry), value(value) {
		init();
	}
	
	template <class T>
	FixedLengthComparator<T>::FixedLengthComparator() {
		init();
	}
	
	template <class T>
	void FixedLengthComparator<T>::init() {
		//spaceNeeded = sizeof(NodeEntry) + sizeof(T);
		MIN_NODE_SIZE_ALLOWED = constInit();
	}
	
	template <class T>
	T FixedLengthComparator<T>::getKey(char* item, unsigned int itemSize) {
		return *((T*) getKeyAddr(item, itemSize));
	}
	
	template <class T>
	int FixedLengthComparator<T>::compare (char* item, unsigned int itemSize, lid_t lidDiff) {
		T odds = value - getKey(item, itemSize);
		if (odds == 0) {return lidDiff;}
		return (odds < 0) ? -1 : 1;
	}

	template <class T>
	string FixedLengthComparator<T>::keyToString (char* item, unsigned int itemSize) {
		stringstream temp;
		temp << getKey(item, itemSize); 
		return temp.str();
	}

	template <class T>
	char* FixedLengthComparator<T>::getKeyAddr(char* item, unsigned int itemSize) {
		//entry + rozmiar + wielkosc offsetu
		return item + itemSize;
	}

	template <class T>
	nodeEntry_off_t FixedLengthComparator<T>::getKeySize(char* item, unsigned int itemSize) {
		//odleglosc do nastepnego od entry - rozmiar entry - rozmiar offsetu
		return sizeof(T);
	}
	
	template <class T>
	char* FixedLengthComparator<T>::getNextItem(char* itemAddress, unsigned int itemSize) {
		//entry + offset do nastepnego (offset zaraz za naglowkiem)
		return itemAddress + itemSize + sizeof(T);  
	}
	
	template <class T>
	nodeEntry_off_t FixedLengthComparator<T>::maxEntrySize() {
		// wpis + 
		return MAX_INDEX_STRUCT_SIZE + sizeof(T);
	}

	template <class T>
	char* FixedLengthComparator<T>::getItem(int itemNumber, char* item0Address, unsigned int itemSize) {
		return item0Address + (itemNumber * (itemSize + sizeof(T)));
	}
	
	template <class T>
	nodeEntry_off_t FixedLengthComparator<T>::getSpaceNeeded() {
		return sizeof(T);
	}

	template <class T>
	void FixedLengthComparator<T>::putKey(char* where, nodeEntry_off_t itemSize) {
		*((T*)where) = value;
	}
	
	template <class T>
	void FixedLengthComparator<T>::getValue(char* item, unsigned int itemSize) {
		setValue( *((T*) (item + itemSize)));
	}
	
	//************************************************
	// IntComparator
	//************************************************

	//IntComparator::IntComparator() : FixedLengthComparator<int>() {};
	//IntComparator::IntComparator(int value, RootEntry* entry) : FixedLengthComparator<int>(value, entry) {};
	
	int IntComparator::setValue(int value) {
		this->value = value;
		return 0;
	}
	
	IntComparator* IntComparator::emptyClone() {
		return new IntComparator();
	}
	
	string IntComparator::toString() {
		return ToStringBuilder("IntComparator", this)
			.append("value", value)
			.append("rootEntry", rootEntry ? rootEntry->toString() : "NULL")
			.toString();
	}
	
	Comparator::comparatorType IntComparator::getType() const {
		return Comparator::INT_C;
	}
	
	string IntComparator::typeToString() const {
		return "int"; 
	}
	
	//************************************************
	// DoubleComparator
	//************************************************
	
//	DoubleComparator::DoubleComparator() : FixedLengthComparator<double>() {};
//	DoubleComparator::DoubleComparator(double value, RootEntry* entry) : FixedLengthComparator<double>(value, entry) {};
	
	int DoubleComparator::setValue(double value) {
		this->value = value;
		return 0;
	}
	
	int DoubleComparator::setValue(int value) {
		this->value = value;
		return 0;
	}
	
	DoubleComparator* DoubleComparator::emptyClone() {
		return new DoubleComparator();
	}
	
	Comparator::comparatorType DoubleComparator::getType() const {
		return Comparator::DOUBLE_C;
	}
	
	string DoubleComparator::typeToString() const {
		return "double"; 
	}
	
	//************************************************
	// MinComparator
	//************************************************
	
	char* MinComparator::getItem(int itemNumber, char* item0Address, unsigned int itemSize) {
		assert (itemNumber == 0);
		return item0Address;
	}
	
	int MinComparator::compare(char* item, unsigned int itemSize, lid_t lidDiff) {
		return -1; // zawartosc tego comparatora zawsze ma byc mniejsza od tego co jest w wezle
	}
	
	nodeEntry_off_t MinComparator::getSpaceNeeded() {
		assert(false);
	}
	
	char* MinComparator::getNextItem(char* itemAddress, unsigned int itemSize) {
		assert(false);
	}
	
	nodeEntry_off_t MinComparator::maxEntrySize() {
		return 0;
	}
	
	void MinComparator::putKey(char* where, nodeEntry_off_t itemSize) {
		assert(false);
	}
	
	Comparator* MinComparator::emptyClone() {
		assert(false);
	}
	
	string MinComparator::keyToString(char* item, unsigned int itemSize) {
		assert(false);
	}
	
	void MinComparator::getValue(char* item, unsigned int itemSize) {
		assert(false);
	}
	
	Comparator::comparatorType MinComparator::getType() const {
		assert(false);
	}
	
	string MinComparator::typeToString() const {
		assert(false);
	}
	
//template <RootEntry*> bool Comparator::infinityReached(RootEntry*);
	
}

// template instantiation
void instantiation() {
	Comparator *c;
	RootEntry* r;
	NodeEntry* n;
	BTree* b;
	c->infinityReached(r);
	c->getNextItem(r);
	c->getItem(0, NULL, n);
	c->getNextItem(n);
	c->compare(n);
	c->greaterThan(n);
	c->putKey(n);
	c->getValue(n);
	c->getSpaceNeeded(n);
	
	c->keyToString(r);
	c->getItem(0, NULL, r);
	c->equal(r);
	c->lessThan(r);
	//c->greaterOrEqual(r);
	//c->greaterThan(r);
	c->getSpaceNeeded(r);
	c->putValue(r);
	c->getValue(r);
	c->lessOrEqual(r);
	
	b->getBalanceBorder(NULL, 0, r, true, NULL);
	b->getBalanceBorder(NULL, 0, n, true, NULL);
}
