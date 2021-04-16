#if defined(_WIN32)
#include "windows.hpp"
#include <stdexcept>

namespace Soundux
{
    static constexpr auto WM_TRAY = WM_USER + 1;
    std::map<HWND, std::reference_wrapper<Tray>> Tray::trayList;

    TrayPreInitializer::TrayPreInitializer(const std::string &name, const std::variant<WORD, HICON, std::string> &icon,
                                           Tray &parent)
    {
        memset(&parent.windowClass, 0, sizeof(parent.windowClass));
        parent.windowClass.cbSize = sizeof(WNDCLASSEX);
        parent.windowClass.lpfnWndProc = parent.wndProc;
        parent.windowClass.lpszClassName = name.c_str();
        parent.windowClass.hInstance = GetModuleHandle(nullptr);

        RegisterClassEx(&parent.windowClass);
        parent.hwnd =
            CreateWindow(name.c_str(), nullptr, 0, 0, 0, 0, 0, nullptr, nullptr, parent.windowClass.hInstance, nullptr);
        UpdateWindow(parent.hwnd);

        memset(&parent.notifyData, 0, sizeof(NOTIFYICONDATA));
        parent.notifyData.cbSize = sizeof(NOTIFYICONDATA);
        parent.notifyData.hWnd = parent.hwnd;
        parent.notifyData.uFlags = NIF_ICON | NIF_MESSAGE;
        parent.notifyData.uCallbackMessage = WM_TRAY;

        if (std::holds_alternative<WORD>(icon))
        {
            parent.notifyData.hIcon =
                LoadIcon(parent.windowClass.hInstance, MAKEINTRESOURCE(std::get<WORD>(icon))); // NOLINT
        }
        else if (std::holds_alternative<HICON>(icon))
        {
            parent.notifyData.hIcon = std::get<HICON>(icon);
        }
        else if (std::holds_alternative<std::string>(icon))
        {
            auto *hIcon = reinterpret_cast<HICON>(LoadImageA(nullptr, std::get<std::string>(icon).c_str(), IMAGE_ICON,
                                                             0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE));
            if (hIcon == nullptr)
            {
                throw std::runtime_error("Failed to load icon");
                return;
            }

            parent.notifyData.hIcon = hIcon;
        }

        Shell_NotifyIcon(NIM_ADD, &parent.notifyData);
        parent.trayList.insert({parent.hwnd, parent});
    }

    void Tray::update()
    {
        DestroyMenu(menu);
        menu = constructMenu(children, true);

        Shell_NotifyIcon(NIM_MODIFY, &notifyData);
        SendMessage(hwnd, WM_INITMENUPOPUP, reinterpret_cast<WPARAM>(menu), 0);
    }
    bool Tray::run()
    {
        static MSG msg;
        if (GetMessage(&msg, hwnd, 0, 0))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            return false;
        }

        return true;
    }
    void Tray::exit()
    {
        Shell_NotifyIcon(NIM_DELETE, &notifyData);
        DestroyIcon(notifyData.hIcon);
        DestroyMenu(menu);

        UnregisterClass(name.c_str(), GetModuleHandle(nullptr));
        PostMessage(hwnd, WM_QUIT, 0, 0);
        trayList.erase(hwnd);
        allocations.clear();

        if (!icon.empty())
        {
            DestroyIcon(notifyData.hIcon);
        }
    }

    HMENU Tray::constructMenu(const std::vector<std::shared_ptr<TrayItem>> &items, bool cleanup)
    {
        static auto id = 0;
        if (cleanup)
        {
            allocations.clear();
        }

        auto *rtn = CreatePopupMenu();
        for (const auto &item : items)
        {
            auto *_item = item.get();

            auto name = std::shared_ptr<char[]>(new char[_item->getName().size() + 1]);
            strcpy(name.get(), _item->getName().c_str()); // NOLINT
            allocations.emplace_back(name);

            MENUITEMINFO winItem{0};

            winItem.wID = ++id;
            winItem.dwTypeData = name.get();
            winItem.cbSize = sizeof(MENUITEMINFO);
            winItem.dwItemData = reinterpret_cast<ULONG_PTR>(_item);
            winItem.fMask = MIIM_ID | MIIM_TYPE | MIIM_STATE | MIIM_DATA;
            if (auto *check = dynamic_cast<TrayCheck *>(_item); check)
            {
                if (check->isChecked())
                {
                    winItem.fState |= MFS_CHECKED;
                }
                else
                {
                    winItem.fState |= MFS_UNCHECKED;
                }
            }
            else if (auto *syncedCheck = dynamic_cast<TraySyncedCheck *>(_item); syncedCheck)
            {
                if (syncedCheck->isChecked())
                {
                    winItem.fState |= MFS_CHECKED;
                }
                else
                {
                    winItem.fState |= MFS_UNCHECKED;
                }
            }
            else if (auto *submenu = dynamic_cast<TraySubmenu *>(_item); submenu)
            {
                winItem.fMask |= MIIM_SUBMENU;
                winItem.hSubMenu = constructMenu(submenu->getChildren());
            }

            InsertMenuItem(rtn, 0, TRUE, &winItem);
        }
        return rtn;
    }

    LRESULT CALLBACK Tray::wndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        switch (msg)
        {
        case WM_TRAY:
            if (lParam == WM_RBUTTONUP)
            {
                POINT p;
                GetCursorPos(&p);
                SetForegroundWindow(hwnd);
                auto &menu = trayList.at(hwnd).get();
                auto cmd = TrackPopupMenu(menu.menu, TPM_RETURNCMD | TPM_NONOTIFY, p.x, p.y, 0, hwnd, nullptr);
                SendMessage(hwnd, WM_COMMAND, cmd, 0);
                return 0;
            }
            break;
        case WM_COMMAND:
            MENUITEMINFO winItem{0};
            winItem.fMask = MIIM_DATA | MIIM_ID;
            winItem.cbSize = sizeof(MENUITEMINFO);

            auto &menu = trayList.at(hwnd).get();
            if (GetMenuItemInfo(menu.menu, static_cast<UINT>(wParam), FALSE, &winItem))
            {
                auto *item = reinterpret_cast<TrayItem *>(winItem.dwItemData);
                if (auto *button = dynamic_cast<TrayButton *>(item); button)
                {
                    button->onClicked();
                }
                else if (auto *check = dynamic_cast<TrayCheck *>(item); check)
                {
                    check->onToggled(!check->isChecked());
                    menu.update();
                }
                else if (auto *syncedCheck = dynamic_cast<TraySyncedCheck *>(item); syncedCheck)
                {
                    syncedCheck->onToggled(!syncedCheck->isChecked());
                    menu.update();
                }
            }
            break;
        }
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
} // namespace Soundux
#endif