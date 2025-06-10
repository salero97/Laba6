#include <iostream>
#include <fstream>
#include <vector>
#include <array>
#include <string>
#include <cstdint>
#include <ctime>
#include <iomanip>
#include <cstring>
#include <algorithm>
#include <sstream>
#include <fstream> 

using namespace std;

// таблица замены байтов (s-box) для алгоритма aes
const uint8_t substitutionTable[256] = {
    0x63, 0x7C, 0x77, 0x7B, 0xF2, 0x6B, 0x6F, 0xC5, 0x30, 0x01, 0x67, 0x2B, 0xFE, 0xD7, 0xAB, 0x76,
    0xCA, 0x82, 0xC9, 0x7D, 0xFA, 0x59, 0x47, 0xF0, 0xAD, 0xD4, 0xA2, 0xAF, 0x9C, 0xA4, 0x72, 0xC0,
    0xB7, 0xFD, 0x93, 0x26, 0x36, 0x3F, 0xF7, 0xCC, 0x34, 0xA5, 0xE5, 0xF1, 0x71, 0xD8, 0x31, 0x15,
    0x04, 0xC7, 0x23, 0xC3, 0x18, 0x96, 0x05, 0x9A, 0x07, 0x12, 0x80, 0xE2, 0xEB, 0x27, 0xB2, 0x75,
    0x09, 0x83, 0x2C, 0x1A, 0x1B, 0x6E, 0x5A, 0xA0, 0x52, 0x3B, 0xD6, 0xB3, 0x29, 0xE3, 0x2F, 0x84,
    0x53, 0xD1, 0x00, 0xED, 0x20, 0xFC, 0xB1, 0x5B, 0x6A, 0xCB, 0xBE, 0x39, 0x4A, 0x4C, 0x58, 0xCF,
    0xD0, 0xEF, 0xAA, 0xFB, 0x43, 0x4D, 0x33, 0x85, 0x45, 0xF9, 0x02, 0x7F, 0x50, 0x3C, 0x9F, 0xA8,
    0x51, 0xA3, 0x40, 0x8F, 0x92, 0x9D, 0x38, 0xF5, 0xBC, 0xB6, 0xDA, 0x21, 0x10, 0xFF, 0xF3, 0xD2,
    0xCD, 0x0C, 0x13, 0xEC, 0x5F, 0x97, 0x44, 0x17, 0xC4, 0xA7, 0x7E, 0x3D, 0x64, 0x5D, 0x19, 0x73,
    0x60, 0x81, 0x4F, 0xDC, 0x22, 0x2A, 0x90, 0x88, 0x46, 0xEE, 0xB8, 0x14, 0xDE, 0x5E, 0x0B, 0xDB,
    0xE0, 0x32, 0x3A, 0x0A, 0x49, 0x06, 0x24, 0x5C, 0xC2, 0xD3, 0xAC, 0x62, 0x91, 0x95, 0xE4, 0x79,
    0xE7, 0xC8, 0x37, 0x6D, 0x8D, 0xD5, 0x4E, 0xA9, 0x6C, 0x56, 0xF4, 0xEA, 0x65, 0x7A, 0xAE, 0x08,
    0xBA, 0x78, 0x25, 0x2E, 0x1C, 0xA6, 0xB4, 0xC6, 0xE8, 0xDD, 0x74, 0x1F, 0x4B, 0xBD, 0x8B, 0x8A,
    0x70, 0x3E, 0xB5, 0x66, 0x48, 0x03, 0xF6, 0x0E, 0x61, 0x35, 0x57, 0xB9, 0x86, 0xC1, 0x1D, 0x9E,
    0xE1, 0xF8, 0x98, 0x11, 0x69, 0xD9, 0x8E, 0x94, 0x9B, 0x1E, 0x87, 0xE9, 0xCE, 0x55, 0x28, 0xDF,
    0x8C, 0xA1, 0x89, 0x0D, 0xBF, 0xE6, 0x42, 0x68, 0x41, 0x99, 0x2D, 0x0F, 0xB0, 0x54, 0xBB, 0x16
};

// константы для расширения ключа (Rcon)
const uint8_t roundConstants[11] = {
    0x00, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1B, 0x36
};

