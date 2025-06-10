#include <iostream>
#include <iomanip>  
#include <vector>
#include <algorithm>
#include <map>
#include <ctime>
#include <cstdlib>

using namespace std;

// функция для создания матрицы и заполнения ее случайными числами в заданном диапазоне
vector<vector<int>> createMatrix(int rows, int cols, int minVal, int maxVal) {
    vector<vector<int>> matrix(rows, vector<int>(cols)); // создаем матрицу из векторов, размером rows x cols
    for (int i = 0; i < rows; ++i) { // цикл по строкам
        for (int j = 0; j < cols; ++j) { // цикл по столбцам
            matrix[i][j] = rand() % (maxVal - minVal + 1) + minVal; // заполняем элемент случайным числом в диапазоне
        }
    }
    return matrix; // возвращаем заполненную матрицу
}

// функция для вывода матрицы на экран
void printMatrix(const vector<vector<int>>& matrix) {
    for (const auto& row : matrix) { // перебираем каждую строку матрицы
        for (int val : row) { // перебираем элементы строки
            cout << val << " "; // выводим элемент с пробелом
        }
        cout << endl; // перевод строки после каждой строки матрицы
    }
}

// функция для графического вывода шахматной доски
void printChessBoard(const pair<int, int>& kingPos, const vector<pair<int, int>>& queens, const vector<pair<int, int>>& knights) {
    // Символы для фигур
    const char* king = "K";
    const char* queen = "Q";
    const char* knight = "H";
    const char* empty = "·";
    
    cout << "  a b c d e f g h" << endl;

    for (int i = 0; i < 8; ++i) {
        // вывод номера строки (от 8 до 1)
        cout << 8 - i << " ";
        for (int j = 0; j < 8; ++j) {
            // проверка, есть ли на текущей позиции король
            bool isKing = (i == kingPos.first && j == kingPos.second);
            // флаги для ферзя и коня
            bool isQueen = false;
            bool isKnight = false;
            
            // проверка наличия ферзя на текущей позиции
            for (const auto& q : queens) {
                if (i == q.first && j == q.second) {
                    isQueen = true;
                    break;
                }
            }
            
            // проверка наличия коня на текущей позиции
            for (const auto& k : knights) {
                if (i == k.first && j == k.second) {
                    isKnight = true;
                    break;
                }
            }
            
            // вывод фигуры или клетки
            if (isKing) {
                cout << king << " ";
            } else if (isQueen) {
                cout << queen << " ";
            } else if (isKnight) {
                cout << knight << " ";
            } else {
                // чередование цветов клеток (светлая и тёмная)
                if ((i + j) % 2 == 0) {
                    // светлая клетка с фоном
                    cout << "\033[48;5;7m" << empty << "\033[0m" << " ";
                } else {
                    // тёмная клетка без фона
                    cout << empty << " ";
                }
            }
        }
        // вывод номера строки в конце строки
        cout << 8 - i << endl;
    }
    // повторный вывод заголовка с буквенными обозначениями
    cout << "  a b c d e f g h" << endl;
}

// функция для поиска возрастающей последовательности с минимальным начальным элементом в строках матрицы
vector<int> findIncreasingSequence(const vector<vector<int>>& matrix) {
    vector<int> result; // вектор для хранения найденной последовательности
    int minStart = 101; // начальное значение больше максимального возможного элемента 

    for (const auto& row : matrix) { // перебираем каждую строку матрицы
        for (int i = 0; i < row.size(); ++i) { // перебираем каждый элемент строки
            vector<int> currentSeq = {row[i]}; // начинаем новую последовательность с текущего элемента
            for (int j = i + 1; j < row.size(); ++j) { // ищем продолжение последовательности
                if (row[j] > currentSeq.back()) { // если следующий элемент больше последнего в текущей последовательности
                    currentSeq.push_back(row[j]); // добавляем его в последовательность
                } else {
                    break; // если не больше, прерываем поиск для этой стартовой точки
                }
            }
            if (currentSeq.size() > 1 && currentSeq[0] < minStart) { // если последовательность длиной > 1 и её старт меньше текущего минимального
                minStart = currentSeq[0]; // обновляем минимальный старт
                result = currentSeq; // сохраняем текущую последовательность как лучшую
            }
        }
    }

    return result; // возвращаем найденную последовательность
}

