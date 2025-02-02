#include "Exceptions/ExceptionEventArgs.h"

namespace Heph
{
	ExceptionEventArgs::ExceptionEventArgs(const Exception& ex)
		: exception(ex)
	{
	}
}