<h1 align=center>SnakeGame</h1>

<h2 align=center>--Based on EasyX</h2>

---
---

![C++](https://img.shields.io/badge/c++-%2300599C.svg?style=for-the-badge&logo=c%2B%2B&logoColor=white)
![MIT LICENSE](https://img.shields.io/github/license/Ileriayo/markdown-badges?style=for-the-badge)

# 目录
- [目录](#目录)
- [配置](#配置)
  - [在 VSCode 中配置 EasyX 图形库](#在-vscode-中配置-easyx-图形库)
  - [VSCode 配置文件](#vscode-配置文件)
- [游戏规则说明](#游戏规则说明)
  - [🎯 游戏目标](#-游戏目标)
  - [🕹️ 操作控制](#️-操作控制)
  - [⚙️ 游戏机制](#️-游戏机制)
  - [🏆 胜利与失败](#-胜利与失败)
  - [🌟 特色功能](#-特色功能)
- [关键模块](#关键模块)
  - [🧠 核心数据结构](#-核心数据结构)
  - [⚡ 游戏主循环 (main函数)](#-游戏主循环-main函数)
  - [🎮 移动控制逻辑](#-移动控制逻辑)
  - [🤖 自动寻路算法](#-自动寻路算法)
  - [💥 碰撞检测系统](#-碰撞检测系统)
  - [🎨 渲染引擎优化](#-渲染引擎优化)
  - [🌈 主题系统实现](#-主题系统实现)
  - [🚀 性能优化技巧](#-性能优化技巧)
  - [⚠️ 已知限制](#️-已知限制)



# 配置

注：请使用 **GBK** 编码运行项目

## 在 VSCode 中配置 EasyX 图形库

- **安装 TDM-GCC**
    点击 [链接](https://github.com/jmeubank/tdm-gcc/releases/download/v9.2.0-tdm64-1/tdm64-gcc-9.2.0.exe) 下载并安装 **TDM-GCC**.

- **下载并解压 EasyX 图形库**
    点击 [链接](https://easyx.cn/download/easyx4mingw_20240225.zip) 获取压缩包.

- **链接文件**
    - EasyX 压缩包内有两个链接文件目录，分别是 **lib32** 以及 **lib64**，分别将这两个链接文件里的 **libeasyx.a** 文件复制到 **TDM-GCC** 的安装目录下的 **\TDM-GCC-64\x86_64-w64-mingw32\lib** 和 **\TDM-GCC-64\x86_64-w64-mingw32\lib32** 文件夹里：

        ![lib64](./figure/1.png)
        ![lib32](./figure/2.png)

    - 将 **include** 文件夹下的 **easyx.h** 和 **graphics.h** 拷贝到 **TDM-GCC** 的安装目录下的 **\TDM-GCC-64\x86_64-w64-mingw32\include** 文件夹里:
    
        ![include](./figure/3.png)

## VSCode 配置文件

- 安装必要插件 **C/C++**
- 在 VSCode 项目路径下新建 **.vscode** 目录，在该目录下新建 **tasks.json** 文件并填入以下内容：
    ```
    {
        "tasks": [
            {
                "type": "cppbuild",
                "label": "C/C++: g++.exe 生成活动文件",
                "command": "C:\\Program Files\\TDM-GCC\\bin\\g++.exe",
                "args": [
                    "-fdiagnostics-color=always",
                    "-g",
                    "${file}",
                    "-o",
                    "${fileDirname}\\${fileBasenameNoExtension}.exe",
                    "-L",  // 在库文件的搜索路径列表中添加dir目录，
                    "C:\\Program Files\\TDM-GCC\\x86_64-w64-mingw32\\lib",
                    "-leasyx"
                ],
                "options": {
                    "cwd": "${fileDirname}"
                },
                "problemMatcher": [
                    "$gcc"
                ],
                "group": {
                    "kind": "build",
                    "isDefault": true
                },
                "detail": "调试器生成的任务。"
            }
        ],
        "version": "2.0.0"
    }
    ```

# 游戏规则说明

## 🎯 游戏目标
- 通过操控蛇的移动不断吃掉随机生成的食物
- 每吃1个食物得10分，蛇身变长1节
- 避免碰撞墙壁和自身，尽可能获得更高分数

## 🕹️ 操作控制
| 按键               | 功能说明                  |
|--------------------|-------------------------|
| `W/↑`              | 向上移动                 |
| `S/↓`              | 向下移动                 |
| `A/←`              | 向左移动                 |
| `D/→`              | 向右移动                 |
| `空格键`           | 切换**自动寻路模式**     |
| `C`                | 更换游戏主题皮肤         |
| `R`                | 重新开始游戏             |
| `+/-`              | 调整网格大小(12-40像素)  |
| `F/G`              | 加速/减速(5个速度等级)   |

## ⚙️ 游戏机制
- **移动速度**  
  - 速度等级1(慢) ↔ 等级5(快)
  - 自动模式下AI会自动计算最短路径

- **主题系统**  
  - 7种不同颜色主题皮肤可选
  - 包含暗黑/霓虹/少女/金属等风格

- **网格系统**  
  - 动态调整网格密度(800x600分辨率)
  - 调整后游戏会自动重置

- **碰撞判定**  
  - 触碰到窗口边界立即失败
  - 蛇头接触身体任意部分失败

## 🏆 胜利与失败
- **失败条件**  
  ✅ 碰撞墙壁  
  ✅ 蛇头触碰身体  
  ❌ 本版本暂未设置胜利条件

## 🌟 特色功能
1. **智能辅助模式**  
   - 开启后AI自动寻路找食物
   - 仍可手动干预控制方向

2. **动态UI系统**  
   - 实时显示：得分/速度/主题/网格尺寸
   - 半透明信息面板+抗锯齿字体

3. **画面增强**  
   - 蛇身光晕特效
   - 食物立体投影
   - 渐变半透明网格线

4. **无障碍设计**  
   - 所有文字使用加粗黑体
   - 高对比度颜色方案
   - 关键信息放大显示

> 提示：建议先体验手动模式，熟悉操作后再尝试自动模式

# 关键模块

## 🧠 核心数据结构
```cpp
struct Snake {
    deque<pair<int, int>> body;  // 使用双端队列存储蛇身坐标
    Direction dir = RIGHT;       // 当前移动方向
};
```
- **deque容器**：支持前端插入新头坐标push_front()和尾部删除pop_back()
- **坐标系统**：基于网格坐标(非像素)，如(20,15)表示第20列第15行网格

## ⚡ 游戏主循环 (main函数)
```cpp
while(true) {
    // 1. 输入处理
    int key = GetKey();  // 自定义非阻塞输入检测
    
    // 2. 状态更新
    if(!gameover) update();  // 移动蛇和检测碰撞
    
    // 3. 画面渲染
    draw();          // 绘制所有游戏元素
    Sleep(baseSpeed); // 控制游戏速度
}
```

- **三阶段架构**：经典游戏循环模式（输入→更新→渲染）
- **速度控制**：`baseSpeed`从200ms(慢)到-255ms(快)线性变化

## 🎮 移动控制逻辑
```cpp
// 方向键处理（片段）
case 'W': case VK_UP:    
    if (snake.dir != DOWN) nextDir = UP; 
    break;
```

- **防反向移动**：通过检查当前方向避免180°急转
- **缓冲指令**：使用nextDir暂存输入，在更新时统一应用

## 🤖 自动寻路算法
```cpp
Direction findPath() {
    vector<vector<bool>> visited(cols, vector<bool>(rows, false));
    queue<pair<pair<int, int>, Direction>> q;  // BFS队列
    // ...
}
```

- BFS算法：广度优先搜索最短路径
- 障碍物检测：将蛇身坐标视为不可通过区域
- 方向继承：记录每个节点的初始移动方向用于返回结果

## 💥 碰撞检测系统
```cpp
// 边界检测
if (newHead.first < 0 || newHead.first >= cols || ...)
    gameover = true;

// 自碰检测
for (auto p : snake.body)
    if (p == newHead) gameover = true;
```

- 双重检测：先检查越界再检查自碰
- 高效遍历：遍历deque时使用引用避免拷贝


## 🎨 渲染引擎优化
```cpp
// 抗锯齿绘制（伪代码）
void drawAntiAliasedCircle(...) {
    IMAGE img(r*2+2, r*2+2);  // 创建临时画布
    // 颜色混合算法...
}
```

- **离屏绘制**：先在内存IMAGE操作再混合到屏幕
- **透明度混合**：使用`(orig*(1-rate) + new*rate)`公式实现alpha混合

## 🌈 主题系统实现
```cpp
vector<Theme> themes = {
    { HEAD_COLOR, BODY_COLOR, FOOD_COLOR, BG_COLOR, GRID_COLOR },
    // ...其他主题
};
```

- **结构体打包**：使用Theme结构封装颜色配置
- **循环切换**：`themeIndex = (themeIndex + 1) % themes.size()`

## 🚀 性能优化技巧
```cpp
BeginBatchDraw();  // 开启批量绘制
FlushBatchDraw();  // 统一刷新画面
```

- **批量绘制**：减少显卡调用次数
- **静态缓存**：对网格等不变元素使用预渲染

## ⚠️ 已知限制

- **路径查找缺陷**：BFS未考虑蛇尾移动带来的空位
- **闪烁问题**：快速移动时网格线可能闪烁
- **内存消耗**：每个抗锯齿圆创建临时IMAGE对象