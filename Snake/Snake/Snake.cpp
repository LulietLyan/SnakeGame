#include <graphics.h>
#include <easyx.h>
#include <deque>
#include <queue>
#include <vector>
#include <ctime>
using namespace std;

// 自定义颜色定义
#define DARKGREEN RGB(0,100,0)
#define PURPLE RGB(128,0,128)
#define ORANGE RGB(255,165,0)

// 窗口和游戏设置
const int WIDTH = 800;
const int HEIGHT = 600;
int gridSize = 20;
int cols = WIDTH / gridSize;
int rows = HEIGHT / gridSize;

// 移动速度
int baseSpeed = 200;    // 基础速度（毫秒）
int speedLevel = 3;      // 当前速度等级（1-5）
const int MAX_SPEED = 5; // 最大速度等级
const int MIN_SPEED = 1; // 最小速度等级

// 颜色主题结构体
struct Theme
{
    COLORREF head;
    COLORREF body;
    COLORREF food;
    COLORREF bg;
    COLORREF grid;
};

vector<Theme> themes = {
    { GREEN,  DARKGREEN,   RED,      RGB(240,240,240), LIGHTGRAY },
    { PURPLE, RGB(255,0,255), ORANGE,   RGB(230,230,250), RGB(200,200,200) },
    { CYAN,   BLUE,        RED,      RGB(173,216,230), RGB(150,150,150) },
    { RGB(255,105,180), RGB(255,182,193), ORANGE,   RGB(255,250,240), RGB(170,170,170) },
    { RGB(147,112,219), RGB(216,191,216), RGB(0,255,0), RGB(230,230,250), RGB(200,200,200) },  // 紫罗兰主题
    { RGB(255,99,71),  RGB(255,160,122), RGB(0,206,209), RGB(245,245,220), RGB(150,150,150) }  // 珊瑚色主题
};
int themeIndex = 0;

enum Direction { STOP, UP, DOWN, LEFT, RIGHT };
Direction nextDir = STOP;

struct Snake
{
    deque<pair<int, int>> body;
    Direction dir = RIGHT;
} snake;

pair<int, int> food;
int score = 0;
bool autoMode = false;
bool gameover = false;

// 键盘输入处理（替代kbhit/getch）
bool KeyPressed(int vkey)
{
    return GetAsyncKeyState(vkey) & 0x8000;
}

int GetKey()
{
    static int keyMap[] = {
        'W', VK_UP,
        'S', VK_DOWN,
        'A', VK_LEFT,
        'D', VK_RIGHT,
        VK_SPACE, 'C', 'R', VK_OEM_PLUS, VK_OEM_MINUS,
        'F', 'G'
    };

    for (int i = 0; i < sizeof(keyMap) / sizeof(int); i++)
    {
        if (KeyPressed(keyMap[i]))
            return keyMap[i];
    }
    return 0;
}

// 初始化游戏
void initGame()
{
    snake.body.clear();
    snake.body.push_back({ cols / 2, rows / 2 });
    snake.dir = RIGHT;
    nextDir = STOP;
    gameover = false;
    score = 0;

    // 生成食物
    while (true)
    {
        food = { rand() % cols, rand() % rows };
        bool onSnake = false;
        for (auto& p : snake.body)
            if (p == food) onSnake = true;
        if (!onSnake) break;
    }
}

