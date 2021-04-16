## traypp
A cross-platform C++17 library that allows you to create simple tray menus.

## Compatibility
| Platform | Implementation |
| -------- | -------------- |
| Windows  | WinAPI         |
| Linux    | AppIndicator   |

## Dependencies
- Linux
  - libappindicator-gtk3

## Basic Usage
```cpp
#include <tray.hpp>
using Soundux::Tray;
using Soundux::TrayButton;

int main()
{
  Tray tray("My Tray", "icon.ico");
  tray.addItem(TrayButton("Exit"), [&]{
    tray.exit();
  });

  while (tray.run()) {}

  return 0;
}
```
> On Windows it is not necessary to pass an icon path as icon, you can also use an icon-resource or an existing HICON.

## Menu components
### Button
```cpp
TrayButton(std::string name, std::function<void()> clicked)
```
**Parameters:**
- `clicked` - The function that is called when the button is pressed
----
### Check
```cpp
TrayCheck(std::string name, bool toggled, std::function<void(bool)> callback)
```
**Parameters:**
- `toggled` - The default state of the Toggle
- `callback` - The function that is called when the toggle is pressed
----
### Synced Check
```cpp
TraySyncedCheck(std::string name, bool &toggled, std::function<void(bool)> callback)
```
**Parameters:**
- `toggled` - Reference to a boolean that holds the toggle state
  > The provided boolean will influence the toggled state and **will be modified** if the toggle-state changes
- `callback` - The function that is called when the toggle is pressed
----
### Submenu
```cpp
template <typename... T>
TraySubmenu(std::string name, const T &...items)
```
**Parameters:**
- `items` - The items that should be added upon construction
  > Can be empty - you can add children later with `addItem`/`addItems`
----

## Installation

- Add the library to your project
  ```cmake
  add_subdirectory(/path/to/traypp EXCLUDE_FROM_ALL)
  link_libraries(tray)
  ```
- Use the library
  - See `example` for examples