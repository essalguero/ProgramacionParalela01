#pragma once

#include <vector>

#include "Point.h"

class ConvexHull
{
public:

	// Esta clase implementa distintos algoritmos para calcular la envolvente convexa
	// de una nube de puntos
	static vector<Point> quickHull(const vector<Point> & pointsVector);

	static vector<Point> giftWrapping(const vector<Point> &pointsVector);

private:

	// Es el método recursivo utilizado para calcular la envolvente convexa utilizando
	// el algoritmo QuickHull
	static vector<Point> quickHull(Point a, Point b, const vector<Point> & pointsVector);

	// Devuelve el coseno del ángulo que forman los puntos a, b y c
	static double angleCos(Point a, Point b, Point c);

	// Devuelve el cuadrado del área del triángulo que forman los puntos a, b y c
	static inline int Area2(const Point & a, const Point & b, const Point & c)
	{
		return ((b.getX() - a.getX()) * (c.getY() - a.getY())) -
			((c.getX() - a.getX()) * (b.getY() - a.getY()));
	}

	//Devuelve true si el punto c esta a la izquierda de la recta que pasa por los puntos a y b
	inline static bool left(Point a, Point b, Point c) {
		bool value = Area2(a, b, c) > 0;
		return value;
	}

	//Devuelve true si el punto c esta a la izquierda de la recta que pasa por los puntos a y b o
	// si los tres puntos son coliniales
	inline static bool leftOn(Point a, Point b, Point c) {
		bool value = Area2(a, b, c) >= 0;
		return value;
	}

	// Devuelve true si los puntos a, b y c son coliniales
	static inline bool colinear(const Point & a, const Point &b, const Point &c) {
		return Area2(a, b, c) == 0;
	}

	// Devuelve true si p esta incluido dentro de la envolvente convexa o está en el borde
	static bool incluido(const Point & p, const vector<Point> & convexHull);
};
