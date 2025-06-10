use std::f64; 
use std::cmp; 
use std::fmt; 

// функция для вывода матрицы на экран
fn print_matrix(matrix: &Vec<Vec<f64>>) {
    // перебираем каждую строку матрицы
    for row in matrix {
        // перебираем каждый элемент строки
        for value in row {
            // выводим элемент с шириной 10 символов и 2 знаками после запятой
            print!("{:10.2} ", value);
        }
        // переходим на новую строку после вывода всех элементов строки
        println!();
    }
}

// функция для вывода вектора на экран
fn print_vector(vec: &Vec<f64>) {
    // перебираем все элементы вектора
    for value in vec {
        // выводим элемент с шириной 10 символов и 6 знаками после запятой
        print!("{:10.6} ", value);
    }
    // переходим на новую строку после вывода всех элементов
    println!();
}

// функция проверки диагонального преобладания матрицы
fn check_diagonal_dominance(matrix: &Vec<Vec<f64>>) -> bool {
    // получаем размер матрицы (количество строк/столбцов)
    let size = matrix.len();
    // перебираем все строки матрицы
    for i in 0..size {
        // переменная для суммы модулей недиагональных элементов
        let mut sum = 0.0;
        // перебираем все элементы строки
        for j in 0..size {
            // если элемент не диагональный
            if j != i {
                // добавляем его модуль к сумме
                sum += f64::abs(matrix[i][j]);
            }
        }
        // если диагональный элемент меньше или равен сумме остальных
        if f64::abs(matrix[i][i]) <= sum {
            // выводим сообщение о проблеме
            println!("нет диагонального преобладания в строке {}", i+1);
            // возвращаем false (нет преобладания)
            return false;
        }
    }
    // если все строки прошли проверку, возвращаем true
    true
}

// функция для попытки сделать матрицу диагонально преобладающей
fn try_make_diagonally_dominant(matrix: &mut Vec<Vec<f64>>, vector_b: &mut Vec<f64>) -> bool {
    // получаем размер матрицы
    let size = matrix.len();
    // перебираем все строки
    for i in 0..size {
        // предполагаем, что текущая строка имеет максимальный диагональный элемент
        let mut max_row = i;
        // запоминаем модуль диагонального элемента
        let mut max_value = f64::abs(matrix[i][i]);
        // ищем строку с максимальным диагональным элементом в текущем столбце
        for k in i+1..size {
            // если нашли больший элемент
            if f64::abs(matrix[k][i]) > max_value {
                // обновляем максимальное значение
                max_value = f64::abs(matrix[k][i]);
                // запоминаем номер строки с максимальным элементом
                max_row = k;
            }
        }
        // если нашли строку с большим диагональным элементом
        if max_row != i {
            // меняем строки местами в матрице
            matrix.swap(i, max_row);
            // меняем соответствующие элементы в векторе правой части
            vector_b.swap(i, max_row);
            // выводим сообщение о перестановке
            println!("перестановка строк {} и {} для диагонального преобладания", i+1, max_row+1);
        }
        // проверяем, стало ли выполнено условие диагонального преобладания
        let mut sum = 0.0;
        // вычисляем сумму модулей недиагональных элементов
        for j in 0..size {
            if j != i {
                sum += f64::abs(matrix[i][j]);
            }
        }
        // если диагональный элемент меньше суммы остальных
        if f64::abs(matrix[i][i]) <= sum {
            // возвращаем false (не удалось сделать преобладающей)
            return false;
        }
    }
    // если все получилось, возвращаем true
    true
}

// функция для вычисления невязки
fn calculate_residual(matrix: &Vec<Vec<f64>>, vector_b: &Vec<f64>, solution: &Vec<f64>) -> Vec<f64> {
    // получаем размер системы
    let size = matrix.len();
    // создаем вектор для хранения невязки
    let mut residual = vec![0.0; size];
    // перебираем все уравнения системы
    for i in 0..size {
        // начинаем с -b[i]
        residual[i] = -vector_b[i];
        // добавляем сумму A[i][j]*x[j]
        for j in 0..size {
            residual[i] += matrix[i][j] * solution[j];
        }
    }
    // возвращаем вектор невязки
    residual
}