// 自动寻路
Direction findPath()
{
    vector<vector<bool>> visited(cols, vector<bool>(rows, false));
    queue<pair<pair<int, int>, Direction>> q;
    auto head = snake.body.front();

    // 障碍物集合（蛇身）
    vector<pair<int, int>> obstacles;
    for (auto p : snake.body) obstacles.push_back(p);

    // 初始化队列
    vector<Direction> dirs = { UP, DOWN, LEFT, RIGHT };
    for (auto dir : dirs)
    {
        int dx = (dir == LEFT) ? -1 : (dir == RIGHT) ? 1 : 0;
        int dy = (dir == UP) ? -1 : (dir == DOWN) ? 1 : 0;
        int nx = head.first + dx;
        int ny = head.second + dy;

        if (nx >= 0 && nx < cols && ny >= 0 && ny < rows)
        {
            bool valid = true;
            for (auto p : obstacles)
                if (p.first == nx && p.second == ny) valid = false;

            if (valid)
            {
                q.push({ {nx, ny}, dir });
                visited[nx][ny] = true;
            }
        }
    }

    // BFS搜索
    while (!q.empty())
    {
        auto current = q.front();
        q.pop();

        if (current.first == food)
            return current.second;

        for (auto dir : dirs)
        {
            int dx = (dir == LEFT) ? -1 : (dir == RIGHT) ? 1 : 0;
            int dy = (dir == UP) ? -1 : (dir == DOWN) ? 1 : 0;
            int nx = current.first.first + dx;
            int ny = current.first.second + dy;

            if (nx >= 0 && nx < cols && ny >= 0 && ny < rows && !visited[nx][ny])
            {
                bool valid = true;
                for (auto p : obstacles)
                    if (p.first == nx && p.second == ny) valid = false;

                if (valid)
                {
                    visited[nx][ny] = true;
                    q.push({ {nx, ny}, current.second });
                }
            }
        }
    }
    return snake.dir;
}

// 游戏更新
void update()
{
    if (gameover) return;

    // 自动寻路逻辑
    if (autoMode)
    {
        Direction aiDir = findPath();
        if (aiDir != snake.dir && !(snake.dir == UP && aiDir == DOWN) &&
            !(snake.dir == DOWN && aiDir == UP) &&
            !(snake.dir == LEFT && aiDir == RIGHT) &&
            !(snake.dir == RIGHT && aiDir == LEFT))
        {
            snake.dir = aiDir;
        }
    }

    // 移动蛇头
    pair<int, int> newHead = snake.body.front();
    switch (snake.dir)
    {
    case UP:    newHead.second--; break;
    case DOWN:  newHead.second++; break;
    case LEFT:  newHead.first--;  break;
    case RIGHT: newHead.first++;  break;
    }

    // 碰撞检测
    if (newHead.first < 0 || newHead.first >= cols ||
        newHead.second < 0 || newHead.second >= rows)
    {
        gameover = true;
        return;
    }
    for (auto p : snake.body)
    {
        if (p == newHead)
        {
            gameover = true;
            return;
        }
    }

    // 添加新头
    snake.body.push_front(newHead);

    // 吃食物检测
    if (newHead == food)
    {
        score += 10;
        while (true)
        {
            food = { rand() % cols, rand() % rows };
            bool valid = true;
            for (auto p : snake.body)
                if (p == food) valid = false;
            if (valid) break;
        }
    }
    else
    {
        snake.body.pop_back();
    }
}

