## traypp
A cross-platform C++17 library that allows you to create simple tray menus.

## Compatibility
| Platform | Implementation            |
| -------- | ------------------------- |
| Windows  | WinAPI                    |
| Linux    | AppIndicator              |

## Usage

- Add the library to your project
  - ```cmake
    add_subdirectory(/path/to/traypp EXCLUDE_FROM_ALL)
    link_libraries(tray)
    ```
- Use the library
  - See `example` for examples