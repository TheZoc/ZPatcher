#include "Precomp.h"

#ifdef _WIN32
	#include "Threads-Windows.cinc"
#else
	#include "Threads-Posix.cinc"
#endif
