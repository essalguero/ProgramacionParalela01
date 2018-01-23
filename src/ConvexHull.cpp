#include "ConvexHull.h"

#include <stdio.h>

#include <vector>

#include <algorithm>

#include "Point.h"

#include <math.h>
#include <limits.h>

using namespace std;

// Devuelve true si el punto p es interno o esta en el borde de convexHull
bool ConvexHull::incluido(const Point & p, const vector<Point> & convexHull) {
	for (auto it = convexHull.begin() + 1; it < convexHull.end(); ++it) {
		if (!leftOn(*(it - 1), *it, p)) {
			return false;
		}
	}

	return leftOn(*(convexHull.end() - 1), *(convexHull.begin()), p);
}


// La funcion devuelve el coseno del angulo que forman los puntos a, b y c
double ConvexHull::angleCos(Point a, Point b, Point c) {

	double v1X = c.getX() - b.getX();
	double v1Y = c.getY() - b.getY();

	double v2X = a.getX() - b.getX();
	double v2Y = a.getY() - b.getY();

	double v1Mod = sqrt((v1X * v1X) + (v1Y * v1Y));
	double v2Mod = sqrt((v2X * v2X) + (v2Y * v2Y));

	double v1Dotv2 = (v1X * v2X) + (v1Y * v2Y);

	double value = v1Dotv2 / (v1Mod * v2Mod);

	return value;

}


// Calcula la envolvente convexa con el métod quickHull.
// Esta función, es la componente recursiva utilizada para calcular la envolvente convexa
vector<Point> ConvexHull::quickHull(Point a, Point b, const vector<Point> & pointsVector)
{
	// Devuelve el convexHull calculado en esta llamada recursiva
	vector<Point> convexHull;

	// Se utiliza para dividir los puntos y llamar a las siguientes llamadas recursivas
	vector<Point> izquierda;
	vector<Point> derecha;

	// Guardan los valores devueltos (convexHull parciales) por las siguientes llamadas recursivas
	vector<Point> quickHullIzquierda;
	vector<Point> quickHullDerecha;


	int valorArea;

	// Variables para encontrar el punto usado para dividir la nube de puntos
	int valorMayorArea {0};
	Point puntoMayorArea;

	// No hay puntos en pointsVector. Se devuelve un convexHull vacio
	if (pointsVector.size() == 0) {
		return convexHull;
	}

	// Solo hay un punto en pointsVector. Se devuelve ese punto como convexHull
	if (pointsVector.size() == 1) {
		convexHull.push_back(pointsVector.at(0));
		return convexHull;
	}

	// Hay que buscar el punto que tenga mas area junto con a y b
	/*for (auto it = pointsVector.begin(); it != pointsVector.end(); it++)
	{
		//El punto tiene que ser distinto a A y B
		if ((*it != a) && (*it != b)) {

			valorArea = Area2(a, b, *it);

			// Si el area es negativo, estaria al lado de la linea AB que no necesitamos comprobar
			if (valorArea > 0) {
                if (valorArea > valorMayorArea) {
                    puntoMayorArea = *it;
                    valorMayorArea = valorArea;
                }
			}
		}
	}*/

	size_t i;
	#pragma omp parallel for private (valorArea) shared (valorMayorArea)
	for (i = 0; i < pointsVector.size(); ++i)
	{
		//El punto tiene que ser distinto a A y B
		if ((pointsVector[i] != a) && (pointsVector[i] != b)) {

			valorArea = Area2(a, b, pointsVector[i]);

			// Si el area es negativo, estaria al lado de la linea AB que no necesitamos comprobar

			if (valorArea > 0) {
                #pragma omp critical
                {
                    if (valorArea > valorMayorArea) {
                        puntoMayorArea = pointsVector[i];
                        valorMayorArea = valorArea;
                    }
                }
			}
		}
	}

	// Buscar los puntos que esten a la izquierda de A-PuntoMayorArea
	/*for (auto it = pointsVector.begin(); it != pointsVector.end(); it++) {
		if (left(a, puntoMayorArea, *it)) {
			izquierda.push_back(*it);
		}
	}*/
	// i already defined
	#pragma omp parallel for
	for (i = 0; i < pointsVector.size(); ++i) {
		if (left(a, puntoMayorArea, pointsVector[i])) {
			izquierda.push_back(pointsVector[i]);
		}
	}

	// Buscar los puntos que esten a la derecha de B-PuntoMayorArea (A la izquierda de PuntoMayorArea-B)
	/*for (auto it = pointsVector.begin(); it != pointsVector.end(); it++) {
		if (left(puntoMayorArea, b, *it)) {
			derecha.push_back(*it);
		}
	}*/
	// i already defined
	#pragma omp parallel for
	for (i = 0; i < pointsVector.size(); ++i) {
		if (left(puntoMayorArea, b, pointsVector[i])) {
			derecha.push_back(pointsVector[i]);
		}
	}

	//Procesar los puntos restantes
	quickHullIzquierda = quickHull(a, puntoMayorArea, izquierda);
	quickHullDerecha = quickHull(puntoMayorArea, b, derecha);

	// Mezclar los puntos con los de la siguiente iteracion. Con el orden en que se ha hecho, la envolvente viene dada en sentido antihorario
	for (auto it = quickHullDerecha.begin(); it != quickHullDerecha.end(); ++it) {
		convexHull.push_back(*it);
	}

	convexHull.push_back(puntoMayorArea);

	for (auto it = quickHullIzquierda.begin(); it != quickHullIzquierda.end(); ++it) {
		convexHull.push_back(*it);
	}

	return convexHull;
}


