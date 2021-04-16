#if defined(_WIN32)
#pragma once
#include "../tray.hpp"
#include <Windows.h>
#include <cassert>
#include <map>
#include <shellapi.h>
#include <variant>

namespace Soundux
{
    class Tray;
    class TrayPreInitializer
    {
      protected:
        TrayPreInitializer(const std::string &, const std::variant<WORD, HICON, std::string> &, Tray &);
    };

    class Tray : TrayPreInitializer, public BaseTray
    {
        friend class TrayPreInitializer;

        HWND hwnd = nullptr;
        HMENU menu = nullptr;
        WNDCLASSEX windowClass;
        NOTIFYICONDATA notifyData;
        std::vector<std::shared_ptr<char[]>> allocations;
        HMENU constructMenu(const std::vector<std::shared_ptr<TrayItem>> &, bool = false);

        static LRESULT CALLBACK wndProc(HWND, UINT, WPARAM, LPARAM);
        static std::map<HWND, std::reference_wrapper<Tray>> trayList;

      public:
        template <typename... T>
        Tray(std::string name, const std::variant<WORD, HICON, std::string> &icon, const T &...items)
            : TrayPreInitializer(name, icon, *this),
              BaseTray(std::move(name), std::holds_alternative<std::string>(icon) ? std::get<std::string>(icon) : "",
                       items...)
        {
            update();
        }

        bool run() override;
        void exit() override;
        void update() override;
    };
} // namespace Soundux
#endif