#if defined(_WIN32)
#include "../tray.hpp"
#include <Windows.h>
#include <cassert>
#include <map>
#include <shellapi.h>

namespace Soundux
{
    class Tray;
    class TrayPreInitializer
    {
      protected:
        TrayPreInitializer(const std::string &, WORD, Tray &);
    };

    class Tray : TrayPreInitializer, public BaseTray
    {
        friend class TrayPreInitializer;

        HWND hwnd;
        HMENU menu = nullptr;
        WNDCLASSEX windowClass;
        NOTIFYICONDATA notifyData;
        std::vector<std::shared_ptr<char[]>> allocations;
        HMENU constructMenu(const std::vector<std::shared_ptr<TrayItem>> &, bool = false);

        static LRESULT CALLBACK wndProc(HWND, UINT, WPARAM, LPARAM);
        static std::map<HWND, std::reference_wrapper<Tray>> trayList;

      public:
        template <typename... T>
        Tray(std::string name, WORD icon, const T &...items)
            : TrayPreInitializer(name, icon, *this), BaseTray(std::move(name), "", items...)
        {
            update();
        }

        bool run() override;
        void exit() override;
        void update() override;
    };
} // namespace Soundux
#endif