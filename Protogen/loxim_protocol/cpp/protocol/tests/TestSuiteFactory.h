#ifndef TESTSUITEFACTORY_H_
#define TESTSUITEFACTORY_H_

#include "../layers/ProtocolLayer0.h"

namespace protocol {

class TestSuiteFactory {
public:
	static bool callAllSendTests(ProtocolLayer0* socket);
	static bool callSendTest(ProtocolLayer0* socket,char* testSuiteName);
	static bool callAllRecTests(ProtocolLayer0* socket);
	static bool callRecTest(ProtocolLayer0* socket,char* testSuiteName);

};

}

#endif /*TESTSUITEFACTORY_H_*/
