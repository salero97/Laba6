#include <iostream>  
#include <vector>    
#include <cmath>     
#include <iomanip>   
#include <algorithm> 

using namespace std; 

// функция для вывода матрицы на экран
void printMatrix(const vector<vector<double>>& matrix) {
    // перебираем каждую строку матрицы
    for (const auto& row : matrix) {
        // перебираем каждый элемент строки
        for (double value : row) {
            // выводим элемент с шириной 10 символов
            cout << setw(10) << value << " ";
        }
        // переходим на новую строку после вывода всех элементов строки
        cout << endl;
    }
}

// функция для вывода вектора на экран
void printVector(const vector<double>& vec) {
    // перебираем все элементы вектора
    for (double value : vec) {
        // выводим элемент с шириной 10 символов
        cout << setw(10) << value << " ";
    }
    // переходим на новую строку после вывода всех элементов
    cout << endl;
}

// функция проверки диагонального преобладания матрицы
bool checkDiagonalDominance(const vector<vector<double>>& matrix) {
    // получаем размер матрицы (количество строк/столбцов)
    int size = matrix.size();
    // перебираем все строки матрицы
    for (int i = 0; i < size; ++i) {
        // переменная для суммы модулей недиагональных элементов
        double sum = 0;
        // перебираем все элементы строки
        for (int j = 0; j < size; ++j) {
            // если элемент не диагональный
            if (j != i) {
                // добавляем его модуль к сумме
                sum += fabs(matrix[i][j]);
            }
        }
        // если диагональный элемент меньше или равен сумме остальных
        if (fabs(matrix[i][i]) <= sum) {
            // выводим сообщение о проблеме
            cout << "нет диагонального преобладания в строке " << i+1 << endl;
            // возвращаем false (нет преобладания)
            return false;
        }
    }
    // если все строки прошли проверку, возвращаем true
    return true;
}

// функция для попытки сделать матрицу диагонально преобладающей
bool tryMakeDiagonallyDominant(vector<vector<double>>& matrix, vector<double>& vectorB) {
    // получаем размер матрицы
    int size = matrix.size();
    // перебираем все строки
    for (int i = 0; i < size; ++i) {
        // предполагаем, что текущая строка имеет максимальный диагональный элемент
        int maxRow = i;
        // запоминаем модуль диагонального элемента
        double maxValue = fabs(matrix[i][i]);
        // ищем строку с максимальным диагональным элементом в текущем столбце
        for (int k = i + 1; k < size; ++k) {
            // если нашли больший элемент
            if (fabs(matrix[k][i]) > maxValue) {
                // обновляем максимальное значение
                maxValue = fabs(matrix[k][i]);
                // запоминаем номер строки с максимальным элементом
                maxRow = k;
            }
        }
        // если нашли строку с большим диагональным элементом
        if (maxRow != i) {
            // меняем строки местами в матрице
            swap(matrix[i], matrix[maxRow]);
            // меняем соответствующие элементы в векторе правой части
            swap(vectorB[i], vectorB[maxRow]);
            // выводим сообщение о перестановке
            cout << "перестановка строк " << i+1 << " и " << maxRow+1 << " для диагонального преобладания\n";
        }
        // проверяем, стало ли выполнено условие диагонального преобладания
        double sum = 0;
        // вычисляем сумму модулей недиагональных элементов
        for (int j = 0; j < size; ++j) {
            if (j != i) sum += fabs(matrix[i][j]);
        }
        // если диагональный элемент меньше суммы остальных
        if (fabs(matrix[i][i]) <= sum) {
            // возвращаем false (не удалось сделать преобладающей)
            return false;
        }
    }
    // если все получилось, возвращаем true
    return true;
}

// функция для вычисления невязки
vector<double> calculateResidual(const vector<vector<double>>& matrix, 
                               const vector<double>& vectorB, 
                               const vector<double>& solution) {
    // получаем размер системы
    int size = matrix.size();
    // создаем вектор для хранения невязки
    vector<double> residual(size);
    // перебираем все уравнения системы
    for (int i = 0; i < size; ++i) {
        // начинаем с -b[i]
        residual[i] = -vectorB[i];
        // добавляем сумму A[i][j]*x[j]
        for (int j = 0; j < size; ++j) {
            residual[i] += matrix[i][j] * solution[j];
        }
    }
    // возвращаем вектор невязки
    return residual;
}

