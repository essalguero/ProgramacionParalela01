#include "Matriz.h"

#include <omp.h>

#include <float.h>
#include <iostream>
#include <random>
#include <functional>

Matriz::Matriz() {
	numberRows = 0;
	m = nullptr;
}

Matriz::Matriz(int n)
{
	numberRows = {n};

	m = new double* [n];
	for (int i = 0; i < n; ++i) {
		m[i] = new double[n] {0};
	}

	/*for (int i = 0; i < n; i++)
		for (int j = 0; j < n; j++)
			m[i][j] = 0;*/

}

Matriz::Matriz(const Matriz & matriz) {

	numberRows = matriz.numberRows;

	m = new double*[numberRows];
	for (int i = 0; i < numberRows; ++i) {
		m[i] = new double[numberRows];
	}

	for (int i = 0; i < numberRows; ++i)
		for (int j = 0; j < numberRows; ++j)
			m[i][j] = matriz.m[i][j];
}


Matriz::~Matriz()
{
	for (int i = 0; i < numberRows; ++i)
		delete m[i];

	delete[] m;
}


void Matriz::rellenarMatriz(int maxValue) {

    float intermediateValue;

    if (maxValue > 0) {
        for (int i = 0; i < numberRows; ++i)
        	for (int j = 0; j < numberRows; ++j)
        		m[i][j] = rand() % maxValue;

        //m[i][j] = std::bind(std::uniform_real_distribution<>(DBL_MIN, DBL_MAX),std::default_random_engine());
    //std::bind(std::uniform_real_distribution<>(DBL_MIN, DBL_MAX));
    } else {
        for (int i = 0; i < numberRows; ++i)
            for (int j = 0; j < numberRows; ++j) {
                intermediateValue = (static_cast<float>(rand()) - 0.00001f) / (static_cast<float>(rand()) + 0.00001f);
                m[i][j] = round(static_cast<double>(intermediateValue));
            }
    }
}

// Multiplicar por la matriz pasada como parámetro de forma secuencial
Matriz Matriz::multiplicar(const Matriz & matriz) {
	Matriz result(numberRows);

	for (int i = 0; i < numberRows; ++i) {
		for (int j = 0; j < numberRows; ++j) {
			for (int k = 0; k < numberRows; ++k) {
				result.m[i][j] += m[i][k] * matriz.m[k][j];
			}
		}
	}

	return result;
}

// Multiplicar por la matriz pasada como parámetro de forma paralela
Matriz Matriz::multiplicarParalelo(const Matriz & matriz) {
	Matriz result(numberRows);

	int i, j, k;
	#pragma omp parallel for collapse(3)
	for (i = 0; i < numberRows; ++i) {
		for (j = 0; j < numberRows; ++j) {
			for (k = 0; k < numberRows; ++k) {
				result.m[i][j] += m[i][k] * matriz.m[k][j];
			}
		}
	}

	return result;
}

// Multiplicar por la matriz pasada como parámetro de forma paralela
// Se utiliza critical para proteger la actualización de los datos compartidos
Matriz Matriz::multiplicarParaleloCritical(const Matriz & matriz) {
	Matriz result(numberRows);

	//double dResult;

	int i, j, k;

	#pragma omp parallel for collapse(3)
	for (i = 0; i < numberRows; ++i) {
		for (j = 0; j < numberRows; ++j) {
			for (k = 0; k < numberRows; ++k) {
				#pragma omp critical
				result.m[i][j] += m[i][k] * matriz.m[k][j];
			}
		}
	}


	return result;
}

// Multiplicar por la matriz pasada como parámetro de forma paralela
// Se utiliza atomic para proteger la actualización de los datos compartidos
Matriz Matriz::multiplicarParaleloAtomic(const Matriz & matriz) {
	Matriz result(numberRows);

	int i, j, k;

	#pragma omp parallel for collapse(3)
	for (i = 0; i < numberRows; ++i) {
		for (j = 0; j < numberRows; ++j) {
			for (k = 0; k < numberRows; ++k) {
				#pragma omp atomic

				result.m[i][j] += m[i][k] * matriz.m[k][j];
			}
		}
	}

	return result;
}

// Multiplicar por la matriz pasada como parámetro de forma paralela
// Se utiliza Reduction para proteger la actualización de los datos compartidos
Matriz Matriz::multiplicarParaleloReduction(const Matriz & matriz) {
	Matriz result(numberRows);

	double dResult;

	int i, j, k;

	#pragma omp parallel for
	for (i = 0; i < numberRows; ++i) {
        #pragma omp parallel for private(dResult)
		for (j = 0; j < numberRows; ++j) {
            dResult = 0;
            #pragma omp parallel for reduction(+:dResult)
			for (k = 0; k < numberRows; ++k) {
				dResult += m[i][k] * matriz.m[k][j];
			}
			result.m[i][j] = dResult;
		}
	}

	return result;
}

// Imprime en pantalla los valores de la matriz
void Matriz::print() {
	for (int i = 0; i < numberRows; ++i) {
		for (int j = 0; j < numberRows; ++j)
			cout << m[i][j] << " ";

		cout << endl;
	}
}

// Devuelve el máximo valor contenido en la matriz
double Matriz::Maximo() {
	double maximo {DBL_MIN};

	for (int i = 0; i < numberRows; ++i) {
		for (int j = 0; j < numberRows; ++j) {
			if (m[i][j] > maximo) {
				maximo = m[i][j];
			}
		}
	}

	return maximo;
}

// Devuelve el máximo valor contenido en la matriz utilizando para buscarlo un algoritmo paralelo
double Matriz::MaximoParalelo() {
	double maximo {DBL_MIN};
	double maximoLocal;

    #pragma omp parallel shared(maximo) private(maximoLocal)
    {
        maximoLocal = DBL_MIN;
        int tid;
		tid = omp_get_thread_num();

        #pragma omp for collapse(2) nowait
        for (int i = 0; i < numberRows; ++i) {
            for (int j = 0; j < numberRows; ++j) {
                if (m[i][j] > maximoLocal) {
                    maximoLocal = m[i][j];
                }
            }
        }

        #pragma omp critical
        {
            cout << endl << "Valor comprobado por el thread " << tid << endl;
            cout << "Maximo actual: " << maximo << "\tMaximo local: " << maximoLocal << endl;
            if (maximoLocal > maximo) {
                cout << "Modificado thread: " << tid << endl;
                maximo = maximoLocal;
            } else {
                cout << "Valor NO Modificado por el thread: " << tid << endl;
            }
        }
    }
    cout << endl << endl;

	return maximo;
}

// Compara la matriz con la matriz pasada como parámetro
bool Matriz::compararMatriz(const Matriz & matriz) {
    if (matriz.numberRows != numberRows)
        return false;

    for(int i = 0; i < numberRows; ++i)
        for(int j = 0; j < numberRows; ++j)
            if (matriz.m[i][j] != m[i][j])
                return false;

    return true;
}
