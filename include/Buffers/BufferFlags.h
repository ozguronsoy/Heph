#ifndef HEPH_BUFFER_FLAGS_H
#define HEPH_BUFFER_FLAGS_H

#include "HephShared.h"

/** @file */

namespace
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
        /** @brief Specifies that only strides will be adjusted when transposing. */
        TransposeOnlyStrides = 4
    };
}

#endif