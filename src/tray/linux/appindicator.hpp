#if defined(__linux__)
#include "../tray.hpp"
#include <cassert>
#include <libappindicator/app-indicator.h>

namespace Soundux
{
    class Tray;
    class TrayPreInitializer
    {
      protected:
        TrayPreInitializer(const std::string &, const std::string &, Tray &);
    };

    class Tray : TrayPreInitializer, public BaseTray
    {
        friend class TrayPreInitializer;

        AppIndicator *appIndicator;
        static void callback(GtkWidget *, gpointer);
        static GtkMenuShell *constructMenu(const std::vector<std::shared_ptr<TrayItem>> &);

      public:
        template <typename... T>
        Tray(std::string name, std::string icon, const T &...items)
            : TrayPreInitializer(name, icon, *this), BaseTray(std::move(name), std::move(icon), items...)
        {
            update();
        }

        bool run() override;
        void exit() override;
        void update() override;
    };
} // namespace Soundux
#endif