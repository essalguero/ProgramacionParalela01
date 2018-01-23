#pragma once

#include <stdlib.h>

using namespace std;

class Matriz
{
private:
	double ** m;
	int numberRows;

public:

	Matriz();

	Matriz(int);
	~Matriz();
	Matriz(const Matriz &);

	void rellenarMatriz(int = 0);
	Matriz multiplicar(const Matriz &);
	Matriz multiplicarParalelo(const Matriz &);
	Matriz multiplicarParaleloCritical(const Matriz &);
	Matriz multiplicarParaleloAtomic(const Matriz &);
	Matriz multiplicarParaleloReduction(const Matriz &);
	void print();

	inline int getNumeroElementos() { return numberRows; };

	double Maximo();
	double MaximoParalelo();

	bool compararMatriz(const Matriz &);
};