// функция преобразования массива байт в матрицу состояний 4x4
void ConvertBytesToStateMatrix(const uint8_t* inputBytes, uint8_t stateMatrix[4][4]) {
    // проходим по всем столбцам матрицы
    for (int column = 0; column < 4; ++column) {
        // проходим по всем строкам матрицы
        for (int row = 0; row < 4; ++row) {
            // заполняем матрицу по столбцам
            stateMatrix[row][column] = inputBytes[column * 4 + row];
        }
    }
}

// функция преобразования матрицы состояний в массив байт
void ConvertStateMatrixToBytes(const uint8_t stateMatrix[4][4], uint8_t* outputBytes) {
    // проходим по всем столбцам матрицы
    for (int column = 0; column < 4; ++column) {
        // проходим по всем строкам матрицы
        for (int row = 0; row < 4; ++row) {
            // преобразуем матрицу обратно в линейный массив
            outputBytes[column * 4 + row] = stateMatrix[row][column];
        }
    }
}

// функция подстановки байтов (SubBytes)
void SubstituteBytes(uint8_t stateMatrix[4][4]) {
    // проходим по всем строкам матрицы
    for (int row = 0; row < 4; ++row) {
        // проходим по всем столбцам матрицы
        for (int column = 0; column < 4; ++column) {
            // заменяем каждый байт соответствующим значением из таблицы замен
            stateMatrix[row][column] = substitutionTable[stateMatrix[row][column]];
        }
    }
}

// функция сдвига строк (ShiftRows)
void ShiftRows(uint8_t stateMatrix[4][4]) {
    // временный массив для хранения строки
    uint8_t tempRow[4];
    
    // вторая строка - циклический сдвиг на 1 байт влево
    for (int column = 0; column < 4; ++column) {
        tempRow[column] = stateMatrix[1][(column + 1) % 4];
    }
    // копируем сдвинутую строку обратно в матрицу
    memcpy(stateMatrix[1], tempRow, 4);
    
    // третья строка - циклический сдвиг на 2 байта влево
    for (int column = 0; column < 4; ++column) {
        tempRow[column] = stateMatrix[2][(column + 2) % 4];
    }
    // копируем сдвинутую строку обратно в матрицу
    memcpy(stateMatrix[2], tempRow, 4);
    
    // четвертая строка - циклический сдвиг на 3 байта влево
    for (int column = 0; column < 4; ++column) {
        tempRow[column] = stateMatrix[3][(column + 3) % 4];
    }
    // копируем сдвинутую строку обратно в матрицу
    memcpy(stateMatrix[3], tempRow, 4);
}

// вспомогательная функция для умножения в поле Галуа
uint8_t GaloisFieldMultiply(uint8_t value) {
    // умножение на x (0x02) в поле GF(2^8)
    uint8_t result = value << 1;
    // если старший бит был установлен, выполняем XOR 
    if (value & 0x80) {
        result ^= 0x1B;
    }
    return result;
}

// функция смешивания столбцов (MixColumns)
void MixColumns(uint8_t stateMatrix[4][4]) {
    // временный массив для хранения значений столбца
    uint8_t columnValues[4];
    
    // обрабатываем каждый столбец матрицы
    for (int column = 0; column < 4; ++column) {
        // сохраняем текущие значения столбца
        for (int row = 0; row < 4; ++row) {
            columnValues[row] = stateMatrix[row][column];
        }
        
        // применяем матричное преобразование MixColumns
        stateMatrix[0][column] = GaloisFieldMultiply(columnValues[0]) ^ 
                               GaloisFieldMultiply(columnValues[1]) ^ 
                               columnValues[1] ^ columnValues[2] ^ columnValues[3];
        
        stateMatrix[1][column] = columnValues[0] ^ 
                               GaloisFieldMultiply(columnValues[1]) ^ 
                               GaloisFieldMultiply(columnValues[2]) ^ 
                               columnValues[2] ^ columnValues[3];
        
        stateMatrix[2][column] = columnValues[0] ^ columnValues[1] ^ 
                               GaloisFieldMultiply(columnValues[2]) ^ 
                               GaloisFieldMultiply(columnValues[3]) ^ 
                               columnValues[3];
        
        stateMatrix[3][column] = GaloisFieldMultiply(columnValues[0]) ^ 
                               columnValues[0] ^ columnValues[1] ^ 
                               columnValues[2] ^ GaloisFieldMultiply(columnValues[3]);
    }
}

