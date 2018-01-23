#include <omp.h>

#include <iostream>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#include <ctime>

#include "Matriz.h"


#include <vector>
#include <ctime>
#include <algorithm>
#include <iomanip>

#include "Point.h"
#include "ConvexHull.h"

#include <allegro.h>
#include <allegro_primitives.h>
#include <allegro_image.h>

using namespace std;

//const short int NUMERO_ELEMENTOS = 600;

// Esta constante se utiliza para la generación de números aleatorios al rellenar las matrices que se van a multiplicar
// Se intenta de este modo que no haya overflow en la multiplicación
const short int MAX_VALUE_IN_MATRIX {400};

// 1. Multiplicación de matrices utilizando distintos algoritmos
void ejercicio01()
{

    const int NUMBER_CYCLES {10};
    const int NUMBER_ALGORITHMS {5};

    // Declaracion de variables para calcular tiempos de ejecucion
	unsigned tInicio;
	unsigned tFin;
	double executionTime;

	int numberOfElements;

	// Variable para almacenar los tiempos de ejecución de los algoritmos
	// Al final se calcula una media del tiempo utilizado por cada algoritmo
    double timesTable [NUMBER_CYCLES][NUMBER_ALGORITHMS] {0};

    // Solicitar al usuario el numero de filas que tendra la matriz (Siempre sera una matriz cuadrada)
	cout << endl << endl << "Number of rows for the matrix: ";
	do {
        try {
                cin >> numberOfElements;
            } catch (exception ex) {
                cin.clear();
                cin.ignore(200, '\n');
                numberOfElements = -1;
            }
	} while (numberOfElements < 0);


	cout << endl << endl << "Starting..." << endl << endl;

    // Generar dos matrices cuadradas para multiplicarlas
	Matriz m1(numberOfElements);
	Matriz m2(numberOfElements);

	for(int i = 0; i < NUMBER_CYCLES; i++)
	{

        // Rellenar las matrices con valores aleatorios
        // Las mismas dos matrices se multiplican utilizando los distintos algoritmos
        // Esto permite comprobar la diferencia de tiempos de ejecución de manera más fácil
        m1.rellenarMatriz(MAX_VALUE_IN_MATRIX);
        //m1.print();
        //cout << endl << endl;
        m2.rellenarMatriz(MAX_VALUE_IN_MATRIX);
        //m2.print();
        //cout << endl << endl;

        // Multiplicar las matrices sin paralelizacion
        tInicio = clock();
        Matriz m3 = m1.multiplicar(m2);
        tFin = clock();

        executionTime = (double(tFin - tInicio) / CLOCKS_PER_SEC);
        timesTable[i][0] = executionTime;
        cout << m3.getNumeroElementos() << "x" << m3.getNumeroElementos() << " Elements Matrix Multiplication - Execution Time (normal execution): " << executionTime << endl;
        //m3.print();
        cout << endl;

        // Multiplicar las matrices con el algoritmo paralelizado
        tInicio = clock();
        Matriz m4 = m1.multiplicarParalelo(m2);
        tFin = clock();

        executionTime = (double(tFin - tInicio) / CLOCKS_PER_SEC);
        timesTable[i][1] = executionTime;
        cout << m4.getNumeroElementos() << "x" << m4.getNumeroElementos() << " Elements Matrix Multiplication - Execution Time (parallel execution): " << executionTime
        << " - Resultado obtenido igual: " << m3.compararMatriz(m4) << endl;
        //m4.print();
        cout << endl;

        // Multiplicar las matrices con el algoritmo paralelizado - Critical
        tInicio = clock();
        Matriz m5 = m1.multiplicarParaleloCritical(m2);
        tFin = clock();

        executionTime = (double(tFin - tInicio) / CLOCKS_PER_SEC);
        timesTable[i][2] = executionTime;
        cout << m5.getNumeroElementos() << "x" << m5.getNumeroElementos() << " Elements Matrix Multiplication - Execution Time (parallel critical execution): " << executionTime
        << " - Resultado obtenido igual: " << m3.compararMatriz(m5) << endl;
        //m5.print();
        cout << endl;

        // Multiplicar las matrices con el algoritmo paralelizado - Atomic
        tInicio = clock();
        Matriz m6 = m1.multiplicarParaleloAtomic(m2);
        tFin = clock();

        executionTime = (double(tFin - tInicio) / CLOCKS_PER_SEC);
        timesTable[i][3] = executionTime;
        cout << m6.getNumeroElementos() << "x" << m6.getNumeroElementos() << " Elements Matrix Multiplication - Execution Time (parallel Atomic execution): " << executionTime
        << " - Resultado obtenido igual: " << m3.compararMatriz(m6) << endl;
        //m6.print();
        cout << endl;

        // Multiplicar las matrices con el algoritmo paralelizado - Reduction
        tInicio = clock();
        Matriz m7 = m1.multiplicarParaleloReduction(m2);
        tFin = clock();

        executionTime = (double(tFin - tInicio) / CLOCKS_PER_SEC);
        timesTable[i][4] = executionTime;
        cout << m7.getNumeroElementos() << "x" << m7.getNumeroElementos() << " Elements Matrix Multiplication - Execution Time (parallel Reduction execution): " << executionTime
        << " - Resultado obtenido igual: " << m3.compararMatriz(m7) << endl;
        //m7.print();
        cout << endl << endl << endl << endl;
    }

    cin.clear();
    cin.ignore(200, '\n');

    cout << "Summary of Execution Time - Number of iterations: " << NUMBER_CYCLES << endl;
    cout << "Sequential - Parallel - Parallel_Critical - Parallel_Atomic - Parallel_Reduction" << endl;
    for (int i = 0; i < NUMBER_ALGORITHMS; i++)
    {
        double totalTime {0};
        for (int j = 0; j < NUMBER_CYCLES; j++)
        {
            totalTime += timesTable[j][i];
        }
        cout << totalTime / static_cast<double>(NUMBER_CYCLES) << "\t";
    }

    cout << endl << endl;

    cout << "Any key + Enter to return to menu" << endl;
    char s;
    cin >> s;


    cin.clear();
    cin.ignore(200, '\n');
}

