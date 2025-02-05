#include <cstdio>
#include <cstdlib>
#include <graphics.h>
#include <easyx.h>
#include <conio.h>
#include <time.h>
#pragma comment( lib, "MSIMG32.LIB")
#define WIDTH 800						// 支持的最大宽度
#define HEIGHT 600						// 支持的最大高度

// ---------------------------------------------------------------------------------------------------------- //
// ---------------------------------------------公共变量区域------------------------------------------------- //
// ---------------------------------------------------------------------------------------------------------- //
//																											| //
int width = 40;							// 默认宽度															| //
int height = 30;						// 默认高度															| //
int size = 20;							// 实际绘画一格的步长												| //
int skinIndex = 0;						// 皮肤编号															| //
char skin[4][10] = {"DEFAULT", "SIMPLE", "PUNK", "SEA"};								// 皮肤描述			| //
int backgroundColor[4] = { 0xD4E1E5, WHITE, 0x920094, 0x4954C2 };						// 背景颜色			| //
int lineColor[4] = { WHITE , WHITE, 0xDBB5D9, 0xB5B9DB };								// 线颜色			| //
int gridColor[4] = {LIGHTGRAY, BLACK, 0x8D3CAA, 0x4953AF };								// 网格颜色			| //
//																											| //
int gameOver = 0;						// 游戏结束标记														| //
IMAGE menuImg;							// 菜单图片															| //
char title[9][2] = { "-", "-", "S", "N", "A", "K", "E", "-", "-" };						// 游戏标题			| //
char failure[10][2] = { "-", "-", "F", "A", "I", "L", "E", "D", "-", "-" };				// 失败标题			| //
char win[10][2] = { "-", "-", "W", "I", "N", "-", "-" };								// 成功标题			| //
char settings[12][2] = { "-", "-", "S", "E", "T", "T", "I", "N", "G", "S", "-", "-" };	// 设置标题			| //
int menuChoice = 0;						// 是否进入游戏														| //
int settingsChoice = 0;					// 是否退出设置														| //
int Blocks[WIDTH][HEIGHT] = { 0 };		// 标记网格对应坐标处是否被占位，0 为无占位							| //
char moveDirection, oldMoveDirection;	// 上一时刻与当前时刻的蛇头方向										| //
int foodX, foodY;						// 食物出现位置坐标													| //
int score = 0;							// 分数																| //
int delay = 102;						// 刷新延迟															| //
int isFailure = 0;						// 游戏判胜负标志，游戏中为 0，负为 1，胜为 2						| //
int failureChoice = 0;					// 失败后选项标记													| //
//																											| //
char* int_to_string(int num);			// int 转 char 数组													| //
void initMenu();						// 初始化菜单界面													| //
void drawMenu();						// 绘制菜单界面														| //
void menuInput();						// 菜单界面下的输入													| //
void initSettings();					// 初始化设置界面													| //
void drawSettings();					// 绘制设置界面														| //
void settingsInput();					// 设置界面下的输入													| //
void initGame();						// 初始化游戏界面													| //
void show();							// 每一次运行时重新绘制蛇和食物的图像								| //
void updateWithInput();					// 与输入有关的更新													| //
void updateWithoutInput();				// 与输入无关的更新													| //
void moveSnake();						// 蛇运动															| //
void initFailure();						// 初始化失败界面													| //
void drawFailure();						// 绘制失败界面														| //
void failureInput();					// 失败界面下的输入													| //
void transparentimage(IMAGE* dstimg, int x, int y, IMAGE* srcimg);				// 图片底色透明化处理		| //
//																											| //
// ---------------------------------------------------------------------------------------------------------- //
// ---------------------------------------------公共变量区域------------------------------------------------- //
// ---------------------------------------------------------------------------------------------------------- //

int main() 
{
	initgraph(WIDTH, HEIGHT);
	// 初始化菜单界面，不进入游戏将不转入后续过程
	initMenu();
	// 初始化游戏界面，画出食物、蛇体、地图
	initGame();

	while (!gameOver)
	{
		// 刷新一轮
		show();
		// 被动事件改变状态
		updateWithoutInput();
		// 主动事件改变状态
		updateWithInput();
	}

	return 0;
}