// функция добавления раундового ключа (AddRoundKey)
void AddRoundKey(uint8_t stateMatrix[4][4], const uint8_t* roundKey) {
    // проходим по всем строкам матрицы
    for (int row = 0; row < 4; ++row) {
        // проходим по всем столбцам матрицы
        for (int column = 0; column < 4; ++column) {
            // выполняем операцию XOR между байтом состояния и байтом ключа
            stateMatrix[row][column] ^= roundKey[row + 4 * column];
        }
    }
}

// функция расширения ключа (Key Expansion)
// функция для расширения ключа aes из 16 байт в 176 байт
// функция для расширения ключа aes из 16 байт в 176 байт с выводом в поток stringstream
void expandKey(const uint8_t key[16], uint8_t expanded[176], stringstream& ss) {
    // копируем исходный ключ в начало расширенного массива
    memcpy(expanded, key, 16);
    
    // вывод информации о начальном ключе (раунд 0)
    ss << "\nрасширение ключа:" << endl;
    ss << "раунд 0: ";
    for (int i = 0; i < 16; i++) {
        // выводим байты в шестнадцатеричном формате с ведущими нулями
        ss << hex << setw(2) << setfill('0') << (int)expanded[i] << " ";
    }
    ss << dec << endl;
    
    // генерируем оставшиеся слова расширенного ключа
    for (int wordIndex = 4; wordIndex < 44; wordIndex++) {
        uint8_t temp[4];
        // копируем предыдущее слово
        memcpy(temp, &expanded[(wordIndex-1)*4], 4);
        
        // для каждого четвертого слова выполняем специальные преобразования
        if (wordIndex % 4 == 0) {
            // циклический сдвиг байтов
            rotate(temp, temp + 1, temp + 4);
            // замена байтов через таблицу Substitution (SubWord)
            for (int i = 0; i < 4; i++) {
                temp[i] = substitutionTable[temp[i]];
            }
            // добавляем константу раунда (Rcon)
            temp[0] ^= roundConstants[wordIndex/4];
            
            // вывод ключа для текущего раунда
            ss << "раунд " << wordIndex/4 << ": ";
            for (int i = 0; i < 16; i++) {
                int pos = wordIndex*4 + i - (wordIndex*4 % 16);
                if (pos < 176) {
                    ss << hex << setw(2) << setfill('0') 
                       << (int)expanded[pos] << " ";
                }
            }
            ss << dec << endl;
        }
        // вычисляем новое слово как XOR предыдущего слова и слова 4 позиции назад
        for (int i = 0; i < 4; i++) {
            expanded[wordIndex*4 + i] = expanded[(wordIndex-4)*4 + i] ^ temp[i];
        }
    }
}


// функция для сохранения вывода в файл
// функция для сохранения данных в файл
void saveToFile(const string& filename, const string& content) {
    ofstream file(filename);
    if (file.is_open()) {
        file << content;  // записываем содержимое в файл
        cout << "данные сохранены в файл: " << filename << endl;
    } else {
        cerr << "ошибка при сохранении файла" << endl;
    }
}