// функция для подсчета количества вхождений каждого уникального элемента в матрице
map<int, int> countUniqueElements(const vector<vector<int>>& matrix) {
    map<int, int> counts; // создаем отображение для подсчета элементов
    for (const auto& row : matrix) { // перебираем строки
        for (int val : row) { // перебираем элементы строки
            counts[val]++; // увеличиваем счетчик для текущего элемента
        }
    }
    return counts; // возвращаем отображение с подсчетами
}

// функция для проверки, атакует ли конь короля
bool isKnightAttacking(int nx, int ny, int kx, int ky) {
    int dx = abs(nx - kx); // разница по x
    int dy = abs(ny - ky); // разница по y
    return (dx == 1 && dy == 2) || (dx == 2 && dy == 1); // конь атакует, если ходит по "букве Г"
}

bool isQueenAttackCheck(int qx, int qy, int kx, int ky,
                     const vector<pair<int, int>>& queens,
                     const vector<pair<int, int>>& knights,
                     const pair<int, int>& kingpos) {
    
    // проверяем, находятся ли ферзь и король на одной линии
    if (!(qx == kx || qy == ky || abs(qx - kx) == abs(qy - ky))) {
        return false; // если нет прямой линии - атаки быть не может
    }

    // определяем направление движения от ферзя к королю
    int dx = (kx == qx) ? 0 : (kx > qx) ? 1 : -1; // шаг по x
    int dy = (ky == qy) ? 0 : (ky > qy) ? 1 : -1;  // шаг по y

    // начинаем проверку клеток между ферзем и королем
    int x = qx + dx;
    int y = qy + dy;
    while (x != kx || y != ky) {
        // проверяем, не стоит ли на пути другой ферзь (кроме текущего)
        for (const auto& q : queens) {
            if (q != make_pair(qx, qy) && q == make_pair(x, y)) {
                return false; // если нашли другого ферзя - путь перекрыт
            }
        }
        
        // проверяем, не стоит ли на пути конь
        for (const auto& k : knights) {
            if (k == make_pair(x, y)) {
                return false; // если нашли коня - путь перекрыт
            }
        }
        
        // проверяем, не стоит ли на пути сам король (кроме конечной позиции)
        if (make_pair(x, y) == kingpos && (x != kx || y != ky)) {
            return false; // если нашли короля на пути - путь перекрыт
        }

        // переходим к следующей клетке по направлению к королю
        x += dx;
        y += dy;
    }

    return true; // если дошли до короля без препятствий - атака возможна
}
// функция для генерации случайных координат на шахматной доске
pair<int, int> generateRandomPosition() {
    return {rand() % 8, rand() % 8}; // возвращает пару случайных чисел от 0 до 7
}

// функция проверки уникальности позиций
bool ispositionunique(const vector<pair<int, int>>& positions, const pair<int, int>& newpos) {
    // проверяем, что новая позиция не совпадает с существующими
    return find(positions.begin(), positions.end(), newpos) == positions.end();
}

