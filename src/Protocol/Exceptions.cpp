/* DO NOT MODIFY, this file is auto generated using lw_protogen*/
#include <errno.h>
#include <Protocol/Exceptions.h>

namespace Protocol{
	OperationCancelled::OperationCancelled() : LoximException(ECANCELED)
	{
	}
	
	ReadError::ReadError(int error) : LoximException(error)
	{
	}
	
	WriteError::WriteError(int error) : LoximException(error)
	{
	}
	
	ConnectionError::ConnectionError(int error) : LoximException(error)
	{
	}
	
	ProtocolLogic::ProtocolLogic() : LoximException(EINVAL)
	{
	}
	
	PackageTooBig::PackageTooBig() : LoximException(E2BIG)
	{
	}
}