void initMenu()
{
	loadimage(&menuImg, _T("PNG"), _T("MENU"));
	drawMenu();

	// 当没有选定触发项并按下空格时持续检查输入
	while (menuChoice != 3)
		menuInput();
	menuChoice = 0;
}

void drawMenu()
{
	setbkcolor(backgroundColor[skinIndex]);
	cleardevice();

	transparentimage(NULL, 6, 1, &menuImg);

	LOGFONT f;
	gettextstyle(&f);
	f.lfHeight = 100;
	f.lfWidth = 50;
	f.lfWeight = 1000;
	strcpy_s(f.lfFaceName, _T("Consolas"));
	f.lfQuality = ANTIALIASED_QUALITY;
	setbkmode(TRANSPARENT);
	settextstyle(&f);
	for (int i = 0; i < 9; i++)
	{
		settextcolor(HSVtoRGB((i + 1) * 10, 0.9, 1));
		outtextxy(175 + i * 50, 150, _T(title[i]));
	}

	// 提示按下空格选中
	f.lfHeight = 20;
	f.lfWidth = 10;
	settextstyle(&f);
	settextcolor(0x000000);
	outtextxy(285, 240, _T("(press space to choice)"));

	// 菜单界面的三个选项
	f.lfHeight = 40;
	f.lfWidth = 20;
	f.lfWeight = 1000;
	f.lfQuality = ANTIALIASED_QUALITY;
	settextstyle(&f);

	if (menuChoice == 0)
	{
		// 选中，鲜红
		settextcolor(0x3E03FE);
		outtextxy(350, 340, _T("start"));
		// 未选中，亮蓝
		settextcolor(0xFE0303);
		outtextxy(320, 400, _T("settings"));
		// 未选中，亮蓝
		settextcolor(0xFE0303);
		outtextxy(360, 460, _T("exit"));
	}
	else if(menuChoice == 1)
	{
		// 未选中，亮蓝
		settextcolor(0xFE0303);
		outtextxy(350, 340, _T("start"));
		// 选中，鲜红
		settextcolor(0x3E03FE);
		outtextxy(320, 400, _T("settings"));
		// 未选中，亮蓝
		settextcolor(0xFE0303);
		outtextxy(360, 460, _T("exit"));
	}
	else if (menuChoice == 2)
	{
		// 未选中，亮蓝
		settextcolor(0xFE0303);
		outtextxy(350, 340, _T("start"));
		// 未选中，亮蓝
		settextcolor(0xFE0303);
		outtextxy(320, 400, _T("settings"));
		// 选中，鲜红
		settextcolor(0x3E03FE);
		outtextxy(360, 460, _T("exit"));
	}
}

void menuInput()
{
	BeginBatchDraw();
	if (_kbhit())
	{
		char input = _getch();
		// 向上或向下选择
		if (input == 'w' && menuChoice != 0)
		{
			menuChoice--;
			drawMenu();
		}
		else if (input == 's' && menuChoice !=2)
		{
			menuChoice++;
			drawMenu();
		}
		// 空格选定开始
		else if (input == ' ' && menuChoice == 0)
		{
			menuChoice = 3;
		}
		// 空格选定游戏设置
		else if (input == ' ' && menuChoice == 1)
		{
			initSettings();
			drawMenu();
		}
		// 空格选定退出游戏
		else if (input == ' ' && menuChoice == 2)
		{
			menuChoice = 3;
			gameOver = 1;
		}
	}
	FlushBatchDraw();
}

void initSettings()
{
	drawSettings();
	while (settingsChoice != 6)
		settingsInput();
	settingsChoice = 0;
}

