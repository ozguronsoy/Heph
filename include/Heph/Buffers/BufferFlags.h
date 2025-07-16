#ifndef HEPH_BUFFER_FLAGS_H
#define HEPH_BUFFER_FLAGS_H

#include "Heph/Utils.h"

/** @file */

namespace Heph
{
    /** @brief Defines flags for buffer configuration.  */
    enum BufferFlags
    {
        /** @brief No flags are set. */
        None = 0,
        /** @brief Specifies that the elements will NOT be initialized after allocating memory. */
        AllocUninitialized = 1,
        /** @brief Specifies that the buffer is circular.  */
        Circular = 2,
        /** @brief Specifies that the transpose method will not modify the memory. */
        TransposeInPlace = 4
    };
}

#endif