#pragma once

#include <iostream>

using namespace std;

class Point
{
public:
	Point();
	Point(int x, int y) : x(x), y(y) {}

	Point(const Point& p): x(p.x), y(p.y) {}

	~Point();

	// Getters y Setters

	void setX(int x);
	void setY(int y);

	inline int getX() const { return x; };
	inline int getY() const { return y; };

	// Sobrecarga de los operadores
	void operator=(const Point & p);
	bool operator<(const Point &p) const;
	bool operator>(const Point &p) const;

	inline bool operator!=(const Point & p) const {
		return ((x != p.x) || (y != p.y));
	}

	inline bool operator==(const Point & p) const {
		return ((x == p.x) && (y == p.y));
	}

	friend ostream & operator<<(ostream & stream, const Point & p) {
		return stream << "(" << p.x << ", " << p.y << ")";
	}

	// Devuelve el cuadrado de la distancia hasta el punto p
	double distance2(const Point &p);


private:
	int x;
	int y;
};
