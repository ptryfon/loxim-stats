#include <Indexes/Comparator.h>

#include <Indexes/ToStringBuilder.h>
#include <cassert>
#include <sstream>
#include <QueryExecutor/QueryResult.h>

using namespace QExecutor;
using namespace std;
namespace Indexes {

	
	//************************************************
	// Comparator
	//************************************************
	
	bool Comparator::canTakeExtraKey(Node* node) {
		return node->left >= maxEntrySize();
	}
	
	void Comparator::removeEntry(Node* node, RootEntry* entry) {  
		char* src = (char*) getNextItem(entry);
		long bytes2copy = node->afterAddr() - src;
		//node->left += getSpaceNeeded(entry);
		node->left += src - (char*)entry;
		memmove(entry, src, bytes2copy);
	}
	
	void Comparator::putValue(RootEntry* where) {
		assert(rootEntry);
		*where = *rootEntry;
		putKey(where);
		//cout << keyToString((char*)where, sizeof(RootEntry)) << endl;
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
	
	int Comparator::setValue(double /*value*/) {
		return EBadValue | ErrIndexes;
	}
	
	int Comparator::setValue(int /*value*/) {
		return EBadValue | ErrIndexes;
	}
	
	int Comparator::setValue(string /*value*/) {
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
		return -1;
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
	
	
	string Comparator::nodeToString(Node* node) {
		if (node->leaf) {
			RootEntry* e = NULL;
			return nodeToString(node, e);
		} else {
			NodeEntry* n = NULL;
			return nodeToString(node, n);
		}
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
	
	
	//************************************************
	// StringComparator
	//************************************************
	
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
	
	
	//************************************************
	// IntComparator
	//************************************************

	
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
	
	char* MinComparator::getItem(int itemNumber, char* item0Address, unsigned int /*itemSize*/) {
		assert (itemNumber == 0);
		return item0Address;
	}
	
	int MinComparator::compare(char* /*item*/, unsigned int /*itemSize*/, lid_t /*lidDiff*/) {
		return -1; // zawartosc tego comparatora zawsze ma byc mniejsza od tego co jest w wezle
	}
	
	nodeEntry_off_t MinComparator::getSpaceNeeded() {
		assert(false);
		return 0;
	}
	
	char* MinComparator::getNextItem(char* /*itemAddress*/, unsigned int /*itemSize*/) {
		assert(false);
		return NULL;
	}
	
	nodeEntry_off_t MinComparator::maxEntrySize() {
		return 0;
	}
	
	void MinComparator::putKey(char* /*where*/, nodeEntry_off_t /*itemSize*/) {
		assert(false);
	}
	
	Comparator* MinComparator::emptyClone() {
		assert(false);
		return NULL;
	}
	
	string MinComparator::keyToString(char* /*item*/, unsigned int /*itemSize*/) {
		assert(false);
		return "";
	}
	
	void MinComparator::getValue(char* /*item*/, unsigned int /*itemSize*/) {
		assert(false);
	}
	
	Comparator::comparatorType MinComparator::getType() const {
		assert(false);
		return Comparator::comparatorType();
	}
	
	string MinComparator::typeToString() const {
		assert(false);
		return "";
	}
	
	
}

