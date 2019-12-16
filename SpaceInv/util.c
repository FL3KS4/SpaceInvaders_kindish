#include "util.h"

int maxx(int num1, int num2)
{
	return (num1 > num2) ? num1 : num2;
}


int minn(int num1, int num2)
{
	return (num1 > num2) ? num2 : num1;
}

int collision(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2)
{
	return (maxx(x1, x2) < minn(x1 + w1, x2 + w2)) && (maxx(y1, y2) < minn(y1 + h1, y2 + h2));
}