// Calcula la envolvente convexa con el método quickHull.
// Esta función, se utiliza para la inicialización de variables y para realizar la ultima
// mezcla de las componentes calculadas mediante el metodo recursivo.
// Esta es la afunción que tiene que ser llamada, por ese motivo es pública
vector<Point> ConvexHull::quickHull(const vector<Point> & pointsVector)
{
	// Se utilizan para dividir la nube de puntos en dos mitades
	Point minX;
	Point maxX;

	int valorArea {0};

	// Lista a devolver con el convexHull de todos los puntos
	vector<Point> convexHull;

	//Estas listas contienen los convexHull parciales obtenidos de las llamadas recursivas
	vector<Point> quickHullArriba;
	vector<Point> quickHullAbajo;

	// Listas utilizadas para las siguientes llamadas
	vector<Point> arriba;
	vector<Point> abajo;

	// Comprobar si el vector viene vacio
	if (pointsVector.size() > 0)
	{
		//Buscar el maximo y minimo en coordenada X
		minX = pointsVector.at(0);
		maxX = pointsVector.at(0);
		for (vector<Point>::const_iterator it = pointsVector.begin() + 1; it != pointsVector.end(); ++it)
		{
			// Comparar los puntos y obtener el de menor x y menor y
			if (minX.getX() > it->getX())
			{
				minX = *it;
			}
			else if (minX.getX() == it->getX()) {
				// En caso de igual X, se queda con el de menor coordenada Y
				if (minX.getY() > it->getY())
					minX = *it;
			}

			// Comparar los puntos y obtener el de mayor x y mayor y
			if (maxX.getX() < it->getX())
			{
				maxX.setX(it->getX());
				maxX.setY(it->getY());
			}
			else if (maxX.getX() == it->getX()) {
				// En caso de igual X, se queda con el de mayor coordenada Y
				if (maxX.getY() < it->getY())
					maxX = *it;
			}


		}

		// Buscar el punto que tiene mayor area -> debe coincidir con el que este mas lejos de la linea
		/*for (vector<Point>::const_iterator it = pointsVector.begin(); it != pointsVector.end(); it++)
		{
			// Recorrer todos los puntos de la lista. No se tienen en cuenta puntos repetidos
			if ((*it != minX) && (*it != maxX)) {

				// Calcular el area del triangulo minX maxX it -> solo importa el signo por eso se usa el doble del area
				valorArea = Area2(minX, maxX, *it);

				//Depende del valor del area (signo), se encuentra a un lado u otro de la linea que une los puntos minX y maxX
				if (valorArea > 0) {
					arriba.push_back(*it);
				}
				else {
					abajo.push_back(*it);
				}


			}
		}*/

		// Buscar el punto que tiene mayor area -> debe coincidir con el que este mas lejos de la linea
		size_t i;
        #pragma omp parallel for private (valorArea)
		for(i = 0; i < pointsVector.size(); ++i)
        {
            // Recorrer todos los puntos de la lista. No se tienen en cuenta puntos repetidos
            if ((pointsVector[i] != minX) && (pointsVector[i] != maxX)) {
                // Calcular el area del triangulo minX maxX it -> solo importa el signo por eso se usa el doble del area
                valorArea = Area2(minX, maxX, pointsVector[i]);
                //Depende del valor del area (signo), se encuentra a un lado u otro de la linea que une los puntos minX y maxX
				if (valorArea > 0) {
					arriba.push_back(pointsVector[i]);
				}
				else {
					abajo.push_back(pointsVector[i]);
				}
            }
        }

		// Calcular el convex hull de los puntos de arriba y abajo
		quickHullArriba = quickHull(minX, maxX, arriba);
		quickHullAbajo = quickHull(maxX, minX, abajo);

		//Mezclar las dos envolventes calculadas. Con el orden en que se ha hecho, la envolvente
		//viene dada en sentido antihorario
		convexHull.push_back(minX);

		for (auto it : quickHullAbajo) {
			convexHull.push_back(it);
		}

		convexHull.push_back(maxX);

		for (auto it : quickHullArriba) {
			convexHull.push_back(it);
		}


	}
	return convexHull;
}


