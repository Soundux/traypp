#pragma once
#include <core/entry.hpp>
#include <functional>

namespace Tray
{
    class Seperator : public TrayEntry
    {
      public:
        Seperator();
        ~Seperator() override = default;
    };
} // namespace Tray