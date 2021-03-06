// -*- C++ -*- Exception handling routines for catching.

#include <typeinfo>
#include <cxxabi.h>
#include "unwind-cxx.h"

namespace __cxxabiv1
{

	// Returns the type_info for the currently handled exception [15.3/8], or
	// null if there is none.
extern "C"
std::type_info *__cxa_current_exception_type ()
{
  	__cxa_eh_globals *globals = __cxa_get_globals ();
  	__cxa_exception *header = globals->caughtExceptions;
  	if (header)
    	return header->exceptionType;
  	else
    	return 0;
}

} // namespace __cxxabiv1
