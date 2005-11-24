#ifndef __STORE_PAGEPOINTER_H__
#define __STORE_PAGEPOINTER_H__

namespace Store
{
	class PagePointer
	{
	private:
		unsigned short fileID;
		unsigned int pageID;
		char* pagePointer;

	public:
		PagePointer(unsigned short fileID, unsigned int pageID, char* pagePointer);

		int aquire();
		int release();
		char* getPage();
	};
};

#endif // __STORE_PAGEPOINTER_H__