void drawSettings()
{
	setbkcolor(backgroundColor[skinIndex]);
	cleardevice();

	LOGFONT f;
	gettextstyle(&f);
	f.lfHeight = 100;
	f.lfWidth = 50;
	f.lfWeight = 1000;
	strcpy_s(f.lfFaceName, _T("Consolas"));
	f.lfQuality = ANTIALIASED_QUALITY;
	setbkmode(TRANSPARENT);
	settextstyle(&f);
	for (int i = 0; i < 12; i++)
	{
		settextcolor(HSVtoRGB((i + 1) * 10, 0.9, 1));
		outtextxy(100 + i * 50, 80, _T(settings[i]));
	}

	// 提示按下空格选中
	f.lfHeight = 20;
	f.lfWidth = 10;
	settextstyle(&f);
	settextcolor(0x000000);
	outtextxy(260, 170, _T("(press 'a' or 'd' to change)"));

	// 设置界面的四个选项
	f.lfHeight = 40;
	f.lfWidth = 20;
	f.lfWeight = 1000;
	f.lfQuality = ANTIALIASED_QUALITY;
	settextstyle(&f);
	
	char widthString[10], heightString[10], gridSizeString[10],  delayString[10];

	sprintf_s(widthString, "%d", width);
	sprintf_s(heightString, "%d", height);
	sprintf_s(gridSizeString, "%d", size);
	sprintf_s(delayString, "%d", 10002 - delay);


	if (settingsChoice == 0)
	{
		// 选中，鲜红
		settextcolor(0x3E03FE);
		outtextxy(230, 230, _T("width    >>  ")), outtextxy(490, 230, _T(widthString));
		// 未选中，亮蓝
		settextcolor(0xFE0303);
		outtextxy(230, 290, _T("height   >>  ")), outtextxy(490, 290, _T(heightString));
		// 未选中，亮蓝
		settextcolor(0xFE0303);
		outtextxy(230, 350, _T("grid size>>  ")), outtextxy(490, 350, _T(gridSizeString));
		// 未选中，亮蓝
		settextcolor(0xFE0303);
		outtextxy(230, 410, _T("speed    >>  ")), outtextxy(490, 410, _T(delayString));
		// 未选中，亮蓝
		settextcolor(0xFE0303);
		outtextxy(230, 470, _T("skin     >>  ")), outtextxy(490, 470, _T(skin[skinIndex]));
		// 未选中，亮蓝
		settextcolor(0xFE0303);
		outtextxy(360, 530, _T("back"));
	}
	else if (settingsChoice == 1)
	{
		// 未选中，亮蓝
		settextcolor(0xFE0303);
		outtextxy(230, 230, _T("width    >>  ")), outtextxy(490, 230, _T(widthString));
		// 选中，鲜红
		settextcolor(0x3E03FE);
		outtextxy(230, 290, _T("height   >>  ")), outtextxy(490, 290, _T(heightString));
		// 未选中，亮蓝
		settextcolor(0xFE0303);
		outtextxy(230, 350, _T("grid size>>  ")), outtextxy(490, 350, _T(gridSizeString));
		// 未选中，亮蓝
		settextcolor(0xFE0303);
		outtextxy(230, 410, _T("speed    >>  ")), outtextxy(490, 410, _T(delayString));
		// 未选中，亮蓝
		settextcolor(0xFE0303);
		outtextxy(230, 470, _T("skin     >>  ")), outtextxy(490, 470, _T(skin[skinIndex]));
		// 未选中，亮蓝
		settextcolor(0xFE0303);
		outtextxy(360, 530, _T("back"));
	}
	else if (settingsChoice == 2)
	{
		// 未选中，亮蓝
		settextcolor(0xFE0303);
		outtextxy(230, 230, _T("width    >>  ")), outtextxy(490, 230, _T(widthString));
		// 未选中，亮蓝
		settextcolor(0xFE0303);
		outtextxy(230, 290, _T("height   >>  ")), outtextxy(490, 290, _T(heightString));
		// 选中，鲜红
		settextcolor(0x3E03FE);
		outtextxy(230, 350, _T("grid size>>  ")), outtextxy(490, 350, _T(gridSizeString));
		// 未选中，亮蓝
		settextcolor(0xFE0303);
		outtextxy(230, 410, _T("speed    >>  ")), outtextxy(490, 410, _T(delayString));
		// 未选中，亮蓝
		settextcolor(0xFE0303);
		outtextxy(230, 470, _T("skin     >>  ")), outtextxy(490, 470, _T(skin[skinIndex]));
		// 未选中，亮蓝
		settextcolor(0xFE0303);
		outtextxy(360, 530, _T("back"));
	}
	else if (settingsChoice == 3)
	{
		// 未选中，亮蓝
		settextcolor(0xFE0303);
		outtextxy(230, 230, _T("width    >>  ")), outtextxy(490, 230, _T(widthString));
		// 未选中，亮蓝
		settextcolor(0xFE0303);
		outtextxy(230, 290, _T("height   >>  ")), outtextxy(490, 290, _T(heightString));
		// 未选中，亮蓝
		settextcolor(0xFE0303);
		outtextxy(230, 350, _T("grid size>>  ")), outtextxy(490, 350, _T(gridSizeString));
		// 选中，鲜红
		settextcolor(0x3E03FE);
		outtextxy(230, 410, _T("speed    >>  ")), outtextxy(490, 410, _T(delayString));
		// 未选中，亮蓝
		settextcolor(0xFE0303);
		outtextxy(230, 470, _T("skin     >>  ")), outtextxy(490, 470, _T(skin[skinIndex]));
		// 未选中，亮蓝
		settextcolor(0xFE0303);
		outtextxy(360, 530, _T("back"));
	}
	else if (settingsChoice == 4)
	{
		// 未选中，亮蓝
		settextcolor(0xFE0303);
		outtextxy(230, 230, _T("width    >>  ")), outtextxy(490, 230, _T(widthString));
		// 未选中，亮蓝
		settextcolor(0xFE0303);
		outtextxy(230, 290, _T("height   >>  ")), outtextxy(490, 290, _T(heightString));
		// 未选中，亮蓝
		settextcolor(0xFE0303);
		outtextxy(230, 350, _T("grid size>>  ")), outtextxy(490, 350, _T(gridSizeString));
		// 未选中，亮蓝
		settextcolor(0xFE0303);
		outtextxy(230, 410, _T("speed    >>  ")), outtextxy(490, 410, _T(delayString));
		// 选中，鲜红
		settextcolor(0x3E03FE);
		outtextxy(230, 470, _T("skin     >>  ")), outtextxy(490, 470, _T(skin[skinIndex]));
		// 未选中，亮蓝
		settextcolor(0xFE0303);
		outtextxy(360, 530, _T("back"));
	}
	else if (settingsChoice == 5)
	{
		// 未选中，亮蓝
		settextcolor(0xFE0303);
		outtextxy(230, 230, _T("width    >>  ")), outtextxy(490, 230, _T(widthString));
		// 未选中，亮蓝
		settextcolor(0xFE0303);
		outtextxy(230, 290, _T("height   >>  ")), outtextxy(490, 290, _T(heightString));
		// 未选中，亮蓝
		settextcolor(0xFE0303);
		outtextxy(230, 350, _T("grid size>>  ")), outtextxy(490, 350, _T(gridSizeString));
		// 未选中，亮蓝
		settextcolor(0xFE0303);
		outtextxy(230, 410, _T("speed    >>  ")), outtextxy(490, 410, _T(delayString));
		// 未选中，亮蓝
		settextcolor(0xFE0303);
		outtextxy(230, 470, _T("skin     >>  ")), outtextxy(490, 470, _T(skin[skinIndex]));
		// 选中，鲜红
		settextcolor(0x3E03FE);
		outtextxy(360, 530, _T("back"));
	}
}