// модифицированная функция вывода состояния
void printState(const uint8_t state[4][4], stringstream& ss) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            // форматированный вывод hex-значений
            ss << hex << setw(2) << setfill('0') << (int)state[i][j] << " ";
        }
        ss << endl;  // новая строка после каждой строки матрицы
    }
    ss << endl;  // дополнительный отступ после матрицы
}
void encryptBlock(uint8_t state[4][4], const uint8_t* roundKeys, stringstream& ss) {
    AddRoundKey(state, roundKeys);
    ss << "\nначальное состояние (после AddRoundKey):" << endl;
    printState(state, ss);
    
    for (int round = 1; round < 10; ++round) {
        SubstituteBytes(state);
        ss << "\nраунд " << round << " (после SubBytes):" << endl;
        printState(state, ss);
        
        ShiftRows(state);
        ss << "раунд " << round << " (после ShiftRows):" << endl;
        printState(state, ss);
        
        MixColumns(state);
        ss << "раунд " << round << " (после MixColumns):" << endl;
        printState(state, ss);
        
        AddRoundKey(state, roundKeys + round*16);
        ss << "раунд " << round << " (после AddRoundKey):" << endl;
        printState(state, ss);
    }
    
    SubstituteBytes(state);
    ShiftRows(state);
    AddRoundKey(state, roundKeys + 160);
    ss << "\nфинальное состояние (после последнего AddRoundKey):" << endl;
    printState(state, ss);
}
// функция реализации режима OFB
void processInOFBMode(const uint8_t* key, const uint8_t* iv, 
                     const uint8_t* input, uint8_t* output, 
                     size_t length, stringstream& ss) {
    uint8_t expandedKeys[176];
    expandKey(key, expandedKeys, ss);
    
    uint8_t state[4][4];
    uint8_t feedback[16];
    memcpy(feedback, iv, 16);
    
    size_t fullBlocks = length / 16;
    size_t remaining = length % 16;
    
    for (size_t block = 0; block < fullBlocks; ++block) {
        ConvertBytesToStateMatrix(feedback, state);
        encryptBlock(state, expandedKeys, ss);
        ConvertStateMatrixToBytes(state, feedback);
        
        for (int i = 0; i < 16; ++i) {
            output[block*16 + i] = input[block*16 + i] ^ feedback[i];
        }
    }
    
    if (remaining > 0) {
        ConvertBytesToStateMatrix(feedback, state);
        encryptBlock(state, expandedKeys, ss);
        ConvertStateMatrixToBytes(state, feedback);
        
        for (size_t i = 0; i < remaining; ++i) {
            output[fullBlocks*16 + i] = input[fullBlocks*16 + i] ^ feedback[i];
        }
    }
}

// функция генерации случайного 128-битного ключа
void GenerateRandomKey(uint8_t key[16]) {
    // инициализируем генератор случайных чисел
    srand(time(nullptr));
    
    // генерируем 16 случайных байтов
    for (int i = 0; i < 16; ++i) {
        key[i] = rand() % 256;
    }
    
    // выводим сгенерированный ключ в шестнадцатеричном формате
    cout << "сгенерированный ключ (hex): ";
    for (int i = 0; i < 16; ++i) {
        cout << hex << setw(2) << setfill('0') << (int)key[i] << " ";
    }
    cout << dec << endl;
}

// функция генерации вектора инициализации
void GenerateInitializationVector(uint8_t iv[16]) {
    // генерируем 16 случайных байтов
    for (int i = 0; i < 16; ++i) {
        iv[i] = rand() % 256;
    }
    
    // выводим сгенерированный вектор инициализации
    cout << "сгенерированный iv (hex): ";
    for (int i = 0; i < 16; ++i) {
        cout << hex << setw(2) << setfill('0') << (int)iv[i] << " ";
    }
    cout << dec << endl;
}

// функция сохранения данных в файл
void SaveDataToFile(const string& filename, const uint8_t* data, size_t length) {
    // открываем файл для записи в бинарном режиме
    ofstream file(filename, ios::binary);
    if (!file) {
        cerr << "ошибка при создании файла " << filename << endl;
        return;
    }
    
    // записываем данные в файл
    file.write(reinterpret_cast<const char*>(data), length);
}

// функция загрузки данных из файла
vector<uint8_t> LoadDataFromFile(const string& filename) {
    // открываем файл для чтения в бинарном режиме
    ifstream file(filename, ios::binary | ios::ate);
    if (!file) {
        cerr << "ошибка при открытии файла " << filename << endl;
        return {};
    }
    
    // определяем размер файла
    size_t size = file.tellg();
    file.seekg(0, ios::beg);
    
    // создаем вектор для хранения данных
    vector<uint8_t> data(size);
    // читаем данные из файла
    file.read(reinterpret_cast<char*>(data.data()), size);
    
    return data;
}

