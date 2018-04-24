#undef OTHERCORE
#ifdef RELEASECONFIG
#define OTHERCORE "..\CoreA\Release\Producer_BF561.dxe"
#else
#define OTHERCORE "..\CoreA\Debug\Producer_BF561.dxe"
#endif
#include <shared_symbols.h>
RESOLVE(_turn, OTHERCORE)
RESOLVE(_interested, OTHERCORE)
RESOLVE(_D0WasDisabled, OTHERCORE)
RESOLVE(_S0EnabledCount, OTHERCORE)