// 2. Buscar el valor maximo en una matriz cuadrade de N elementos
void ejercicio02()
{

	int numberOfElements;

	// Solicitar al usuario el numero de filas que tendra la matriz (Siempre sera una matriz cuadrada)
	cout << endl << endl << "Number of rows for the matrix: ";
	do {
        try {
                cin >> numberOfElements;
            } catch (exception ex) {
                cin.clear();
                cin.ignore(200, '\n');
                numberOfElements = -1;
            }
	} while (numberOfElements < 0);


	cout << endl << endl << "Starting..." << endl << endl;

	// Generar una matriz aleatoria que se utilizará para buscar el valor máximo y compara
	// la eficiencia de un algoritmo secuencial y un algoritmo paralelo
	Matriz m8(numberOfElements);
	m8.rellenarMatriz();

	unsigned tInicio;
	unsigned tFin;
	double executionTime;

	//Ejecución del Algoritmo Secuencial para obtener el valor maximo de una matriz
    tInicio = clock();
	cout << m8.getNumeroElementos() << "x" << m8.getNumeroElementos() << " Elements Matrix - Maximum value search: " << m8.Maximo();
	tFin = clock();
	executionTime = (double(tFin - tInicio) / CLOCKS_PER_SEC);
	cout << " -> Execution Time: " << executionTime << endl;
	cout << endl;


	//Ejecución del Algoritmo Paralelizado para obtener el valor maximo de una matriz
    tInicio = clock();
	cout <<  m8.getNumeroElementos() << "x" << m8.getNumeroElementos() << " Elements Matrix - Maximum value search (Parallel execution): " << m8.MaximoParalelo();
	tFin = clock();
	executionTime = (double(tFin - tInicio) / CLOCKS_PER_SEC);
	cout << " -> Execution Time: " << executionTime << endl;

	cout << endl << endl;

	cout << endl << endl;
	cout << endl << endl;

	cin.clear();
    cin.ignore(200, '\n');

	cout << "Any key + Enter to return to menu" << endl;
    char s;
    cin >> s;


    cin.clear();
    cin.ignore(200, '\n');
}

