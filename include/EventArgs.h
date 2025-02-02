#pragma once
#include "HephShared.h"

/** @file */

namespace Heph
{
	/** @brief Base class for storing arguments for an event. */
	struct HEPH_API EventArgs
	{
		/** @copydoc destructor */
		virtual ~EventArgs() = default;
	};
}