// метод гаусса с выбором главного элемента
vector<double> solveGauss(vector<vector<double>> matrix, vector<double> vectorB) {
    // получаем размер системы
    int size = matrix.size();
    // создаем вектор для хранения решения
    vector<double> solution(size, 0);
    
    // выводим заголовок для прямого хода
    cout << "\nпрямой ход метода гаусса:\n";
    
    // прямой ход метода (приведение к треугольному виду)
    for (int k = 0; k < size; ++k) {
        // выбор главного элемента в текущем столбце
        int maxRow = k;
        // запоминаем модуль текущего диагонального элемента
        double maxValue = fabs(matrix[k][k]);
        // ищем максимальный элемент в столбце ниже текущего
        for (int i = k + 1; i < size; ++i) {
            // если нашли больший элемент
            if (fabs(matrix[i][k]) > maxValue) {
                // обновляем максимальное значение
                maxValue = fabs(matrix[i][k]);
                // запоминаем строку с максимальным элементом
                maxRow = i;
            }
        }
        
        // если нашли строку с большим элементом
        if (maxRow != k) {
            // меняем строки местами в матрице
            swap(matrix[k], matrix[maxRow]);
            // меняем соответствующие элементы в векторе правой части
            swap(vectorB[k], vectorB[maxRow]);
            // выводим сообщение о перестановке
            cout << "перестановка строк " << k+1 << " и " << maxRow+1 << endl;
        }
        
        // проверка на вырожденность матрицы
        if (fabs(matrix[k][k]) < 1e-10) {
            // выводим сообщение об ошибке
            cerr << "матрица вырожденная!" << endl;
            // завершаем программу с кодом ошибки
            exit(1);
        }
        
        // нормируем текущую строку
        double divisor = matrix[k][k];
        // делим все элементы строки на диагональный элемент
        for (int j = k; j < size; ++j) {
            matrix[k][j] /= divisor;
        }
        // делим соответствующий элемент вектора правой части
        vectorB[k] /= divisor;
        
        // выводим матрицу после нормировки
        cout << "после нормировки строки " << k+1 << ":\n";
        printMatrix(matrix);
        cout << "вектор b: ";
        printVector(vectorB);
        
        // исключаем текущую переменную из всех нижних строк
        for (int i = k + 1; i < size; ++i) {
            // вычисляем коэффициент для вычитания
            double factor = matrix[i][k];
            // вычитаем из текущей строки нормированную строку, умноженную на коэффициент
            for (int j = k; j < size; ++j) {
                matrix[i][j] -= factor * matrix[k][j];
            }
            // аналогично для вектора правой части
            vectorB[i] -= factor * vectorB[k];
        }
        
        // выводим матрицу после исключения
        cout << "после исключения в столбце " << k+1 << ":\n";
        printMatrix(matrix);
        cout << "вектор b: ";
        printVector(vectorB);
    }
    
    // выводим заголовок для обратного хода
    cout << "\nобратный ход метода гаусса:\n";
    
    // обратный ход (нахождение решения)
    for (int k = size - 1; k >= 0; --k) {
        // начинаем с элемента вектора правой части
        solution[k] = vectorB[k];
        // вычитаем уже найденные переменные
        for (int j = k + 1; j < size; ++j) {
            solution[k] -= matrix[k][j] * solution[j];
        }
        // выводим найденное значение переменной
        cout << "x[" << k+1 << "] = " << solution[k] << endl;
    }
    
    // возвращаем полученное решение
    return solution;
}

// метод якоби для решения системы
vector<double> solveJacobi(const vector<vector<double>>& matrix, 
                         const vector<double>& vectorB, 
                         double epsilon, 
                         int maxIterations = 100) {
    // получаем размер системы
    int size = matrix.size();
    // создаем вектор для решения (начальное приближение - нули)
    vector<double> solution(size, 0);
    // вектор для новых значений на следующей итерации
    vector<double> newSolution(size);
    // счетчик итераций
    int iterations = 0;
    // переменная для хранения погрешности
    double error;
    
    // выводим заголовок для итерационного процесса
    cout << "\nметод якоби (начальное приближение - нулевое):\n";
    cout << "n\tx1\t\tx2\t\tx3\t\tx4\t\tεn\n";
    
    // основной итерационный цикл
    do {
        // вычисляем новые значения для каждой переменной
        for (int i = 0; i < size; ++i) {
            // сумма для i-го уравнения
            double sum = 0;
            // перебираем все переменные кроме текущей
            for (int j = 0; j < size; ++j) {
                if (j != i) sum += matrix[i][j] * solution[j];
            }
            // вычисляем новое значение переменной
            newSolution[i] = (vectorB[i] - sum) / matrix[i][i];
        }
        
        // вычисляем погрешность как максимальное изменение между итерациями
        error = 0;
        for (int i = 0; i < size; ++i) {
            error = max(error, fabs(newSolution[i] - solution[i]));
        }
        
        // выводим номер текущей итерации
        cout << iterations << "\t";
        // выводим значения переменных с точностью 6 знаков после запятой
        for (int i = 0; i < size; ++i) {
            cout << fixed << setprecision(6) << newSolution[i] << "\t";
        }
        // выводим текущую погрешность
        cout << error << endl;
        
        // обновляем решение
        solution = newSolution;
        // увеличиваем счетчик итераций
        iterations++;
        
        // проверяем, не превышен ли лимит итераций
        if (iterations > maxIterations) {
            // выводим предупреждение
            cerr << "достигнут лимит итераций!" << endl;
            // прерываем цикл
            break;
        }
    } while (error > epsilon); // условие продолжения итераций
    
    // возвращаем полученное решение
    return solution;
}

