#ifndef HEPH_EVENT_ARGS_H
#define HEPH_EVENT_ARGS_H

#include "HephShared.h"

/** @file */

namespace Heph
{
	/** @brief Base class for storing arguments for an event. */
	struct HEPH_API EventArgs
	{
		virtual ~EventArgs() = default;
	};
}

#endif