#include <graphics.h>
#include <easyx.h>
#include <conio.h>
#include <time.h>

#pragma comment( lib, "MSIMG32.LIB")

#define WIDTH 800						// 支持的最大宽度
#define HEIGHT 600						// 支持的最大高度

int width = 40;							// 默认宽度
int height = 30;						// 默认高度
int size = 20;							// 实际绘画一格的步长

IMAGE menuImg;							// 菜单图片
char title[9][2] = { "-", "-", "S", "N", "A", "K", "E", "-", "-" };				// 游戏标题
char failure[10][2] = { "-", "-", "F", "A", "I", "L", "E", "D", "-", "-" };		// 游戏标题
int menuChoice = 0;						// 是否进入游戏
int Blocks[WIDTH][HEIGHT] = { 0 };		// 标记网格对应坐标处是否被占位，0 为无占位
char moveDirection, oldMoveDirection;	// 上一时刻与当前时刻的蛇头方向
int foodX, foodY;						// 食物出现位置坐标
int delay = 10;							// 延迟
int isFailure = 0;						// 游戏判负标志
int failureChoice = 0;					// 失败后选项标记

void initMenu();						// 初始化菜单
void drawMenu();						// 绘制菜单
void menuInput();						// 菜单状态下的输入
void initGame();						// 初始化游戏界面
void show();							// 每一次运行时重新绘制蛇和食物的图像
void updateWithInput();					// 与输入有关的更新
void updateWithoutInput();				// 与输入无关的更新
void moveSnake();						// 蛇运动
void drawFailure();						// 游戏失败处理面板
void failureInput();					// 失败状态下的输入
void transparentimage(IMAGE* dstimg, int x, int y, IMAGE* srcimg);					// 图片底色透明化处理

int main() 
{
	initgraph(WIDTH, HEIGHT);
	// 初始化菜单界面，不进入游戏将不转入后续过程
	initMenu();
	// 初始化游戏界面，画出食物、蛇体、地图
	initGame();
	while (1) 
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
	loadimage(&menuImg, "./menu.png");
	drawMenu();

	// 当没有选定游戏开始并按下空格时持续检查输入
	while (menuChoice != 2)
		menuInput();
}

void drawMenu()
{
	setbkcolor(0xD4E1E5);
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

	// 菜单的两个选项
	f.lfHeight = 40;
	f.lfWidth = 20;
	f.lfWeight = 1000;
	f.lfQuality = ANTIALIASED_QUALITY;
	settextstyle(&f);

	if (menuChoice == 0)
	{
		settextcolor(0x3E03FE);
		outtextxy(350, 340, _T("start"));
		settextcolor(0xFE0303);
		outtextxy(320, 400, _T("settings"));
	}
	else
	{
		settextcolor(0xFE0303);
		outtextxy(350, 340, _T("start"));
		settextcolor(0x3E03FE);
		outtextxy(320, 400, _T("settings"));
	}
}

void menuInput()
{
	if (_kbhit() && isFailure == 0)
	{
		char input = _getch();
		// 向上或向下选择
		if (input == 'w' && menuChoice == 1)
		{
			menuChoice = 1 - menuChoice;
			drawMenu();
		}
		else if (input == 's' && menuChoice == 0)
		{
			menuChoice = 1 - menuChoice;
			drawMenu();
		}
		// 空格选定开始
		else if (input == ' ' && menuChoice == 0)
		{
			menuChoice = 2;
		}
		// 空格选定游戏设置
		else if (input == ' ' && menuChoice == 1)
		{

		}
	}
}

void initGame() 
{
	// 设置背景色为 LIGHTGRAY，这并不是填充背景
	setbkcolor(LIGHTGRAY);
	BeginBatchDraw();
	// 使用当前背景色 LIGHTGRAY 清空屏幕内容
	cleardevice();
	// 设置线条颜色为 WHITE
	setlinecolor(WHITE);
	// 绘制线条，呈现网格效果
	for (int i = size; i < width * size; i += size)
		line(i, 0, i, height * size);
	for (int i = size; i < height * size; i += size)
		line(0, i, width * size, i);

	// 默认移动方向为右，蛇体产生在网格中间
	moveDirection = oldMoveDirection = 'd';
	Blocks[width / 2][height / 2] = 1;
	
	// 初始长度是 4
	for (int i = 1; i < 4; i++)
		Blocks[width / 2 - i][height / 2] = i + 1;
	
	// 网格内产生食物
	foodX = rand() % (width - 2) + 1;
	foodY = rand() % (height - 2) + 1;
}