// Esta función genera aleatoriamente puntos que se devuelven en un std::vector
// El número de puntos a generar viene especificado como parámetro de entrada
vector<Point> test2Dataset(int numeroPuntos) {

	vector<Point> pointsVector;

	//Inicializar generador de numeros aleatorios
	srand((unsigned)time(NULL));

	int valorX;
	int valorY;
	int const VALOR_MAXIMO_X {1000};
	int const VALOR_MAXIMO_Y {700};

	for (int i = 0; i < numeroPuntos; ++i) {
		valorX = (rand() % VALOR_MAXIMO_X) + 20;
		valorY = (rand() % VALOR_MAXIMO_Y) + 20;

		Point p(valorX, valorY);

		pointsVector.push_back(p);
	}

	return pointsVector;
}

void ejercicio03()
{
	vector<Point> convexHullVector_QH;
	vector<Point> convexHullVector_GW;
	vector<Point> pointsVector;


	/*ALLEGRO_DISPLAY *display = NULL;
	ALLEGRO_COLOR white = ALLEGRO_COLOR();
	ALLEGRO_COLOR yellow = ALLEGRO_COLOR();
	ALLEGRO_COLOR red = ALLEGRO_COLOR();
	ALLEGRO_COLOR green = ALLEGRO_COLOR();
	ALLEGRO_COLOR blue = ALLEGRO_COLOR();
	ALLEGRO_BITMAP *backBuffer = NULL;

	// Fijar los colores utilizados para pintar las envolventes convexas
	white.r = 255;
	white.g = 255;
	white.b = 255;

	yellow.r = 63;
	yellow.g = 63;
	yellow.b = 0;

	red.r = 50;
	red.g = 0;
	red.b = 0;

	green.r = 0;
	green.g = 50;
	green.b = 0;

	blue.r = 0;
	blue.g = 0;
	blue.b = 50;

	al_init_primitives_addon();

	if (!al_init()) {
		fprintf(stderr, "failed to initialize allegro!\n");
		return -1;
	}

	display = al_create_display(1040, 730);
	if (!display) {
		fprintf(stderr, "failed to create display!\n");
		return -1;
	}

	backBuffer = al_get_backbuffer(display);

	al_clear_to_color(al_map_rgb(0, 0, 0));*/

	time_t start;
	time_t end;

	cout << setprecision(7);

	const int numberAttempts {10};
	int numeroPuntos;
	double tablaTiempos[numberAttempts][2] = { 0 };

	cout << endl << endl << "Number of Point to generate: ";
	do {
        try {
                cin >> numeroPuntos;
            } catch (exception ex) {
                cin.clear();
                cin.ignore(200, '\n');
                numeroPuntos = -1;
            }
	} while (numeroPuntos < 0);

	cout << endl << endl << "Starting..." << endl << endl;

	int i;
	#pragma omp parallel for lastprivate (start, end, pointsVector, convexHullVector_QH, convexHullVector_GW) shared(tablaTiempos)
    for (i = 0; i < numberAttempts; ++i) {
		//pointsVector = test1Dataset();

		pointsVector = test2Dataset(numeroPuntos);

		start = clock();
		convexHullVector_QH = ConvexHull::quickHull(pointsVector);
		end = clock();

		#pragma omp atomic
		tablaTiempos[i][0] += (float(end - start) / CLOCKS_PER_SEC);

		start = clock();
		convexHullVector_GW = ConvexHull::giftWrapping(pointsVector);
		end = clock();

		#pragma omp atomic
		tablaTiempos[i][1] += (float(end - start) / CLOCKS_PER_SEC);
    }


	double tiemposTotales[2] = { 0 };

	cout << "Summary of Execution Time - Number of iterations: " << numberAttempts << endl;
	cout << "QuickHull - GiftWrapping" << endl;
	for (int i = 0; i < numberAttempts; ++i) {
		for (int j = 0; j < 2; j++) {
			tiemposTotales[j] += tablaTiempos[i][j];
		}
	}

	for (int i = 0; i < 2; ++i) {
		cout << tiemposTotales[i] / static_cast<double>(numberAttempts) << "\t";
	}
	cout << endl << endl;

	/*cout << "Dibujando ultimo caso ejecutado. El numero de puntos a dibujar es: " << numeroPuntos << endl << endl;
	//Dibujar todos los puntos que se están tratando
	for (int i = 0; i < pointsVector.size(); i++)
		al_put_pixel(pointsVector.at(i).getX(), pointsVector.at(i).getY(), yellow);

	//Dibujar el convex hull obtenido con el algoritmo QuickHull
	for (auto it = convexHullVector_QH.begin(); it < convexHullVector_QH.end() - 1; ++it) {
		al_draw_line((*it).getX(), (*it).getY(), (*(it + 1)).getX(), (*(it + 1)).getY(), red, 2.0);
	}

	//Dibujar la línea que cierra el convex hull
	al_draw_line((*(convexHullVector_QH.end() - 1)).getX(), (*(convexHullVector_QH.end() - 1)).getY(),
		(*convexHullVector_QH.begin()).getX(), (*convexHullVector_QH.begin()).getY(), red, 2.0);


	//Dibujar el convex hull con el algoritmo GiftWrapping
	for (auto it = convexHullVector_GW.begin(); it < convexHullVector_GW.end() - 1; ++it) {
		al_draw_line((*it).getX(), (*it).getY(), (*(it + 1)).getX(), (*(it + 1)).getY(), green, 2.0);
	}

	//Dibujar la línea que cierra el convex hull
	al_draw_line((*(convexHullVector_GW.end() - 1)).getX(), (*(convexHullVector_GW.end() - 1)).getY(),
		(*convexHullVector_GW.begin()).getX(), (*convexHullVector_GW.begin()).getY(), green, 2.0);




	al_flip_display();*/


    cin.clear();
    cin.ignore(200, '\n');

	cout << "Any key + Enter to return to menu" << endl;
	char s;
	cin >> s;


    cin.clear();
    cin.ignore(200, '\n');
	/*al_destroy_display(display);*/

}