void settingsInput()
{
	BeginBatchDraw();

	if (_kbhit())
	{
		char input = _getch();
		// 向上或向下选择
		if (input == 'w' && settingsChoice != 0)
		{
			settingsChoice--;
			drawSettings();
		}
		else if (input == 's' && settingsChoice != 5)
		{
			settingsChoice++;
			drawSettings();
		}
		// 修改宽度属性
		else if ((input == 'a' || input == 'd') && settingsChoice == 0)
		{
			if (input == 'a' && width > 3)
			{
				width--;
				drawSettings();
			}
			else if (input == 'd' && width < 40 && (size * width < WIDTH && size * height < HEIGHT))
			{
				width++;
				drawSettings();
			}
		}
		// 修改高度属性
		else if ((input == 'a' || input == 'd') && settingsChoice == 1)
		{
			if (input == 'a' && height > 3)
			{
				height--;
				drawSettings();
			}
			else if (input == 'd' && height < 30 && (size * width < WIDTH && size * height < HEIGHT))
			{
				height++;
				drawSettings();
			}
		}
		// 修改网格大小属性
		else if ((input == 'a' || input == 'd') && settingsChoice == 2)
		{
			if (input == 'a' && size > 1)
			{
				size--;
				drawSettings();
			}
			else if (input == 'd' && (size * width < WIDTH && size * height < HEIGHT))
			{
				size++;
				drawSettings();
			}
		}
		// 修改移速属性
		else if ((input == 'a' || input == 'd') && settingsChoice == 3)
		{
			if (input == 'a' && delay < 10000)
			{
				delay += 100;
				drawSettings();
			}
			else if(input == 'd' && delay > 2)
			{
				delay -=100;
				drawSettings();
			}
		}
		// 修改皮肤属性
		else if ((input == 'a' || input == 'd') && settingsChoice == 4)
		{
			if (input == 'a' && skinIndex > 0)
			{
				skinIndex--;
				drawSettings();
			}
			else if(input == 'd' && skinIndex < 3)
			{
				skinIndex++;
				drawSettings();
			}
		}
		// 空格选定退出游戏
		else if (input == ' ' && settingsChoice == 5)
			settingsChoice = 6;
	}

	FlushBatchDraw();
}

