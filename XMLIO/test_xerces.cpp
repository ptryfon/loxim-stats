#include <xercesc/util/XMLString.hpp>

XERCES_CPP_NAMESPACE_USE

int main() {
        XMLString::transcode("TEST\0");
}
