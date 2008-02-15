#ifndef __TC_CONSTANTS_H__
#define __TC_CONSTANTS_H__

// Some TC constants, to use across typechecking procs..
namespace TypeCheckConstants {

#define TC_FINAL_ERROR 		-112

#define TC_MDN_NAME			"__MDN__"
#define TC_MDNT_NAME		"__MDNT__"
#define	TC_SUB_OBJ_NAME		"subobject"
#define TC_MDN_ATOMIC		"atomic"
#define TC_MDN_LINK			"link"
#define TC_MDN_COMPLEX		"complex"
#define TC_MDN_DEFTYPE		"defined_type"
#define TC_REF_TYPE_SUFF	"__REFTYPE_"
#define TC_MDS_KIND			"kind"
#define TC_MDS_NAME			"name"
#define TC_MDS_CARD			"card"
#define TC_MDS_ISDIST		"isDistinct"
#define TC_MDS_REFNAME		"refName"
#define TC_MDS_TYPENAME		"typeName"
#define TC_MDS_TYPE			"type"
#define TC_MDS_OWNER		"owner"
#define TC_MDS_TARGET		"target"
#define TC_CK_BAG			"bag"
#define TC_CK_SEQ			"sequence"
#define TC_RS_ERROR			"ERROR"
#define TC_RS_COERCE		"COERCE"
#define TC_RS_SUCCESS		"SUCCESS"

#define TC_INTEGER			"integer"
#define TC_STRING			"string"
#define TC_BOOLEAN			"boolean"
//Meta rules markers
#define M_EQ		"__MS_"
#define M_L			"__ML_"
#define M_R			"__MR_"
#define M_B			"__MB_"
#define META		"__MT_"
#define M_ELSE		"__ME_"
#define	M_0			"__N_N_"
#define M_MATCH		"__AC_"
	
	
}








#endif

