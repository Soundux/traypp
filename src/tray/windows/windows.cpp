#if defined(_WIN32)
#include "windows.hpp"

namespace Soundux
{
    static constexpr auto WM_TRAY = WM_USER + 1;
    std::map<HWND, std::reference_wrapper<Tray>> Tray::trayList;

    TrayPreInitializer::TrayPreInitializer(const std::string &name, WORD icon, Tray &parent)
    {
        memset(&parent.windowClass, 0, sizeof(parent.windowClass));
        parent.windowClass.cbSize = sizeof(WNDCLASSEX);
        parent.windowClass.lpfnWndProc = parent.wndProc;
        parent.windowClass.lpszClassName = name.c_str();
        parent.windowClass.hInstance = GetModuleHandle(nullptr);

        assert(RegisterClassEx(&parent.windowClass) != 0);
        parent.hwnd = CreateWindowEx(0, name.c_str(), nullptr, 0, 0, 0, 0, 0, nullptr, nullptr, nullptr, nullptr);
        assert(parent.hwnd != nullptr);
        UpdateWindow(parent.hwnd);

        memset(&parent.notifyData, 0, sizeof(parent.notifyData));
        parent.notifyData.cbSize = sizeof(NOTIFYICONDATA);
        parent.notifyData.hWnd = parent.hwnd;
        parent.notifyData.hIcon = LoadIcon(GetModuleHandle(nullptr), MAKEINTRESOURCE(icon)); // NOLINT
        parent.notifyData.uFlags = NIF_ICON | NIF_MESSAGE;
        parent.notifyData.uCallbackMessage = WM_TRAY;
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
            strcpy(name.get(), _item->getName().c_str());
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
            }
            break;
        }
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
} // namespace Soundux
#endif