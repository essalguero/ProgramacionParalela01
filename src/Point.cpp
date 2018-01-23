#include "Point.h"


Point::Point()
{
	x = 0;
	y = 0;
}


Point::~Point()
{
}


void Point::setX(int x)
{
	this->x = x;
}

void Point::setY(int y)
{
	this->y = y;
}

void Point::operator=(const Point &p) {
	x = p.x;
	y = p.y;
}

bool Point::operator<(const Point &p) const {
	if (x < p.x)
		return true;
	else if ((x == p.x) && (y < p.y))
		return true;

	return false;
}

bool Point::operator>(const Point &p) const {
	if (x > p.x)
		return true;
	else if ((x == p.x) && (y > p.y))
		return true;

	return false;
}

double Point::distance2(const Point &p) {
	double distX = x - p.x;
	double distY = y - p.y;

	return ((distX * distX) + (distY * distY));
}
