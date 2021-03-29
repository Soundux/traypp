#include "tray.hpp"
#include <iostream>

int main()
{
#if defined(__linux__)
    Soundux::Tray tray(
        "TestTray", "icon.png",
        Soundux::TrayCheck("Test Check", false, [](bool state) { std::cout << "Checked: " << state << std::endl; }),
        Soundux::TrayCheck("Test Check 2", true, [](bool state) { std::cout << "Checked 2: " << state << std::endl; }));
#elif defined(_WIN32)
    Soundux::Tray tray(
        "TestTray", 0 /*ICON_RESOURCE*/,
        Soundux::TrayCheck("Test Check", false, [](bool state) { std::cout << "Checked: " << state << std::endl; }),
        Soundux::TrayCheck("Test Check 2", true, [](bool state) { std::cout << "Checked 2: " << state << std::endl; }),
        Soundux::TraySubmenu("Submenu", Soundux::TrayButton("Some button", []() {
                                 std::cout << "Submenu Button 1 pressed!" << std::endl;
                             })));
#endif

    tray.addItem(Soundux::TraySubmenu(
                     "Submenu", Soundux::TrayButton("Some button",
                                                    []() { std::cout << "Submenu Button 1 pressed!" << std::endl; })))
        ->addItem(
            Soundux::TrayButton("Another button", []() { std::cout << "Submenu Button 2 pressed!" << std::endl; }));

    tray.addItem(Soundux::TrayButton("Exit", [&] { tray.exit(); }));

    while (tray.run())
    {
    }

    return 0;
}