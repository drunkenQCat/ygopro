# Side 额外卡自动移至 Extra 卡组功能

## 功能介绍

本功能实现了一个独立的外接模块 `ExtraDeckHelper`，用于自动将 Side 卡组中的额外卡（融合、同步、超量、连接怪兽）移动到 Extra 卡组中。

### 功能特点

- 自动识别 Side 中的额外怪兽
- 自动将额外卡移动到 Extra 卡组
- 自动更新卡组计数
- 自动处理，无需手动调用

---

## 使用方式

### 集成步骤

1. 添加独立文件
2. 修改原文件
3. 重新编译

### 集成后使用

功能集成后会自动生效，加载卡组时自动处理，无需额外操作。

### 功能效果示例

假设原卡组配置：
- 主卡组：40 张
- 额外卡组：5 张
- Side 卡组：3 张（其中 1 张是 XYZ 额外卡）

**最终卡组结构**：
- 主卡组：40 张
- 额外卡组：6 张（原有5张 + Side移来的1张）
- Side 卡组：2 张

---

## 文件结构

### 独立文件（核心逻辑）

| 文件 | 说明 |
|------|------|
| `gframe/extra_deck_helper.h` | 头文件，定义 `ExtraDeckHelper` 类 |
| `gframe/extra_deck_helper.cpp` | 实现文件，包含 `PreProcessSideCards` 函数核心逻辑 |

### 原文件修改

| 文件 | 修改内容 |
|------|----------|
| `gframe/deck_manager.h` | 添加 `#include "extra_deck_helper.h"` |
| `gframe/deck_manager.cpp` | 在 `LoadDeck` 函数开头调用 `ExtraDeckHelper::PreProcessSideCards` |
| `gframe/premake5.lua` | 在 SERVER_MODE 项目的 `files` 中添加新文件 |

---

## 原文件修改详情

### 1. `gframe/deck_manager.h`

**位置**：在其他 `#include` 之后

**修改前**：
```cpp
#include <unordered_map>
#include <vector>
#include <sstream>
#include "deck.h"
```

**修改后**：
```cpp
#include <unordered_map>
#include <vector>
#include <sstream>
#include "deck.h"
#include "extra_deck_helper.h"
```

---

### 2. `gframe/deck_manager.cpp`

**位置**：`LoadDeck` 函数开头，第 159 行

**修改前**：
```cpp
uint32_t DeckManager::LoadDeck(Deck& deck, uint32_t dbuf[], int mainc, int sidec, bool is_packlist) {
    deck.clear();
    uint32_t errorcode = 0;
    auto& _datas = dataManager.GetDataTable();
    for(int i = 0; i < mainc; ++i) {
```

**修改后**：
```cpp
uint32_t DeckManager::LoadDeck(Deck& deck, uint32_t dbuf[], int mainc, int sidec, bool is_packlist) {
    deck.clear();
    uint32_t errorcode = 0;
    auto& _datas = dataManager.GetDataTable();

    // 预处理：将 side 中的额外卡分离出来
    ExtraDeckHelper::PreProcessSideCards(dbuf, &mainc, &sidec);

    for(int i = 0; i < mainc; ++i) {
```

---

### 3. `gframe/premake5.lua`

**位置**：`SERVER_MODE` 项目的 `files` 列表

**修改前**：
```lua
files { "gframe.cpp", "config.h",
        "game.cpp", "game.h", "myfilesystem.h",
        "deck_manager.cpp", "deck_manager.h",
        "data_manager.cpp", "data_manager.h",
        "replay.cpp", "replay.h",
        "netserver.cpp", "netserver.h",
        "single_duel.cpp", "single_duel.h",
        "tag_duel.cpp", "tag_duel.h" }
```

**修改后**：
```lua
files { "gframe.cpp", "config.h",
        "game.cpp", "game.h", "myfilesystem.h",
        "deck_manager.cpp", "deck_manager.h",
        "data_manager.cpp", "data_manager.h",
        "replay.cpp", "replay.h",
        "netserver.cpp", "netserver.h",
        "single_duel.cpp", "single_duel.h",
        "tag_duel.cpp", "tag_duel.h",
        "extra_deck_helper.cpp", "extra_deck_helper.h" }
```

---

## 重新添加功能完整步骤

1. **添加独立文件**：
   - `gframe/extra_deck_helper.h`
   - `gframe/extra_deck_helper.cpp`

2. **修改 `gframe/deck_manager.h`**：
   - 在 `#include "deck.h"` 之后添加 `#include "extra_deck_helper.h"`

3. **修改 `gframe/deck_manager.cpp`**：
   - 在 `LoadDeck` 函数开头添加调用代码

4. **修改 `gframe/premake5.lua`**：
   - 在 `files` 列表中添加新文件

5. **提交代码到 GitLab**：
   - 项目会自动在 Docker 中构建
   - premake 会自动生成 Windows 项目文件

---

## 关于 Docker 构建流程

### Windows 构建流程（GitLab CI）

```
1. 准备阶段：
   - 下载 premake 依赖
   - 复制 premake 配置到根目录

2. 生成项目文件阶段：
   - 运行 `.\premake5.exe vs2026 --server-zip-support`
   - 自动生成 build/ygopro.vcxproj（包含新文件）

3. 编译阶段：
   - 使用 MSBuild 编译生成的项目文件
```

### 关键文件

- **`gframe/premake5.lua`** - **唯一需要修改的项目配置文件**
- **`build/ygopro.vcxproj`** - **由 premake 自动生成，不需要手动修改**

---

## 注意事项

1. **额外卡数量限制**：处理后额外卡组不能超过 `EXTRA_MAX_SIZE`（15张）
2. **Token 跳过**：Token 卡不会被添加到任何卡组
3. **自动处理**：加载卡组时自动处理，无需手动调用
4. **额外卡类型**：融合、同步、超量、连接怪兽会被识别为额外卡
