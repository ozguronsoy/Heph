#include "EventParams.h"

namespace Heph
{
    EventParams::EventParams(const EventArgs& args, EventResult& result)
        : args(args), result(result)
    {
    }
}