int menu()
{
    int option;

    for (int i = 0; i < 30; ++i)
        cout << endl;


    cin.exceptions(ios_base::failbit);//Why this line "exceptions" different from the next "exception"

    cout << "\t\t\t\t\t\t Menu" << endl<< endl;
    cout << "\t\t\t1. Ejercicio 1 - Matrix Multiplication" << endl;
    cout << "\t\t\t2. Ejercicio 2 - Maximum Element in Matrix" << endl;
    cout << "\t\t\t3. Ejercicio 3 - Parallel calculation of Convex Hull" << endl;
    cout << "\t\t\t0. Exit" << endl;
    cout << endl;
    cout << "\t\t\tOption: ";

    do {
            try {
                cin >> option;
            } catch (exception ex) {
                cin.clear();
                cin.ignore();
                option = -1;
            }
    } while (option != 1 && option != 2 && option != 3 && option != 0);

    cin.clear();
    cin.ignore(200, '\n');

    return option;
}

int main()
{
    int menuOption {-1};

	// Inicializacion de la semilla para numeros aleatorios
	srand(time(NULL));

    while (menuOption != 0)
    {
        menuOption = menu();
        //cin.clear();
        //cin.ignore();
        switch (menuOption)
        {
            case 1:
                ejercicio01();
                break;
            case 2:
                ejercicio02();
                break;
            case 3:
                ejercicio03();
                break;
            case 0:
                break;
        }
    }
	return 0;
}