void initGame()
{
	// 清空游戏记录很重要，失败重启时不能有任何以往记录
	score = 0, memset(Blocks, 0, sizeof Blocks);
	// 设置背景色为 LIGHTGRAY，这并不是填充背景
	setbkcolor(backgroundColor[skinIndex]);
	// 使用当前背景色 LIGHTGRAY 清空屏幕内容
	cleardevice();
	// 设置线条颜色为 WHITE
	setlinecolor(lineColor[skinIndex]);
	// 绘制线条，呈现网格效果
	for (int i = size; i < height * size; i += size)
		line((WIDTH - width * size) / 2 + i, (HEIGHT - height * size) / 2, (WIDTH - width * size) / 2 + i, (HEIGHT - height * size) / 2 + height * size);
	for (int i = size; i < width * size; i += size)
		line((WIDTH - width * size) / 2, (HEIGHT - height * size) / 2 + i, (WIDTH - width * size) / 2 + width * size, (HEIGHT - height * size) / 2 + i);

	// 默认移动方向为右
	moveDirection = oldMoveDirection = 'd';
	// 初始长度是 1
	Blocks[rand() % width][rand() % height] = 1;

	// 网格内产生食物
	while (Blocks[foodX][foodY] != 0)
	{
		foodX = rand() % width;
		foodY = rand() % height;
	}
}

void show() 
{
	BeginBatchDraw();
	// 屏幕刷新一轮
	for (int i = 0; i < width; i++)
		for (int j = 0; j < height; j++) 
		{
			if (Blocks[i][j] != 0)
				// 已被填充使用渐变色
				setfillcolor(HSVtoRGB(Blocks[i][j] * 10, 0.9, 1));
			else
				// 未被填充使用浅灰色
				setfillcolor(gridColor[skinIndex]);
			// 开始填充
			fillrectangle((WIDTH - width * size) / 2 + i * size, (HEIGHT - height * size) / 2 + j * size, (WIDTH - width * size) / 2 + (i + 1) * size, (HEIGHT - height * size) / 2 + (j + 1) * size);
		}

	// 使用绿色填充食物
	setfillcolor(LIGHTGREEN);
	fillrectangle((WIDTH - width * size) / 2 + foodX * size, (HEIGHT - height * size) / 2 + foodY * size, (WIDTH - width * size) / 2 + (foodX + 1) * size, (HEIGHT - height * size) / 2 + (foodY + 1) * size);

	// 执行未完成的绘制任务
	FlushBatchDraw();

	// 判负流程
	if (isFailure)
	{
		initFailure();
		initGame();
	}
}

