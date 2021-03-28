#include "tray/linux/appindicator.hpp"
#include <iostream>

int main()
{
    Soundux::Tray tray(
        "TestTray", "",
        Soundux::TrayCheck("Test Check", false, [](bool state) { std::cout << "Checked: " << state << std::endl; }),
        Soundux::TrayCheck("Test Check 2", true, [](bool state) { std::cout << "Checked 2: " << state << std::endl; }),
        Soundux::TraySubmenu("Submenu", Soundux::TrayButton("Some button", []() {
                                 std::cout << "Submenu Button 1 pressed!" << std::endl;
                             })));

    auto *submenu = dynamic_cast<Soundux::TraySubmenu *>(tray.getChildren().back().get());
    submenu->addItem(
        Soundux::TrayButton("Another button", []() { std::cout << "Submenu Button 2 pressed!" << std::endl; }));

    tray.addItem(Soundux::TrayButton("Exit", [&] { tray.exit(); }));

    while (tray.run())
    {
    }

    return 0;
}