// метод гаусса с выбором главного элемента
fn solve_gauss(mut matrix: Vec<Vec<f64>>, mut vector_b: Vec<f64>) -> Vec<f64> {
    // получаем размер системы
    let size = matrix.len();
    // создаем вектор для хранения решения
    let mut solution = vec![0.0; size];
    
    // выводим заголовок для прямого хода
    println!("\nпрямой ход метода гаусса:");
    
    // прямой ход метода (приведение к треугольному виду)
    for k in 0..size {
        // выбор главного элемента в текущем столбце
        let mut max_row = k;
        // запоминаем модуль текущего диагонального элемента
        let mut max_value = f64::abs(matrix[k][k]);
        // ищем максимальный элемент в столбце ниже текущего
        for i in k+1..size {
            // если нашли больший элемент
            if f64::abs(matrix[i][k]) > max_value {
                // обновляем максимальное значение
                max_value = f64::abs(matrix[i][k]);
                // запоминаем строку с максимальным элементом
                max_row = i;
            }
        }
        
        // если нашли строку с большим элементом
        if max_row != k {
            // меняем строки местами в матрице
            matrix.swap(k, max_row);
            // меняем соответствующие элементы в векторе правой части
            vector_b.swap(k, max_row);
            // выводим сообщение о перестановке
            println!("перестановка строк {} и {}", k+1, max_row+1);
        }
        
        // проверка на вырожденность матрицы
        if f64::abs(matrix[k][k]) < 1e-10 {
            // выводим сообщение об ошибке
            eprintln!("матрица вырожденная!");
            // завершаем программу с кодом ошибки
            std::process::exit(1);
        }
        
        // нормируем текущую строку
        let divisor = matrix[k][k];
        // делим все элементы строки на диагональный элемент
        for j in k..size {
            matrix[k][j] /= divisor;
        }
        // делим соответствующий элемент вектора правой части
        vector_b[k] /= divisor;
        
        // выводим матрицу после нормировки
        println!("после нормировки строки {}:", k+1);
        print_matrix(&matrix);
        println!("вектор b: ");
        print_vector(&vector_b);
        
        // исключаем текущую переменную из всех нижних строк
        for i in k+1..size {
            // вычисляем коэффициент для вычитания
            let factor = matrix[i][k];
            // вычитаем из текущей строки нормированную строку, умноженную на коэффициент
            for j in k..size {
                matrix[i][j] -= factor * matrix[k][j];
            }
            // аналогично для вектора правой части
            vector_b[i] -= factor * vector_b[k];
        }
        
        // выводим матрицу после исключения
        println!("после исключения в столбце {}:", k+1);
        print_matrix(&matrix);
        println!("вектор b: ");
        print_vector(&vector_b);
    }
    
    // выводим заголовок для обратного хода
    println!("\nобратный ход метода гаусса:");
    
    // обратный ход (нахождение решения)
    for k in (0..size).rev() {
        // начинаем с элемента вектора правой части
        solution[k] = vector_b[k];
        // вычитаем уже найденные переменные
        for j in k+1..size {
            solution[k] -= matrix[k][j] * solution[j];
        }
        // выводим найденное значение переменной
        println!("x[{}] = {}", k+1, solution[k]);
    }
    
    // возвращаем полученное решение
    solution
}

// метод якоби для решения системы
fn solve_jacobi(matrix: &Vec<Vec<f64>>, vector_b: &Vec<f64>, epsilon: f64, max_iterations: usize) -> Vec<f64> {
    // получаем размер системы
    let size = matrix.len();
    // создаем вектор для решения (начальное приближение - нули)
    let mut solution = vec![0.0; size];
    // вектор для новых значений на следующей итерации
    let mut new_solution = vec![0.0; size];
    // счетчик итераций
    let mut iterations = 0;
    // переменная для хранения погрешности
    let mut error;
    
    // выводим заголовок для итерационного процесса
    println!("\nметод якоби (начальное приближение - нулевое):");
    println!("n\tx1\t\tx2\t\tx3\t\tx4\t\tεn");
    
    // основной итерационный цикл
    loop {
        // вычисляем новые значения для каждой переменной
        for i in 0..size {
            // сумма для i-го уравнения
            let mut sum = 0.0;
            // перебираем все переменные кроме текущей
            for j in 0..size {
                if j != i {
                    sum += matrix[i][j] * solution[j];
                }
            }
            // вычисляем новое значение переменной
            new_solution[i] = (vector_b[i] - sum) / matrix[i][i];
        }
        
        // вычисляем погрешность как максимальное изменение между итерациями
        error = 0.0;
        for i in 0..size {
            error = f64::max(error, f64::abs(new_solution[i] - solution[i]));
        }
        
        // выводим номер текущей итерации
        print!("{}\t", iterations);
        // выводим значения переменных с точностью 6 знаков после запятой
        for i in 0..size {
            print!("{:.6}\t", new_solution[i]);
        }
        // выводим текущую погрешность
        println!("{:.6}", error);
        
        // обновляем решение
        solution.copy_from_slice(&new_solution);
        // увеличиваем счетчик итераций
        iterations += 1;
        
        // проверяем условие выхода из цикла
        if error <= epsilon || iterations > max_iterations {
            // проверяем, не превышен ли лимит итераций
            if iterations > max_iterations {
                // выводим предупреждение
                eprintln!("достигнут лимит итераций!");
            }
            // прерываем цикл
            break;
        }
    }
    
    // возвращаем полученное решение
    solution
}

