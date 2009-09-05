set(ENUMSDIR "../Protocol/Enums")

set(ProtocolEnums_SRCS 
	${ENUMSDIR}/Features.h 
	${ENUMSDIR}/AuthMethods.h 
	${ENUMSDIR}/Collations.h 
	${ENUMSDIR}/Modes.h 
	${ENUMSDIR}/StatementFlags.h 
	${ENUMSDIR}/Errors.h 
	${ENUMSDIR}/ByeReasons.h 
	${ENUMSDIR}/SendValueFlags.h 
	${ENUMSDIR}/AbortReasonCodes.h 
	${ENUMSDIR}/DataPackages.h 
	${ENUMSDIR}/Packages.h CACHE INTERNAL "" )

