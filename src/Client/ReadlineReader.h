#ifndef READLINEREADER_H_
#define READLINEREADER_H_

namespace protocol{

    class ReadlineReader
    {
	private:
		char* buffor;		
		
	public:
		ReadlineReader();
		
	public:
		char* readStatement();	
    };
}

#endif /*READLINEREADER_H_*/