void show() 
{
	// 屏幕刷新一轮
	for (int i = 0; i < width; i++)
		for (int j = 0; j < height; j++) 
		{
			if (Blocks[i][j] != 0)
				// 已被填充使用渐变色
				setfillcolor(HSVtoRGB(Blocks[i][j] * 10, 0.9, 1));
			else
				// 未被填充使用浅灰色
				setfillcolor(LIGHTGRAY);
			// 开始填充
			fillrectangle(i * size, j * size, (i + 1) * size, (j + 1) * size);
		}

	// 使用绿色填充食物
	setfillcolor(LIGHTGREEN);
	fillrectangle(foodX * size, foodY * size, (foodX + 1) * size, (foodY + 1) * size);

	// 判负后提示游戏失败
	if (isFailure)
	{
		drawFailure();
		isFailure = 0;
		while (failureChoice != 2)
			failureInput();
	}

	// 执行未完成的绘制任务
	FlushBatchDraw(); 
}

void drawFailure()
{
	setbkcolor(0xD4E1E5);
	cleardevice();

	LOGFONT f;
	gettextstyle(&f);
	f.lfHeight = 100;
	f.lfWidth = 50;
	strcpy_s(f.lfFaceName, _T("Consolas"));
	f.lfQuality = ANTIALIASED_QUALITY;
	settextcolor(BLACK);
	setbkmode(TRANSPARENT);
	settextstyle(&f);
	for (int i = 0; i < 10; i++)
	{
		settextcolor(HSVtoRGB((i + 1) * 10, 0.9, 1));
		outtextxy(150 + i * 50, 150, _T(failure[i]));
	}

	// 失败的两个选项
	f.lfHeight = 40;
	f.lfWidth = 20;
	f.lfWeight = 1000;
	f.lfQuality = ANTIALIASED_QUALITY;
	settextstyle(&f);

	if (failureChoice == 0)
	{
		settextcolor(0x3E03FE);
		outtextxy(330, 340, _T("restart"));
		settextcolor(0xFE0303);
		outtextxy(360, 400, _T("menu"));
	}
	else
	{
		settextcolor(0xFE0303);
		outtextxy(330, 340, _T("restart"));
		settextcolor(0x3E03FE);
		outtextxy(360, 400, _T("menu"));
	}
}

void failureInput()
{
	if (_kbhit() && isFailure == 1)
	{
		char input = _getch();
		// 向上或向下选择
		if (input == 'w' && failureChoice == 1)
		{
			failureChoice = 1 - failureChoice;
			drawFailure();
		}
		else if (input == 's' && failureChoice == 0)
		{
			failureChoice = 1 - failureChoice;
			drawFailure();
		}
		// 空格选定重新游戏
		else if (input == ' ' && failureChoice == 0)
		{
			// TODO
			failureChoice = 2;
		}
		// 空格选定菜单
		else if (input == ' ' && failureChoice == 1)
		{
			failureChoice = 2;
		}
	}
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
				moveDirection = 'd';
			else if (moveDirection == 's' && oldMoveDirection == 'w')
				moveDirection = 'w';
			else if (moveDirection == 'd' && oldMoveDirection == 'a')
				moveDirection = 'a';
			else if (moveDirection == 'w' && oldMoveDirection == 's')
				moveDirection = 's';
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
			newHeadX -= 1;
			break;
		case 's':
			newHeadY += 1;
			break;
		case 'd':
			newHeadX += 1;
			break;
		case 'w':
			newHeadY -= 1;
			break;
	}

	// 判负
	if (newHeadX >= width || newHeadX < 0 || newHeadY >= height || newHeadY < 0 || Blocks[newHeadX][newHeadY] != 0) 
	{
		isFailure = 1;
		return;
	}

	// 对蛇头的处理
	Blocks[newHeadX][newHeadY] = 1;
	
	// 对吃到食物以及蛇尾的处理
	if (newHeadX == foodX && newHeadY == foodY) 
	{
		foodX = rand() % (width - 2) + 1;
		foodY = rand() % (height - 2) + 1;
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