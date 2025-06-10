use rand::Rng;
use std::collections::HashMap;
use std::fmt;

// функция для создания матрицы и заполнения ее случайными числами
fn create_matrix(rows: usize, cols: usize, min_val: i32, max_val: i32) -> Vec<Vec<i32>> {
    // создаем генератор случайных чисел
    let mut rng = rand::thread_rng();
    // создаем матрицу с помощью итератора
    (0..rows)
        .map(|_| {
            // для каждой строки создаем вектор случайных чисел
            (0..cols)
                .map(|_| rng.gen_range(min_val..=max_val))
                .collect()
        })
        .collect()
}

// функция для вывода матрицы на экран
fn print_matrix(matrix: &[Vec<i32>]) {
    // перебираем каждую строку матрицы
    for row in matrix {
        // перебираем каждый элемент строки
        for val in row {
            // выводим элемент с пробелом
            print!("{} ", val);
        }
        // перевод строки после каждой строки матрицы
        println!();
    }
}

// структура для представления позиции на шахматной доске
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
struct Position {
    row: usize,  // номер строки (0-7, где 0 - первая строка)
    col: usize,  // номер столбца (0-7, где 0 - первый столбец)
}

// реализация методов для Position
impl Position {
    // создает новую позицию с заданными координатами
    fn new(row: usize, col: usize) -> Self {
        Self { row, col }
    }

    // генерирует случайную позицию на доске
    fn random() -> Self {
        let mut rng = rand::thread_rng();
        Self {
            row: rng.gen_range(0..8),  // случайная строка
            col: rng.gen_range(0..8),  // случайный столбец
        }
    }
}

// структура для представления шахматной доски
struct ChessBoard {
    king: Position,      // позиция короля
    queens: Vec<Position>,  // позиции ферзей
    knights: Vec<Position>, // позиции коней
}

// реализация методов для ChessBoard
impl ChessBoard {
    // создает новую доску с заданным количеством ферзей и коней
    fn new(num_queens: usize, num_knights: usize) -> Self {
        // генерируем случайную позицию для короля
        let king = Position::random();
        
        // создаем вектор для ферзей
        let mut queens = Vec::with_capacity(num_queens);
        while queens.len() < num_queens {
            let pos = Position::random();
            // проверяем, что позиция не занята королем и другими ферзями
            if pos != king && !queens.contains(&pos) {
                queens.push(pos);
            }
        }
        
        // создаем вектор для коней
        let mut knights = Vec::with_capacity(num_knights);
        while knights.len() < num_knights {
            let pos = Position::random();
            // проверяем, что позиция не занята королем, ферзями и другими конями
            if pos != king && !queens.contains(&pos) && !knights.contains(&pos) {
                knights.push(pos);
            }
        }
        
        // возвращаем новую доску
        Self { king, queens, knights }
    }

    // проверяет, атакует ли конь короля
    fn is_knight_attacking(&self, knight: Position) -> bool {
        // вычисляем разницу по строкам и столбцам
        let dx = (knight.row as i32 - self.king.row as i32).abs();
        let dy = (knight.col as i32 - self.king.col as i32).abs();
        // конь атакует, если ходит буквой "Г" (1x2 или 2x1)
        (dx == 1 && dy == 2) || (dx == 2 && dy == 1)
    }

    // проверяет, атакует ли ферзь короля
    fn is_queen_attacking(&self, queen: Position) -> bool {
        // проверяем, находятся ли ферзь и король на одной линии
        if !(queen.row == self.king.row 
            || queen.col == self.king.col 
            || (queen.row as i32 - self.king.row as i32).abs() 
                == (queen.col as i32 - self.king.col as i32).abs()) {
            return false;
        }

        // определяем направление движения от ферзя к королю
        let dx = if self.king.row == queen.row {
            0
        } else if self.king.row > queen.row {
            1
        } else {
            -1
        };
        
        let dy = if self.king.col == queen.col {
            0
        } else if self.king.col > queen.col {
            1
        } else {
            -1
        };

        // проверяем все клетки между ферзем и королем
        let mut x = queen.row as i32 + dx;
        let mut y = queen.col as i32 + dy;
        while x != self.king.row as i32 || y != self.king.col as i32 {
            let current_pos = Position::new(x as usize, y as usize);
            
            // проверяем, не стоит ли на пути другой ферзь
            for q in &self.queens {
                if *q != queen && *q == current_pos {
                    return false;
                }
            }
            
            // проверяем, не стоит ли на пути конь
            for k in &self.knights {
                if *k == current_pos {
                    return false;
                }
            }
            
            // переходим к следующей клетке
            x += dx;
            y += dy;
        }

        true
    }

    // преобразует координаты в шахматную нотацию (например, "e4")
    fn to_chess_notation(pos: Position) -> String {
        // преобразуем номер столбца в букву (a-h)
        let col_char = (b'a' + pos.col as u8) as char;
        // номер строки (1-8)
        format!("{}{}", col_char, 8 - pos.row)
    }
}

