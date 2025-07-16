#ifndef HEPH_ICLONEABLE_H
#define HEPH_ICLONEABLE_H

#include "Heph/Utils.h"

/** @file */

namespace Heph
{
    /** @brief Provides method for creating clones. */
    class HEPH_API ICloneable
    {
    public:
        /** @copydoc destructor */
        virtual ~ICloneable() = default;

        /**
         * Creates a new object and copies the data of current instance.
         * 
         * @important The clone is created on the heap memory and should be used with a smart pointer.
         *  
         * @return Pointer to the new instance.
         */
        virtual ICloneable* Clone() const noexcept = 0;
    };
}

#endif