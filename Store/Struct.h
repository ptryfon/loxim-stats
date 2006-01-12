#ifndef __STORE_STRUCT_H__
#define __STORE_STRUCT_H__

#define STORE_PAGESIZE					0x0400	// 1024

#define STORE_FILE_NONE					0x0000
#define STORE_FILE_DEFAULT				0x0001
#define STORE_FILE_MAP					0xFF10
#define STORE_FILE_ROOTS				0xFF20

#define STORE_PAGE_DATAHEADER			0x0001
#define STORE_PAGE_DATAPAGE				0x0002
#define STORE_PAGE_MAPHEADER			0x1001
#define STORE_PAGE_MAPPAGE				0x1002
#define STORE_PAGE_ROOTSHEADER			0x2001
#define STORE_PAGE_ROOTSPAGE			0x2002

namespace Store
{
	typedef struct physical_id
	{
		unsigned int page_id;
		unsigned short file_id;
		unsigned short offset;
	} physical_id;

	typedef struct page_header
	{
		unsigned int page_id;
		unsigned short file_id;
		unsigned short page_type;
		unsigned int timestamp;
	} page_header;
	
	typedef struct page_header_ex
	{
		int objectCount;
		int freeSpace;
	} page_header_ex;

	typedef struct map_header
	{
		page_header page_hdr;
		unsigned int page_count;
		unsigned int last_assigned;
	} map_header;

	typedef struct map_page
	{
		page_header page_hdr;
	} map_page;

	typedef struct roots_header
	{
		page_header page_hdr;
		unsigned int last_assigned;
		unsigned int page_count;
	} roots_header;

	typedef struct roots_page
	{
		page_header page_hdr;
	} roots_page;

//	typedef struct data_header
//	{
//		page_header page_hdr;
//		unsigned int page_count;
//	} data_header;
	
	typedef struct page_data
	{
		unsigned char byte[];
		page_header header;
		page_header_ex headex;
		int objectOffset[];
	} page_data;
}

#endif // __STORE_SIZES_H__