// реализация вывода шахматной доски
impl fmt::Display for ChessBoard {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        // символы для фигур
        let king = "K";
        let queen = "Q";
        let knight = "H";
        let empty = "·";
        
        // выводим заголовок с буквами столбцов
        writeln!(f, "  a b c d e f g h")?;

        // выводим каждую строку доски
        for row in 0..8 {
            // выводим номер строки (8-1)
            write!(f, "{} ", 8 - row)?;
            
            // выводим каждую клетку в строке
            for col in 0..8 {
                let pos = Position::new(row, col);
                
                // определяем, какая фигура находится в клетке
                if pos == self.king {
                    write!(f, "{} ", king)?;
                } else if self.queens.contains(&pos) {
                    write!(f, "{} ", queen)?;
                } else if self.knights.contains(&pos) {
                    write!(f, "{} ", knight)?;
                } else {
                    // чередуем цвета пустых клеток
                    if (row + col) % 2 == 0 {
                        write!(f, "\x1b[48;5;7m{}\x1b[0m ", empty)?;
                    } else {
                        write!(f, "{} ", empty)?;
                    }
                }
            }
            
            // выводим номер строки в конце
            writeln!(f, "{}", 8 - row)?;
        }
        
        // выводим нижний заголовок с буквами столбцов
        writeln!(f, "  a b c d e f g h")?;
        Ok(())
    }
}

// функция для поиска возрастающей последовательности с минимальным начальным элементом
fn find_increasing_sequence(matrix: &[Vec<i32>]) -> Vec<i32> {
    let mut result = Vec::new();
    let mut min_start = i32::MAX; // начальное значение больше любого возможного элемента

    // перебираем каждую строку матрицы
    for row in matrix {
        // перебираем каждый элемент строки как начальный элемент последовательности
        for i in 0..row.len() {
            let mut current_seq = vec![row[i]];
            
            // ищем продолжение последовательности
            for j in i + 1..row.len() {
                if row[j] > *current_seq.last().unwrap() {
                    current_seq.push(row[j]);
                } else {
                    break;
                }
            }
            
            // проверяем, подходит ли текущая последовательность
            if current_seq.len() > 1 && current_seq[0] < min_start {
                min_start = current_seq[0];
                result = current_seq;
            }
        }
    }

    result
}

// функция для подсчета уникальных элементов в матрице
fn count_unique_elements(matrix: &[Vec<i32>]) -> HashMap<i32, usize> {
    let mut counts = HashMap::new();
    
    // перебираем все элементы матрицы
    for row in matrix {
        for &val in row {
            // увеличиваем счетчик для текущего элемента
            *counts.entry(val).or_insert(0) += 1;
        }
    }
    
    counts
}

fn main() {
    // задача 1: поиск возрастающей последовательности
    println!("задача 1:");
    let mat = create_matrix(5, 5, 0, 100);
    println!("матрица:");
    print_matrix(&mat);
    let seq = find_increasing_sequence(&mat);
    println!("найденная последовательность:");
    for num in &seq {
        print!("{} ", num);
    }
    println!("\n");

    // задача 2: подсчет уникальных элементов
    println!("задача 2:");
    let mat2 = create_matrix(5, 5, -50, 50);
    println!("матрица:");
    print_matrix(&mat2);
    let counts = count_unique_elements(&mat2);
    println!("количество вхождений каждого элемента:");
    // сортируем элементы для удобного вывода
    let mut sorted_counts: Vec<_> = counts.iter().collect();
    sorted_counts.sort_by_key(|&(k, _)| k);
    for (key, value) in sorted_counts {
        println!("{}: {}", key, value);
    }
    println!();

    // задача 3: шахматная доска
    println!("задача 3:");
    let board = ChessBoard::new(3, 2); // 3 ферзя и 2 коня
    
    // выводим позицию короля с координатами
    println!("позиция короля: {} (строка {}, столбец {})", 
        ChessBoard::to_chess_notation(board.king),
        8 - board.king.row,
        board.king.col);
    
    // выводим позиции ферзей с координатами
    println!("позиции ферзей:");
    for queen in &board.queens {
        print!("{} (строка {}, столбец {}) ", 
            ChessBoard::to_chess_notation(*queen),
            8 - queen.row,
            queen.col);
    }
    println!();
    
    // выводим позиции коней с координатами
    println!("позиции коней:");
    for knight in &board.knights {
        print!("{} (строка {}, столбец {}) ", 
            ChessBoard::to_chess_notation(*knight),
            8 - knight.row,
            knight.col);
    }
    println!();
    
    // выводим ферзей, атакующих короля
    print!("ферзи, атакующие короля:");
    for queen in &board.queens {
        if board.is_queen_attacking(*queen) {
            print!(" {} ", ChessBoard::to_chess_notation(*queen));
        }
    }
    println!();
    
    // выводим коней, атакующих короля
    print!("кони, атакующие короля:");
    for knight in &board.knights {
        if board.is_knight_attacking(*knight) {
            print!(" {} ", ChessBoard::to_chess_notation(*knight));
        }
    }
    println!();
    
    // выводим графическое представление доски
    println!("Шахматная доска:");
    println!("{}", board);
}