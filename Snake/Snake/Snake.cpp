#include <graphics.h>
#include <easyx.h>
#include <conio.h>
#include <time.h>

#define WIDTH 200						// 支持的最大宽度
#define HEIGHT 150						// 支持的最大高度

int width = 40;							// 默认宽度
int height = 30;						// 默认高度
int size = 20;							// 实际绘画一格的步长

int Blocks[WIDTH][HEIGHT] = { 0 };		// 标记网格对应坐标处是否被占位，0 为无占位
char moveDirection, oldMoveDirection;	// 上一时刻与当前时刻的蛇头方向
int foodX, foodY;						// 食物出现位置坐标
int delay = 10;							// 延迟
int isFailure = 0;						// 游戏判负标志

void initMenu();						// 初始化菜单
void initGame();						// 初始化游戏界面
void show();							// 每一次运行时重新绘制蛇和食物的图像
void updateWithInput();					// 与输入有关的更新
void updateWithoutInput();				// 与输入无关的更新
void moveSnake();						// 蛇运动

int main() 
{
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
	initgraph(800, 600);
	// TODO
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
		settextcolor(LIGHTBLUE);
		settextstyle(80, 0, _T("黑体"));
		setbkmode(TRANSPARENT);
		outtextxy(240, 220, _T("Failed"));
	}

	// 执行未完成的绘制任务
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