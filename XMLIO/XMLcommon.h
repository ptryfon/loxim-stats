#ifndef XMLCOMMON_H_
#define XMLCOMMON_H_

#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLString.hpp>
#include <string>

using namespace std;

XERCES_CPP_NAMESPACE_USE

enum FileFormat {
	FORMAT_SIMPLE = 0,
	FORMAT_EXTENDED = 1
};		

class XStr
{
public :
    // -----------------------------------------------------------------------
    //  Constructors and Destructor
    // -----------------------------------------------------------------------
       XStr(string toTranscode)
    {
	fUnicodeForm = XMLString::transcode(toTranscode.c_str());
    }


    XStr(const char* const toTranscode)
    {
        // Call the private transcoding method
        fUnicodeForm = XMLString::transcode(toTranscode);
    }
     ~XStr()
    {
        XMLString::release(&fUnicodeForm);
    }


    // -----------------------------------------------------------------------
    //  Getter methods
    // -----------------------------------------------------------------------
    const XMLCh* unicodeForm() const
    {
        return fUnicodeForm;
    }

private :
    // -----------------------------------------------------------------------
    //  Private data members
    //
    //  fUnicodeForm
    //      This is the Unicode XMLCh format of the string.
    // -----------------------------------------------------------------------
    XMLCh*   fUnicodeForm;
};

#define X(str) XStr(str).unicodeForm()

#endif /*XMLCOMMON_H_*/