int main() {
    srand(time(0)); // инициализация генератора случайных чисел текущим временем


    // задача 1: поиск возрастающей последовательности с минимальным начальным элементом
    cout << "задача 1:" << endl;
    int m = 5, n = 5; // размеры матрицы
    auto mat = createMatrix(m, n, 0, 100); // создаем матрицу 5x5 с числами от 0 до 100
    cout << "матрица:" << endl; // выводим заголовок
    printMatrix(mat); // выводим матрицу
    auto seq = findIncreasingSequence(mat); // ищем возрастающую последовательность
    cout << "найденная последовательность:" << endl; // выводим заголовок
    for (int num : seq) { // перебираем элементы последовательности
        cout << num << " "; // выводим элемент
    }
    cout << endl << endl; 

    // задача 2: подсчет уникальных элементов в матрице
    cout << "задача 2:" << endl; // выводим заголовок
    auto mat2 = createMatrix(m, n, -50, 50); // создаем матрицу 5x5 с числами от -50 до 50
    cout << "матрица:" << endl; // выводим заголовок
    printMatrix(mat2); // выводим матрицу
    auto counts = countUniqueElements(mat2); // считаем вхождения каждого элемента
    cout << "количество вхождений каждого элемента:" << endl; // выводим заголовок
    for (const auto& pair : counts) { // перебираем пары ключ-значение
        cout << pair.first << ": " << pair.second << endl; // выводим элемент и его количество
    }
    cout << endl; // перевод строки

    // задача 3: шахматная доска с ферзями и королем
    cout << "задача 3:" << endl; // выводим заголовок
// генерируем позицию короля
auto kingPos = generateRandomPosition();

// добавляем случайных ферзей с проверкой уникальности
vector<pair<int, int>> queens;
for (int i = 0; i < 3; ++i) {
    pair<int, int> newQueen;
    do {
        newQueen = generateRandomPosition();
    } while (!ispositionunique(queens, newQueen) || newQueen == kingPos);
    queens.push_back(newQueen);
}

// добавляем случайных коней с проверкой уникальности
vector<pair<int, int>> knights;
for (int i = 0; i < 2; ++i) {
    pair<int, int> newKnight;
    do {
        newKnight = generateRandomPosition();
    } while (!ispositionunique(knights, newKnight) || 
             !ispositionunique(queens, newKnight) || 
             newKnight == kingPos);
    knights.push_back(newKnight);
}
// лямбда-функция для преобразования координат в шахматную нотацию
auto toChessNotation = [](int row, int col) {
    
    return string(1, 'a' + col) + to_string(8 - row);
};

// проверка на наличие дубликатов перед дальнейшей обработкой
bool hasDuplicates = false;

// проверяем ферзей на дубликаты
for (size_t i = 0; i < queens.size(); ++i) {
    for (size_t j = i + 1; j < queens.size(); ++j) {
        // сравниваем текущего ферзя с остальными
        if (queens[i] == queens[j]) {
            // выводим сообщение об ошибке с указанием клетки
            cout << "ошибка: два ферзя на клетке " 
                 << toChessNotation(queens[i].first, queens[i].second) << endl;
            // отмечаем наличие дубликатов
            hasDuplicates = true;
        }
    }
}

// проверяем коней на дубликаты
for (size_t i = 0; i < knights.size(); ++i) {
    for (size_t j = i + 1; j < knights.size(); ++j) {
        // сравниваем текущего коня с остальными
        if (knights[i] == knights[j]) {
            // выводим сообщение об ошибке с указанием клетки
            cout << "ошибка: два коня на клетке " 
                 << toChessNotation(knights[i].first, knights[i].second) << endl;
            // отмечаем наличие дубликатов
            hasDuplicates = true;
        }
    }
}

// если найдены дубликаты, выводим сообщение и завершаем программу
if (hasDuplicates) {
    cout << "обнаружены дубликаты позиций! программа завершена." << endl;
    return 1;
}


cout << "позиция короля: " << toChessNotation(kingPos.first, kingPos.second) 
     << " (строка " << kingPos.first << ", столбец " << kingPos.second << ")" << endl;


cout << "позиции ферзей:" << endl;
for (const auto& queen : queens) {
    cout << toChessNotation(queen.first, queen.second) 
         << " (строка " << queen.first << ", столбец " << queen.second << ") ";
}
cout << endl;

// вывод позиций коней
cout << "позиции коней:" << endl;
for (const auto& knight : knights) {
    // преобразование координат коня в шахматную нотацию и вывод
    cout << toChessNotation(knight.first, knight.second) 
         << " (строка " << knight.first << ", столбец " << knight.second << ") ";
}
cout << endl;

    // проверяем, какие ферзи атакуют короля
    cout << "ферзи, атакующие короля:" << endl;
    for (const auto& queen : queens) { // перебираем ферзей
// Gроверяем, какие ферзи атакуют короля
    if (isQueenAttackCheck(queen.first, queen.second, 
                        kingPos.first, kingPos.second,
                        queens, knights, kingPos)) {
        cout << toChessNotation(queen.first, queen.second) << " ";
    }
}
cout << endl; // перевод строки

    // проверяем, какие кони атакуют короля
cout << "кони, атакующие короля:" << endl;
for (const auto& knight : knights) { // перебираем коней
    if (isKnightAttacking(knight.first, knight.second, kingPos.first, kingPos.second)) { // проверка атаки
        cout << toChessNotation(knight.first, knight.second) << " "; // выводим в шахматной нотации
    }
}
cout << endl; // перевод строки


    // графический вывод доски
    cout << "Шахматная доска:" << endl;
    printChessBoard(kingPos, queens, knights);
    cout << endl;

    return 0; 
}
