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
		class Buffer* buffer;

	public:
		PagePointer(unsigned short fileID, unsigned int pageID, char* pagePointer, class Buffer* buffer);

		unsigned short getFileID();
		unsigned int getPageID();
		int aquire();
		int release();
		int releaseSync();
		char* getPage();
		void setPage(char* pagePointer);
	};
};

#endif // __STORE_PAGEPOINTER_H__