// 绘制界面
void draw()
{
    cleardevice();

    // 绘制背景和网格
    setbkcolor(themes[themeIndex].bg);
    cleardevice();
    setlinecolor(themes[themeIndex].grid);
    for (int i = 0; i <= cols; i++)
        line(i * gridSize, 0, i * gridSize, HEIGHT);
    for (int j = 0; j <= rows; j++)
        line(0, j * gridSize, WIDTH, j * gridSize);

    // 绘制蛇
    bool isHead = true;
    for (auto p : snake.body)
    {
        setfillcolor(isHead ? themes[themeIndex].head : themes[themeIndex].body);
        solidrectangle(p.first * gridSize + 1, p.second * gridSize + 1,
            (p.first + 1) * gridSize - 1, (p.second + 1) * gridSize - 1);
        isHead = false;
    }

    // 绘制食物
    setfillcolor(themes[themeIndex].food);
    solidrectangle(food.first * gridSize + 1, food.second * gridSize + 1,
        (food.first + 1) * gridSize - 1, (food.second + 1) * gridSize - 1);

    // 设置字体样式
    static LOGFONT f;
    gettextstyle(&f);
    f.lfHeight = 20;
    f.lfWeight = FW_BOLD;
    f.lfQuality = ANTIALIASED_QUALITY;  // 启用字体抗锯齿
    _tcscpy_s(f.lfFaceName, _T("黑体"));
    settextstyle(&f);

    // 绘制UI信息（带半透明背景）
    settextcolor(RGB(0, 0, 0));  // 黑色字体
    setbkmode(TRANSPARENT);

    // 绘制半透明背景
    COLORREF bgColor = themes[themeIndex].bg;
    double blendRatio = 0.5; // 50%透明度
    COLORREF blendedColor = RGB(
        (GetRValue(bgColor) * (1 - blendRatio) + 255 * blendRatio),
        (GetGValue(bgColor) * (1 - blendRatio) + 255 * blendRatio),
        (GetBValue(bgColor) * (1 - blendRatio) + 255 * blendRatio)
    );
    setfillcolor(blendedColor);

    char info[128];
    int yPos = 10;
    sprintf_s(info, "Score: %d", score);
    outtextxy(10, yPos, info); yPos += 25;

    sprintf_s(info, "Grid: %d", gridSize);
    outtextxy(10, yPos, info); yPos += 25;

    sprintf_s(info, "Theme: %d", themeIndex + 1);
    outtextxy(10, yPos, info); yPos += 25;

    sprintf_s(info, "Auto: %s", autoMode ? "On" : "Off");
    outtextxy(10, yPos, info); yPos += 25;

    sprintf_s(info, "Speed: %d/5", speedLevel);
    outtextxy(10, yPos, info);

    if (gameover)
    {
        settextcolor(RED);
        settextstyle(40, 0, _T("黑体"));
        outtextxy(WIDTH / 2 - 120, HEIGHT / 2 - 30, "Game Over!");
    }

    FlushBatchDraw();
}

int main()
{
    initgraph(WIDTH, HEIGHT);
    BeginBatchDraw();
    srand(time(0));
    initGame();

    while (true)
    {
        int key = GetKey();
        if (key)
        {
            switch (toupper(key))
            {
            case 'W': case VK_UP:    if (snake.dir != DOWN) nextDir = UP; break;
            case 'S': case VK_DOWN:  if (snake.dir != UP) nextDir = DOWN; break;
            case 'A': case VK_LEFT:  if (snake.dir != RIGHT) nextDir = LEFT; break;
            case 'D': case VK_RIGHT: if (snake.dir != LEFT) nextDir = RIGHT; break;
            case VK_SPACE: autoMode = !autoMode; break;
            case 'C': themeIndex = (themeIndex + 1) % themes.size(); break;
            case VK_OEM_PLUS:  // +号
                if (gridSize < 40)
                {
                    gridSize += 4;
                    cols = WIDTH / gridSize;
                    rows = HEIGHT / gridSize;
                    initGame();
                }
                break;
            case VK_OEM_MINUS: // -号
                if (gridSize > 12)
                {
                    gridSize -= 4;
                    cols = WIDTH / gridSize;
                    rows = HEIGHT / gridSize;
                    initGame();
                }
                break;
            case 'R': initGame(); break;
            case 'F': // 加速
                if (speedLevel < MAX_SPEED)
                {
                    speedLevel++;
                    baseSpeed = 200 - (speedLevel - 3) * 95; // 修正计算公式
                }
                break;
            case 'G': // 减速
                if (speedLevel > MIN_SPEED)
                {
                    speedLevel--;
                    baseSpeed = 200 - (speedLevel - 3) * 95; // 修正计算公式
                }
                break;
            default: break;
            }
            if (nextDir != STOP) snake.dir = nextDir;
        }

        if (!gameover) update();
        Sleep(baseSpeed);
        draw(); 
        if (gameover) initGame();
    }

    EndBatchDraw();
    closegraph();
    return 0;
}