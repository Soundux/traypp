#if defined(__linux__)
#include "appindicator.hpp"

namespace Soundux
{
    TrayPreInitializer::TrayPreInitializer(const std::string &name, const std::string &icon, Tray &parent)
    {
        assert(gtk_init_check(nullptr, nullptr) == TRUE);

        parent.appIndicator = app_indicator_new(name.c_str(), icon.c_str(), APP_INDICATOR_CATEGORY_APPLICATION_STATUS);
        app_indicator_set_status(parent.appIndicator, APP_INDICATOR_STATUS_ACTIVE);
    }

    void Tray::update()
    {
        assert(appIndicator != nullptr);
        app_indicator_set_menu(appIndicator, reinterpret_cast<GtkMenu *>(constructMenu(children)));
    }
    bool Tray::run()
    {
        if (appIndicator)
        {
            return gtk_main_iteration();
        }
        return false;
    }
    void Tray::exit()
    {
        appIndicator = nullptr;
    }

    void Tray::callback([[maybe_unused]] GtkWidget *widget, gpointer userData)
    {
        auto *item = reinterpret_cast<TrayItem *>(userData);

        if (auto *button = dynamic_cast<TrayButton *>(item); button)
        {
            button->onClicked();
        }
        else if (auto *check = dynamic_cast<TrayCheck *>(item); check)
        {
            check->onToggled(!check->isChecked());
        }
    }

    GtkMenuShell *Tray::constructMenu(const std::vector<std::shared_ptr<TrayItem>> &items)
    {
        auto *rtn = reinterpret_cast<GtkMenuShell *>(gtk_menu_new());

        for (const auto &item : items)
        {
            auto *_item = item.get();
            GtkWidget *gtkItem = nullptr;

            if (dynamic_cast<TrayButton *>(_item))
            {
                gtkItem = gtk_menu_item_new_with_label(_item->getName().c_str());
            }
            else if (auto *check = dynamic_cast<TrayCheck *>(_item); check)
            {
                gtkItem = gtk_check_menu_item_new_with_label(_item->getName().c_str());
                gtk_check_menu_item_set_active(reinterpret_cast<GtkCheckMenuItem *>(gtkItem), check->isChecked());
            }
            else if (auto *submenu = dynamic_cast<TraySubmenu *>(_item); submenu)
            {
                gtkItem = gtk_menu_item_new_with_label(submenu->getName().c_str());
                gtk_menu_item_set_submenu(reinterpret_cast<GtkMenuItem *>(gtkItem),
                                          reinterpret_cast<GtkWidget *>(constructMenu(submenu->getChildren())));
            }

            g_signal_connect(gtkItem, "activate", reinterpret_cast<GCallback>(callback), _item);

            gtk_widget_show(gtkItem);
            gtk_menu_shell_append(rtn, gtkItem);
        }
        return rtn;
    }
} // namespace Soundux
#endif