void initFailure()
{
	drawFailure();
	while (failureChoice != 3)
		failureInput();
	isFailure = 0;
	failureChoice = 0;
}

void drawFailure()
{
	setbkcolor(backgroundColor[skinIndex]);
	cleardevice();

	LOGFONT f;
	gettextstyle(&f);
	f.lfHeight = 100;
	f.lfWidth = 50;
	strcpy_s(f.lfFaceName, _T("Consolas"));
	f.lfQuality = ANTIALIASED_QUALITY;
	setbkmode(TRANSPARENT);
	settextstyle(&f);
	if(isFailure == 1)
		for (int i = 0; i < 10; i++)
		{
			settextcolor(HSVtoRGB((i + 1) * 10, 0.9, 1));
			outtextxy(150 + i * 50, 150, _T(failure[i]));
		}
	else
		for (int i = 0; i < 7; i++)
		{
			settextcolor(HSVtoRGB((i + 1) * 10, 0.9, 1));
			outtextxy(225 + i * 50, 150, _T(win[i]));
		}

	f.lfHeight = 20;
	f.lfWidth = 10;
	settextcolor(BLACK);
	settextstyle(&f);
	outtextxy(5, 5, _T("SCORE: "));
	if (score > 0)
	{
		char scoreString[10];
		sprintf_s(scoreString, "%d", score);
		outtextxy(75, 5, _T(scoreString));
	}
	else
		outtextxy(75, 5, _T("0"));

	// 失败界面的三个选项
	f.lfHeight = 40;
	f.lfWidth = 20;
	f.lfWeight = 1000;
	f.lfQuality = ANTIALIASED_QUALITY;
	settextstyle(&f);

	if (failureChoice == 0)
	{
		// 选中，鲜红
		settextcolor(0x3E03FE);
		outtextxy(330, 340, _T("restart"));
		// 未选中，亮蓝
		settextcolor(0xFE0303);
		outtextxy(360, 400, _T("menu"));
		// 未选中，亮蓝
		settextcolor(0xFE0303);
		outtextxy(360, 460, _T("exit"));
	}
	else if(failureChoice == 1)
	{
		// 未选中，亮蓝
		settextcolor(0xFE0303);
		outtextxy(330, 340, _T("restart"));
		// 选中，鲜红
		settextcolor(0x3E03FE);
		outtextxy(360, 400, _T("menu"));
		// 未选中，亮蓝
		settextcolor(0xFE0303);
		outtextxy(360, 460, _T("exit"));
	}
	else if (failureChoice == 2)
	{
		// 未选中，亮蓝
		settextcolor(0xFE0303);
		outtextxy(330, 340, _T("restart"));
		// 未选中，亮蓝
		settextcolor(0xFE0303);
		outtextxy(360, 400, _T("menu"));
		// 选中，鲜红
		settextcolor(0x3E03FE);
		outtextxy(360, 460, _T("exit"));
	}
}

void failureInput()
{
	BeginBatchDraw();

	if (_kbhit() && isFailure == 0)
	{
		char input = _getch();
		// 向上或向下选择
		if (input == 'w' && failureChoice != 0)
		{
			failureChoice--;
			drawFailure();
		}
		else if (input == 's' && failureChoice != 2)
		{
			failureChoice++;
			drawFailure();
		}
		// 空格选定重新游戏
		else if (input == ' ' && failureChoice == 0)
		{
			failureChoice = 3;
			initGame();
		}
		// 空格选定菜单
		else if (input == ' ' && failureChoice == 1)
		{
			failureChoice = 3;
			initMenu();
		}
		// 空格选定退出游戏
		else if (input == ' ' && failureChoice == 2)
		{
			failureChoice = 3;
			gameOver = 1;
		}
	}

	FlushBatchDraw();
}

