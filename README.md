# pdf-tools

基于 C++17 和 Qt 6 构建的轻量级 PDF 阅读器。

## 技术栈

- **语言：** C++17
- **GUI：** Qt 6 (Widgets)
- **PDF 引擎：** Qt6::PdfWidgets（基于 PDFium）
- **构建系统：** CMake + Ninja
- **许可证：** GPL v3

## 功能

- **页面渲染** — 平滑缩放与滚动，支持鼠标滚轮和键盘导航
- **目录导航** — 自动解析 PDF 书签，点击跳转
- **全文搜索** — 实时搜索，结果高亮与跳转
- **书签管理** — 添加/删除书签，按文件持久化保存
- **文本选择** — 选择模式下框选文本，支持复制
- **打印支持** — 完整打印对话框，支持多页打印
- **深色主题** — 一键切换，设置自动保存
- **拖放打开** — 直接拖放 PDF 文件到窗口打开
- **最近文件** — 自动记录最近打开的文件

## 快捷键

| 快捷键 | 功能 |
|--------|------|
| `Ctrl+O` | 打开文件 |
| `Ctrl+P` | 打印 |
| `Ctrl+W` | 关闭文档 |
| `Ctrl+Q` | 退出 |
| `Ctrl+G` | 跳转页面 |
| `Ctrl+F` | 搜索 |
| `Ctrl+B` | 添加书签 |
| `Ctrl+Shift+T` | 切换侧边栏 |
| `Ctrl+Shift+D` | 切换深色主题 |
| `Alt+S` | 切换文本选择模式 |
| `Ctrl+滚轮` | 缩放 |
| `PageUp/Down` | 翻页 |
| `Home/End` | 首页/末页 |
| `Ctrl+C` | 复制选中文本 |

## 项目结构

```
pdf-tools/
├── src/
│   ├── main.cpp              # 程序入口
│   ├── MainWindow.h/cpp      # 主窗口（菜单、工具栏、状态栏）
│   ├── PdfView.h/cpp         # PDF 渲染视图（缩放、滚动、选择）
│   ├── TocWidget.h/cpp       # 目录导航面板
│   ├── SearchWidget.h/cpp    # 全文搜索面板
│   └── BookmarkWidget.h/cpp  # 书签管理面板
├── resources/
│   ├── resources.qrc         # Qt 资源文件
│   └── icons/                # 图标资源
├── CMakeLists.txt            # 构建配置
├── CMakePresets.json         # CMake 预设
└── README.md
```

## 构建要求

- Qt 6.2+（需要 PdfWidgets 模块）
- CMake 3.16+
- C++17 编译器（MSVC 2022 / GCC 10+ / Clang 12+）

## 构建

```powershell
# 使用 CMakePresets（推荐）
cmake --preset default
cmake --build --preset default

# 或手动指定 Qt 路径
cmake -B build -G Ninja -DCMAKE_PREFIX_PATH="C:/Qt/6.x.x/msvc2022_64"
cmake --build build
```

## 许可证

GNU General Public License v3.0。详见 [LICENSE](LICENSE)。
