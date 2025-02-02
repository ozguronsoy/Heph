#include "EventParams.h"

namespace Heph
{
    EventParams::EventParams(const UserEventArgs& userArgs)
        : pArgs(nullptr), pResult(nullptr), userArgs(userArgs)
    {
    }
}