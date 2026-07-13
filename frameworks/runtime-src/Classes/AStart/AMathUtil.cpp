#include "AMathUtil.h"

/**
* 根据两点确定这两点连线的二元一次方程 y = ax + b或者 x = ay + b
* @param ponit1
* @param point2
* @param type		指定返回函数的形式。为0则根据x值得到y，为1则根据y得到x
*
* @return 由参数中两点确定的直线的二元一次函数
*/
sel_callfuncx AMathUtil::getLineFunc(Point p1, Point p2, int type)
{
	sel_callfuncx resultFuc;
	// 先考虑两点在一条垂直于坐标轴直线的情况，此时直线方程为 y = a 或者 x = a 的形式
	if (p1.x == p2.x)
	{
		if (type == 1)
		{
			resultFuc = AMathUtil::getLineFunc1;
		}
		return resultFuc;
	}
	else if (p1.y == p2.y)
	{
		if (type==0)
		{
			resultFuc = AMathUtil::getLineFunc2;
		}
		return resultFuc;
	}

	if (type==0)
	{
		resultFuc = AMathUtil::getLineFunc3;
	}
	else if (type==1)
	{
		resultFuc = AMathUtil::getLineFunc4;
	}
	return resultFuc;
}

float AMathUtil::getLineFunc1(Point p1, Point p2,float y)
{
	return p1.x;
}
float AMathUtil::getLineFunc2(Point p1, Point p2, float x)
{
	return p1.y;
}
float AMathUtil::getLineFunc3(Point p1, Point p2, float x)
{
	float a = (p1.y - p2.y) / (p1.x - p2.x);
	float b = (p1.y - a * p1.x);
	return a*x + b;
}
float AMathUtil::getLineFunc4(Point p1, Point p2, float y)
{
	float a = (p1.y - p2.y) / (p1.x - p2.x);
	float b = (p1.y - a * p1.x);
	return (y-b)/a;
}
/**
* 得到两点间连线的斜率
* @param ponit1
* @param point2
* @return 两点间连线的斜率
*
*/
float AMathUtil::getSlope(Point p1, Point p2)
{
	return (p2.y - p1.y) / (p2.x - p1.x);
}