// метод гаусса-зейделя для решения системы
fn solve_seidel(matrix: &Vec<Vec<f64>>, vector_b: &Vec<f64>, epsilon: f64, max_iterations: usize) -> Vec<f64> {
    // получаем размер системы
    let size = matrix.len();
    // создаем вектор для решения (начальное приближение - нули)
    let mut solution = vec![0.0; size];
    // счетчик итераций
    let mut iterations = 0;
    // переменная для хранения погрешности
    let mut error;
    
    // выводим заголовок для итерационного процесса
    println!("\nметод гаусса-зейделя (начальное приближение - нулевое):");
    println!("n\tx1\t\tx2\t\tx3\t\tx4\t\tεn");
    
    // основной итерационный цикл
    loop {
        // сбрасываем погрешность перед новой итерацией
        error = 0.0;
        // сохраняем старое решение для сравнения
        let old_solution = solution.clone();
        
        // вычисляем новые значения переменных
        for i in 0..size {
            // сумма для i-го уравнения
            let mut sum = 0.0;
            // используем уже обновленные значения переменных
            for j in 0..size {
                if j != i {
                    sum += matrix[i][j] * solution[j];
                }
            }
            // вычисляем новое значение переменной
            let new_value = (vector_b[i] - sum) / matrix[i][i];
            // обновляем погрешность
            error = f64::max(error, f64::abs(new_value - solution[i]));
            // сразу записываем новое значение
            solution[i] = new_value;
        }
        
        // выводим номер текущей итерации
        print!("{}\t", iterations);
        // выводим значения переменных с точностью 6 знаков после запятой
        for i in 0..size {
            print!("{:.6}\t", solution[i]);
        }
        // выводим текущую погрешность
        println!("{:.6}", error);
        
        // увеличиваем счетчик итераций
        iterations += 1;
        // проверяем условие выхода из цикла
        if error <= epsilon || iterations > max_iterations {
            // проверяем, не превышен ли лимит итераций
            if iterations > max_iterations {
                // выводим предупреждение
                eprintln!("достигнут лимит итераций!");
            }
            // прерываем цикл
            break;
        }
    }
    
    // возвращаем полученное решение
    solution
}

fn main() {
    // задаем параметры системы
    let m = 1.09;
    let n = -0.16;
    let p = 0.84;

    // создаем матрицу коэффициентов системы
    let mut matrix_a = vec![
        vec![m, -0.04, 0.21, -18.0],
        vec![0.25, -1.23, n, -0.09],
        vec![-0.21, n, 0.8, -0.13],
        vec![0.15, -1.31, 0.06, p]
    ];

    // создаем вектор правой части системы
    let mut vector_b = vec![-1.24, p, 2.56, m];

    // выводим матрицу коэффициентов
    println!("матрица коэффициентов:");
    print_matrix(&matrix_a);
    // выводим вектор правой части
    println!("\nвектор правой части:");
    print_vector(&vector_b);

    // проверяем диагональное преобладание матрицы
    println!("\nпроверка диагонального преобладания:");
    if !check_diagonal_dominance(&matrix_a) {
        // пытаемся сделать матрицу диагонально преобладающей
        println!("попытка переставить строки для диагонального преобладания...");
        if !try_make_diagonally_dominant(&mut matrix_a, &mut vector_b) {
            println!("не удалось добиться диагонального преобладания!");
            println!("итерационные методы могут не сойтись!");
        } else {
            println!("теперь матрица имеет диагональное преобладание!");
            println!("новая матрица:");
            print_matrix(&matrix_a);
        }
    }

    // решаем систему методом гаусса
    println!("\n=== решение методом гаусса ===");
    let solution_gauss = solve_gauss(matrix_a.clone(), vector_b.clone());
    println!("\nрешение методом гаусса:");
    print_vector(&solution_gauss);
    println!("невязка:");
    print_vector(&calculate_residual(&matrix_a, &vector_b, &solution_gauss));

    // решаем систему методом якоби
    println!("\n=== решение методом якоби с точностью 0.001 ===");
    let solution_jacobi = solve_jacobi(&matrix_a, &vector_b, 1e-3, 100);
    println!("\nрешение методом якоби:");
    print_vector(&solution_jacobi);
    println!("невязка:");
    print_vector(&calculate_residual(&matrix_a, &vector_b, &solution_jacobi));

    // решаем систему методом гаусса-зейделя
    println!("\n=== решение методом гаусса-зейделя с точностью 0.001 ===");
    let solution_seidel = solve_seidel(&matrix_a, &vector_b, 1e-3, 100);
    println!("\nрешение методом гаусса-зейделя:");
    print_vector(&solution_seidel);
    println!("невязка:");
    print_vector(&calculate_residual(&matrix_a, &vector_b, &solution_seidel));

    // сравниваем результаты всех методов
    println!("\n=== сравнение результатов ===");
    println!("метод\t\tx1\t\tx2\t\tx3\t\tx4");
    print!("гаусса\t");
    for val in &solution_gauss {
        print!("{:.6}\t", val);
    }
    print!("\nякоби\t");
    for val in &solution_jacobi {
        print!("{:.6}\t", val);
    }
    print!("\nзейдель\t");
    for val in &solution_seidel {
        print!("{:.6}\t", val);
    }
    println!();
}