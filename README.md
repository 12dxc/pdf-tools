# pdf-tools

基于 C++17 和 Qt 6 构建的 PDF 阅读器。

## 技术栈

- **语言：** C++17
- **GUI：** Qt 6.11 (Widgets)
- **PDF 引擎：** Qt6::PdfWidgets（基于 PDFium）
- **构建系统：** CMake + Ninja
- **许可证：** GPL v3

## 功能

- 页面渲染（平滑缩放与滚动）
- 目录导航
- 全文搜索（结果高亮与跳转）
- 书签管理
- 文本选择与复制
- 打印支持
- 深色主题

## 构建

```powershell
# 使用 CMakePresets（推荐）
cmake --preset default
cmake --build --preset default

# 或手动指定 Qt 路径
cmake -B build -G Ninja -DCMAKE_PREFIX_PATH="C:/Qt/6.11.x/msvc2022_64"
cmake --build build
```

## 许可证

GNU General Public License v3.0。详见 [LICENSE](LICENSE)。