// Calcula la envolvente convexa de una nube de puntos utilizando el algoritmo GiftWrapping
vector<Point> ConvexHull::giftWrapping(const vector<Point> & pointsVector) {

	vector<Point> convexHullVector;

	Point minX;

	if (pointsVector.size() == 0)
		return convexHullVector;

	if (pointsVector.size() == 1) {
		convexHullVector.push_back(pointsVector.at(0));
		return convexHullVector;
	}


	// Buscar el punto mas a la izquierda (menor x)
	// En caso de igualdad, coger la de menor Y. Para evitar que el segundo punto pueda ser colineal.
	minX = pointsVector.at(0);
	for (auto it = pointsVector.begin() + 1; it < pointsVector.end(); ++it) {
		if ((*it) < minX) {
			minX = *it;
		}
	}

	// Buscar los puntos que formen un mayor angulo con los dos previos
	// Para empezar, se inicializa uno de ellos con x = minX e y = INT_MIN
	Point previous(minX.getX(), minX.getY() + 1);
	Point current = minX;
	convexHullVector.push_back(minX);
	Point next;
	bool finished = false;

	//Repetir el bucle hasta obtener un convex hull
	while (!finished) {

		// En cada iteracion se busca el menor angulo posible (en realidad se busca el menor coseno)
		double angleMin { INT_MAX };

		// Se compara con todos los puntos de la lista
		/*for (int i = 0; i < static_cast<int>(pointsVector.size()); i++) {
			Point p = pointsVector.at(i);
			// Solo se procesan aquellos que forman un angulo menor de 180 grados
			if (left(previous, current, p)) {

				//Calcular el angulo que forman p y las dos puntos anteriores
				double angle = angleCos(p, current, previous);

				//Utilizar las dos condiciones para evitar problemas con el redondeo
				//Con el Area2 se ve si los puntos son colineares
				if ((angle < angleMin) && (Area2(current, p, next) != 0)) {
					angleMin = angle;
					next = p;

				} //Se utiliza Area2 en lugar de ((angle == angleMin) porque durante las pruebas se
				  //han detectado problemas causados por redondeos
				else if ((Area2(current, p, next) == 0) &&
					(current.distance2(p) > current.distance2(next))) {
					angleMin = angle;
					next = p;
				}

			}
		} //for(int i = 0; i < pointsVector.size(); i++)
		*/

		int i;
		#pragma omp parallel for shared (angleMin)
		for (i = 0; i < static_cast<int>(pointsVector.size()); ++i) {
			Point p = pointsVector.at(i);
			// Solo se procesan aquellos que forman un angulo menor de 180 grados
			if (left(previous, current, p)) {

				//Calcular el angulo que forman p y las dos puntos anteriores
				double angle = angleCos(p, current, previous);

				//Utilizar las dos condiciones para evitar problemas con el redondeo
				//Con el Area2 se ve si los puntos son colineares
				if (Area2(current, p, next) != 0) {
                        #pragma omp critical
                        {
                            if (angle < angleMin) {
                                angleMin = angle;
                                next = p;
                            }
                        }

				} //Se utiliza Area2 en lugar de ((angle == angleMin) porque durante las pruebas se
				  //han detectado problemas causados por redondeos
				else if ((Area2(current, p, next) == 0) &&
					(current.distance2(p) > current.distance2(next))) {
                    #pragma omp critical
                    {
                        angleMin = angle;
                    }
					next = p;
				}

			}
		} //for(i = 0; i < pointsVector.size(); i++)

		  // Comprobar si el punto obtenido es igual al inicial. Si es igual, el proceso ha terminado
		if (next != minX) {
			convexHullVector.push_back(next);
			previous = current;
			current = next;
		}
		else {
			finished = true;
		}
	}

	// Envolvente convexa obtenida
	return convexHullVector;
}