// метод гаусса-зейделя для решения системы
vector<double> solveSeidel(const vector<vector<double>>& matrix, 
                         const vector<double>& vectorB, 
                         double epsilon, 
                         int maxIterations = 100) {
    // получаем размер системы
    int size = matrix.size();
    // создаем вектор для решения (начальное приближение - нули)
    vector<double> solution(size, 0);
    // счетчик итераций
    int iterations = 0;
    // переменная для хранения погрешности
    double error;
    
    // выводим заголовок для итерационного процесса
    cout << "\nметод гаусса-зейделя (начальное приближение - нулевое):\n";
    cout << "n\tx1\t\tx2\t\tx3\t\tx4\t\tεn\n";
    
    // основной итерационный цикл
    do {
        // сбрасываем погрешность перед новой итерацией
        error = 0;
        // сохраняем старое решение для сравнения
        vector<double> oldSolution = solution;
        
        // вычисляем новые значения переменных
        for (int i = 0; i < size; ++i) {
            // сумма для i-го уравнения
            double sum = 0;
            // используем уже обновленные значения переменных
            for (int j = 0; j < size; ++j) {
                if (j != i) sum += matrix[i][j] * solution[j];
            }
            // вычисляем новое значение переменной
            double newValue = (vectorB[i] - sum) / matrix[i][i];
            // обновляем погрешность
            error = max(error, fabs(newValue - solution[i]));
            // сразу записываем новое значение
            solution[i] = newValue;
        }
        
        // выводим номер текущей итерации
        cout << iterations << "\t";
        // выводим значения переменных с точностью 6 знаков после запятой
        for (int i = 0; i < size; ++i) {
            cout << fixed << setprecision(6) << solution[i] << "\t";
        }
        // выводим текущую погрешность
        cout << error << endl;
        
        // увеличиваем счетчик итераций
        iterations++;
        // проверяем, не превышен ли лимит итераций
        if (iterations > maxIterations) {
            // выводим предупреждение
            cerr << "достигнут лимит итераций!" << endl;
            // прерываем цикл
            break;
        }
    } while (error > epsilon); // условие продолжения итераций
    
    // возвращаем полученное решение
    return solution;
}

// главная функция программы
int main() {
    // задаем параметры системы
    double M = 1.09;
    double N = -0.16;
    double P = 0.84;

    // создаем матрицу коэффициентов системы
    vector<vector<double>> matrixA = {
        {M, -0.04, 0.21, -18},
        {0.25, -1.23, N, -0.09},
        {-0.21, N, 0.8, -0.13},
        {0.15, -1.31, 0.06, P}
    };

    // создаем вектор правой части системы
    vector<double> vectorB = {-1.24, P, 2.56, M};

    // выводим матрицу коэффициентов
    cout << "матрица коэффициентов:\n";
    printMatrix(matrixA);
    // выводим вектор правой части
    cout << "\nвектор правой части:\n";
    printVector(vectorB);

    // проверяем диагональное преобладание матрицы
    cout << "\nпроверка диагонального преобладания:\n";
    if (!checkDiagonalDominance(matrixA)) {
        // пытаемся сделать матрицу диагонально преобладающей
        cout << "попытка переставить строки для диагонального преобладания...\n";
        if (!tryMakeDiagonallyDominant(matrixA, vectorB)) {
            cout << "не удалось добиться диагонального преобладания!\n";
            cout << "итерационные методы могут не сойтись!\n";
        } else {
            cout << "теперь матрица имеет диагональное преобладание!\n";
            cout << "новая матрица:\n";
            printMatrix(matrixA);
        }
    }

    // решаем систему методом гаусса
    cout << "\n=== решение методом гаусса ===\n";
    vector<double> solutionGauss = solveGauss(matrixA, vectorB);
    cout << "\nрешение методом гаусса:\n";
    printVector(solutionGauss);
    cout << "невязка:\n";
    printVector(calculateResidual(matrixA, vectorB, solutionGauss));

    // решаем систему методом якоби
    cout << "\n=== решение методом якоби с точностью 0.001 ===\n";
    vector<double> solutionJacobi = solveJacobi(matrixA, vectorB, 1e-3);
    cout << "\nрешение методом якоби:\n";
    printVector(solutionJacobi);
    cout << "невязка:\n";
    printVector(calculateResidual(matrixA, vectorB, solutionJacobi));

    // решаем систему методом гаусса-зейделя
    cout << "\n=== решение методом гаусса-зейделя с точностью 0.001 ===\n";
    vector<double> solutionSeidel = solveSeidel(matrixA, vectorB, 1e-3);
    cout << "\nрешение методом гаусса-зейделя:\n";
    printVector(solutionSeidel);
    cout << "невязка:\n";
    printVector(calculateResidual(matrixA, vectorB, solutionSeidel));

    // сравниваем результаты всех методов
    cout << "\n=== сравнение результатов ===\n";
    cout << "метод\t\tx1\t\tx2\t\tx3\t\tx4\n";
    cout << "гаусса\t";
    for (double val : solutionGauss) cout << fixed << setprecision(6) << val << "\t";
    cout << "\nякоби\t";
    for (double val : solutionJacobi) cout << fixed << setprecision(6) << val << "\t";
    cout << "\nзейдель\t";
    for (double val : solutionSeidel) cout << fixed << setprecision(6) << val << "\t";
    cout << endl;


    return 0;
}