#include <iostream>
#include <tray.hpp>

int main()
{
    Tray::Tray tray("test", "icon.ico");

    tray.addEntry(Tray::Button("Test"));
    tray.addEntry(Tray::Button("Test"))->setDisabled(true);
    tray.addEntry(Tray::Seperator());
    tray.addEntry(Tray::Label("Test Label"));
    tray.addEntry(Tray::Toggle("Test Toggle", false, [](bool state) { printf("State: %i\n", state); }));

    while (tray.run(true))
    {
    }

    return 0;
}