void updateWithoutInput()
{
	// 非判负状态下执行
	if (isFailure)
		return;
	// 等待 6 轮，可控制速度
	static int waitIndex = 1;
	waitIndex++;
	if (waitIndex == delay)
	{
		// 移动一次蛇体
		moveSnake();
		waitIndex = 1;
	}
}

void updateWithInput() 
{
	// _kbhit() 检查是否有键按下
	if (_kbhit() && isFailure == 0)
	{
		// 获取一次输入，根据输入字符转换蛇体移动方向
		char input = _getch();
		if (input == 'a' || input == 's' || input == 'd' || input == 'w') 
		{
			moveDirection = input;
			if (moveDirection == 'a' && oldMoveDirection == 'd')
				moveDirection = 'a';
			else if (moveDirection == 's' && oldMoveDirection == 'w')
				moveDirection = 's';
			else if (moveDirection == 'd' && oldMoveDirection == 'a')
				moveDirection = 'd';
			else if (moveDirection == 'w' && oldMoveDirection == 's')
				moveDirection = 'w';
			else oldMoveDirection = input;
			moveSnake();
		}
		else if (input == '!')
		{
			// TODO: 自动模式
		}
	}
}

void moveSnake()
{
	int i, j;
	// 对蛇身更新
	for (i = 0; i < width; i++)
		for (j = 0; j < height; j++)
			if (Blocks[i][j] != 0)
				Blocks[i][j]++;

	// 寻找旧蛇头和旧蛇尾的坐标
	int oldHeadX = 0, oldHeadY = 0, oldTailX = 0, oldTailY = 0;
	int tailBlocks = 0;

	for (i = 0; i < width; i++)
		for (j = 0; j < height; j++)
		{
			if (tailBlocks < Blocks[i][j])
			{
				tailBlocks = Blocks[i][j];
				oldTailX = i;
				oldTailY = j;
			}
			if (Blocks[i][j] == 2)
			{
				oldHeadX = i;
				oldHeadY = j;
			}
		}

	// 根据蛇的移动方向来赋值新蛇头的xy坐标
	int newHeadX = oldHeadX, newHeadY = oldHeadY;
	switch (moveDirection)
	{
	case 'a':
		newHeadX = (newHeadX - 1 + width) % width;
		break;
	case 's':
		newHeadY = (newHeadY + 1) % height;
		break;
	case 'd':
		newHeadX = (newHeadX + 1) % width;
		break;
	case 'w':
		newHeadY = (newHeadY - 1 + height) % height;
		break;
	}

	// 判胜
	bool flag = 1;
	for (int i = 0; i < width; i++)
		for (int j = 0; j < height; j++)
			if (Blocks[i][j] == 0)
			{
				flag = 0;
				break;
			}
	if (flag)
	{
		isFailure = 2;
		return;
	}

	// 判负
	if (Blocks[newHeadX][newHeadY] != 0) 
	{
		isFailure = 1;
		return;
	}

	// 对蛇头的处理
	Blocks[newHeadX][newHeadY] = 1;
	
	// 对吃到食物以及蛇尾的处理
	if (newHeadX == foodX && newHeadY == foodY) 
	{
		score++;
		while (Blocks[foodX][foodY] != 0)
		{
			foodX = rand() % width;
			foodY = rand() % height;
		}
	}
	else
		Blocks[oldTailX][oldTailY] = 0;
}

void transparentimage(IMAGE* dstimg, int x, int y, IMAGE* srcimg)
{
	HDC dstDC = GetImageHDC(dstimg);
	HDC srcDC = GetImageHDC(srcimg);
	int w = srcimg->getwidth();
	int h = srcimg->getheight();
	BLENDFUNCTION bf = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
	AlphaBlend(dstDC, x, y, w, h, srcDC, 0, 0, w, h, bf);
}

char* int_to_string(int num)
{
	int digits = 0;
	int temp = num;
	while (temp > 0)
	{
		digits++;
		temp /= 10;
	}

	char* str = (char*)malloc(digits + 2);

	if (str == NULL)
		return NULL;

	str[digits] = '\0';

	while (num > 0)
	{
		str[--digits] = (char)(num % 10 + '0');
		num /= 10;
	}

	return str;
}