#ifndef __STORE_STRUCT_H__
#define __STORE_STRUCT_H__

#define STORE_PAGESIZE					0x4000	// 16384

#define STORE_FILE_NONE					0x0000
#define STORE_FILE_DEFAULT				0x0001
#define STORE_FILE_MAP					0xFF10
#define STORE_FILE_ROOTS				0xFF20
#define STORE_FILE_VIEWS				0xFF30
#define STORE_FILE_CLASSES				0xFF40
#define STORE_FILE_INTERFACES			0xFF50
#define STORE_FILE_SCHEMAS				0xFF60

#define STORE_PAGE_DATAHEADER			0x0001
#define STORE_PAGE_DATAPAGE				0x0002
#define STORE_PAGE_MAPHEADER			0x1001
#define STORE_PAGE_MAPPAGE				0x1002
#define STORE_PAGE_ROOTSHEADER			0x2001
#define STORE_PAGE_ROOTSPAGE			0x2002
#define STORE_PAGE_VIEWSHEADER			0x3001
#define STORE_PAGE_VIEWSPAGE			0x3002
#define STORE_PAGE_CLASSESHEADER		0x4001
#define STORE_PAGE_CLASSESPAGE			0x4002
#define STORE_PAGE_INTERFACESHEADER		0x5001
#define STORE_PAGE_INTERFACESPAGE		0x5002
#define STORE_PAGE_SCHEMASHEADER		0x6001
#define STORE_PAGE_SCHEMASPAGE			0x6002



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
		int timestamp;
	} page_header;
	
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

	typedef struct page_data
	{
		unsigned char bytes[];
		page_header header;
		int object_count;
		int free_space;
		int object_offset[];
	} page_data;
}

#endif // __STORE_SIZES_H__
