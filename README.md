# pdf-tools

A PDF reader built with C++17 and Qt 6.

## Tech Stack

- **Language:** C++17
- **GUI:** Qt 6.11 (Widgets)
- **PDF Engine:** Qt6::PdfWidgets (based on PDFium)
- **Build System:** CMake + Ninja
- **License:** GPL v3

## Features

- Page rendering with smooth zoom and scroll
- Table of contents navigation
- Full-text search with result highlighting
- Bookmark management
- Text selection and copy
- Print support
- Dark theme

## Build

```powershell
cmake -B build -G Ninja -DCMAKE_PREFIX_PATH="C:/Qt/6.11.x/msvc2022_64"
cmake --build build
./build/pdf-tools.exe
```

## License

GNU General Public License v3.0. See [LICENSE](LICENSE).