int main() {
    // устанавливаем локаль для корректного отображения русских символов
    setlocale(LC_ALL, "Russian");
    // инициализируем генератор случайных чисел
    srand(time(nullptr));

    // создаем поток для сбора всего вывода
    stringstream outputStream;

    // выводим заголовок программы
    outputStream << "==============================================" << endl;
    outputStream << "|| программа для шифрования/дешифрования aes ||" << endl;
    outputStream << "||          режим работы: ofb               ||" << endl;
    outputStream << "||          размер ключа: 128 бит           ||" << endl;
    outputStream << "==============================================\n" << endl;
    
    // объявляем массив для хранения ключа
    uint8_t encryptionKey[16];
    // объявляем массив для хранения вектора инициализации
    uint8_t initializationVector[16];
    
    // генерируем ключ шифрования
    outputStream << "генерируем ключ шифрования..." << endl;
    GenerateRandomKey(encryptionKey);
    // генерируем вектор инициализации
    outputStream << "генерируем вектор инициализации..." << endl;
    GenerateInitializationVector(initializationVector);

    // создаем вектор для хранения входных данных
    vector<uint8_t> inputData;
    // предлагаем пользователю выбрать источник данных
    cout << "\nвыберите источник данных:" << endl;
    cout << "1 - ввод текста вручную" << endl;
    cout << "2 - загрузка из файла" << endl;
    cout << "введите номер выбора: ";
    
    int choice;
    cin >> choice;
    cin.ignore();

    if (choice == 1) {
        // ввод текста с клавиатуры
        cout << "\nвведите текст для обработки: ";
        string text;
        getline(cin, text);
        // копируем символы в вектор байтов
        inputData.assign(text.begin(), text.end());
    } 
    else if (choice == 2) {
        // загрузка данных из файла
        cout << "\nвведите имя файла: ";
        string filename;
        getline(cin, filename);
        inputData = LoadDataFromFile(filename);
        if (inputData.empty()) {
            cerr << "не удалось загрузить данные из файла" << endl;
            return 1;
        }
    } 
    else {
        cerr << "неверный выбор" << endl;
        return 1;
    }

    // создаем буфер для зашифрованных данных
    vector<uint8_t> encryptedData(inputData.size());
    // создаем буфер для расшифрованных данных
    vector<uint8_t> decryptedData(inputData.size());

    // шифруем данные
    outputStream << "\nначало шифрования...\n";
    processInOFBMode(encryptionKey, initializationVector,
                    inputData.data(), encryptedData.data(), 
                    inputData.size(), outputStream);
    outputStream << "шифрование завершено" << endl;

    // выводим зашифрованные данные в шестнадцатеричном формате
    outputStream << "\nзашифрованные данные (hex):" << endl;
    for (auto byte : encryptedData) {
        outputStream << hex << setw(2) << setfill('0') << (int)byte << " ";
    }
    outputStream << dec << endl;

    // дешифруем данные
    outputStream << "\nначало дешифрования...\n";
    processInOFBMode(encryptionKey, initializationVector,
                    encryptedData.data(), decryptedData.data(),
                    encryptedData.size(), outputStream);
    outputStream << "дешифрование завершено" << endl;

    // выводим результат дешифрования
    outputStream << "\nрезультат дешифрования:" << endl;
    outputStream << "----------------------------------------" << endl;
    outputStream.write(reinterpret_cast<const char*>(decryptedData.data()), decryptedData.size());
    outputStream << endl << "----------------------------------------" << endl;

    // выводим все на экран
    cout << outputStream.str();

    // предлагаем сохранить в файл
    cout << "\nсохранить вывод в файл? (y/n): ";
    char saveChoice;
    cin >> saveChoice;
    cin.ignore();
    
    if (tolower(saveChoice) == 'y') {
        cout << "введите имя файла: ";
        string filename;
        getline(cin, filename);
        
        // проверка существования файла
        ifstream test(filename);
        if (test.good()) {
            cout << "файл существует, перезаписать? (y/n): ";
            char overwrite;
            cin >> overwrite;
            cin.ignore();
            if (tolower(overwrite) == 'y') {
                // сохраняем в файл
                saveToFile(filename, outputStream.str());
            }
        } else {
            // сохраняем в новый файл
            saveToFile(filename, outputStream.str());
        }
    }
    
    return 0;
}
