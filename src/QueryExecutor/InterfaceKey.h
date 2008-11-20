#ifndef INTERFACE_KEY_H
#define INTERFACE_KEY_H

#include <string>

typedef std::string TInterfaceKey; 

class InterfaceKey
{
	private:
		TInterfaceKey m_key;
	public:
		InterfaceKey() {setEmpty();}
		InterfaceKey(TInterfaceKey k) {setKey(k);};
		bool isEmpty() const {return m_key.empty();}
		void setEmpty() {m_key.clear();}
		TInterfaceKey getKey() const {return m_key;}
		void setKey(TInterfaceKey k) {m_key = k;}
};


#endif //INTERFACE_KEY_H

