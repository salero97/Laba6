use std::io::{self, Write, Read};
use std::fs::File;
use std::collections::HashMap;
use rand::Rng;
use std::time::{SystemTime, UNIX_EPOCH};
use std::fmt;

// таблица замены байтов (s-box) для алгоритма AES
const SUBSTITUTION_TABLE: [u8; 256] = [
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
];

// константы для расширения ключа (Rcon)
const ROUND_CONSTANTS: [u8; 11] = [
    0x00, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1B, 0x36
];

#[derive(Debug, Clone)]
struct AesState([[u8; 4]; 4]);

impl AesState {
    fn from_bytes(bytes: &[u8]) -> Self {
        let mut state = [[0; 4]; 4];
        for col in 0..4 {
            for row in 0..4 {
                state[row][col] = bytes[col * 4 + row];
            }
        }
        AesState(state)
    }

    fn to_bytes(&self) -> [u8; 16] {
        let mut bytes = [0; 16];
        for col in 0..4 {
            for row in 0..4 {
                bytes[col * 4 + row] = self.0[row][col];
            }
        }
        bytes
    }

    fn sub_bytes(&mut self) {
        for row in 0..4 {
            for col in 0..4 {
                self.0[row][col] = SUBSTITUTION_TABLE[self.0[row][col] as usize];
            }
        }
    }

    fn shift_rows(&mut self) {
        self.0[1].rotate_left(1);
        self.0[2].rotate_left(2);
        self.0[3].rotate_left(3);
    }

    fn galois_mul(a: u8, b: u8) -> u8 {
        let mut p = 0;
        let mut a = a;
        let mut b = b;
        
        for _ in 0..8 {
            if b & 1 != 0 {
                p ^= a;
            }
            
            let hi_bit_set = a & 0x80 != 0;
            a <<= 1;
            if hi_bit_set {
                a ^= 0x1b;
            }
            b >>= 1;
        }
        p
    }

    fn mix_columns(&mut self) {
        for col in 0..4 {
            let s0 = self.0[0][col];
            let s1 = self.0[1][col];
            let s2 = self.0[2][col];
            let s3 = self.0[3][col];
            
            self.0[0][col] = Self::galois_mul(0x02, s0) ^ Self::galois_mul(0x03, s1) ^ s2 ^ s3;
            self.0[1][col] = s0 ^ Self::galois_mul(0x02, s1) ^ Self::galois_mul(0x03, s2) ^ s3;
            self.0[2][col] = s0 ^ s1 ^ Self::galois_mul(0x02, s2) ^ Self::galois_mul(0x03, s3);
            self.0[3][col] = Self::galois_mul(0x03, s0) ^ s1 ^ s2 ^ Self::galois_mul(0x02, s3);
        }
    }

    fn add_round_key(&mut self, round_key: &[u8]) {
        for row in 0..4 {
            for col in 0..4 {
                self.0[row][col] ^= round_key[row + 4 * col];
            }
        }
    }

    fn print(&self, description: &str) {
        println!("{}:", description);
        for row in 0..4 {
            for col in 0..4 {
                print!("{:02x} ", self.0[row][col]);
            }
            println!();
        }
        println!();
    }
}

struct AesKey {
    expanded: [u8; 176],
}

impl AesKey {
    fn expand_with_log(key: &[u8; 16]) -> (Self, String) {
        let mut expanded = [0; 176];
        expanded[..16].copy_from_slice(key);
        
        let mut log = String::new();
        log.push_str("\nрасширение ключа:\n");
        log.push_str(&format!("раунд 0: {:02x?}\n", &expanded[..16]));
        
        for i in 4..44 {
            let mut temp = [0; 4];
            temp.copy_from_slice(&expanded[(i-1)*4..i*4]);
            
            if i % 4 == 0 {
                temp.rotate_left(1);
                
                for j in 0..4 {
                    temp[j] = SUBSTITUTION_TABLE[temp[j] as usize];
                }
                
                temp[0] ^= ROUND_CONSTANTS[i/4];
                
                if i < 40 {
                    log.push_str(&format!("раунд {}: ", i/4));
                    for j in 0..16 {
                        log.push_str(&format!("{:02x} ", expanded[i*4 + j - (i*4 % 16)]));
                    }
                    log.push_str("\n");
                }
            }
            
            for j in 0..4 {
                expanded[i*4 + j] = expanded[(i-4)*4 + j] ^ temp[j];
            }
        }
        
        (AesKey { expanded }, log)
    }
    
    fn round_key(&self, round: usize) -> &[u8] {
        &self.expanded[round*16..(round+1)*16]
    }
}

fn aes_encrypt_block_with_log(block: &[u8; 16], key: &AesKey) -> ([u8; 16], String) {
    let mut state = AesState::from_bytes(block);
    let mut log = String::new();
    
    state.add_round_key(key.round_key(0));
    state.print("начальное состояние (после AddRoundKey)");
    log.push_str(&format!("начальное состояние (после AddRoundKey):\n{:02x?}\n\n", state.0));
    
    for round in 1..10 {
        state.sub_bytes();
        log.push_str(&format!("раунд {} (после SubBytes):\n{:02x?}\n", round, state.0));
        
        state.shift_rows();
        log.push_str(&format!("раунд {} (после ShiftRows):\n{:02x?}\n", round, state.0));
        
        state.mix_columns();
        log.push_str(&format!("раунд {} (после MixColumns):\n{:02x?}\n", round, state.0));
        
        state.add_round_key(key.round_key(round));
        log.push_str(&format!("раунд {} (после AddRoundKey):\n{:02x?}\n\n", round, state.0));
    }
    
    state.sub_bytes();
    state.shift_rows();
    state.add_round_key(key.round_key(10));
    log.push_str("финальное состояние (после последнего AddRoundKey):\n");
    log.push_str(&format!("{:02x?}\n", state.0));
    
    (state.to_bytes(), log)
}

fn aes_ofb_process_with_log(key: &[u8; 16], iv: &[u8; 16], input: &[u8]) -> (Vec<u8>, String) {
    let (aes_key, key_log) = AesKey::expand_with_log(key);
    let mut feedback = iv.clone();
    let mut output = vec![0; input.len()];
    let mut process_log = String::new();
    
    process_log.push_str(&key_log);
    process_log.push_str("\nначало шифрования...\n");
    
    for (i, chunk) in input.chunks(16).enumerate() {
        let (encrypted, block_log) = aes_encrypt_block_with_log(&feedback, &aes_key);
        process_log.push_str(&block_log);
        feedback = encrypted;
        
        for (j, &byte) in chunk.iter().enumerate() {
            output[i*16 + j] = byte ^ feedback[j];
        }
    }
    
    process_log.push_str("шифрование завершено\n");
    (output, process_log)
}

fn generate_random_key() -> [u8; 16] {
    let mut rng = rand::thread_rng();
    let mut key = [0; 16];
    rng.fill(&mut key);
    key
}

fn generate_random_iv() -> [u8; 16] {
    let mut rng = rand::thread_rng();
    let mut iv = [0; 16];
    rng.fill(&mut iv);
    iv
}

fn load_from_file(filename: &str) -> io::Result<Vec<u8>> {
    let mut file = File::open(filename)?;
    let mut data = Vec::new();
    file.read_to_end(&mut data)?;
    Ok(data)
}

fn save_to_file(filename: &str, data: &[u8]) -> io::Result<()> {
    let mut file = File::create(filename)?;
    file.write_all(data)?;
    Ok(())
}

fn main() -> io::Result<()> {
    println!("==============================================");
    println!("|| программа для шифрования/дешифрования aes ||");
    println!("||          режим работы: ofb               ||");
    println!("||          размер ключа: 128 бит           ||");
    println!("==============================================");
    
    let key = generate_random_key();
    println!("\nсгенерированный ключ (hex): {:02x?}", key);
    
    let iv = generate_random_iv();
    println!("сгенерированный iv (hex): {:02x?}", iv);
    
    println!("\nвыберите источник данных:");
    println!("1 - ввод текста вручную");
    println!("2 - загрузка из файла");
    print!("введите номер выбора: ");
    io::stdout().flush()?;
    
    let mut choice = String::new();
    io::stdin().read_line(&mut choice)?;
    let choice = choice.trim();
    
    let input_data = match choice {
        "1" => {
            print!("\nвведите текст для обработки: ");
            io::stdout().flush()?;
            let mut text = String::new();
            io::stdin().read_line(&mut text)?;
            text.trim().as_bytes().to_vec()
        },
        "2" => {
            print!("\nвведите имя файла: ");
            io::stdout().flush()?;
            let mut filename = String::new();
            io::stdin().read_line(&mut filename)?;
            load_from_file(filename.trim())?
        },
        _ => {
            println!("неверный выбор");
            return Ok(());
        }
    };
    
    let (encrypted, encrypt_log) = aes_ofb_process_with_log(&key, &iv, &input_data);
    println!("{}", encrypt_log);
    println!("зашифрованные данные (hex): {:02x?}", encrypted);
    
    println!("\nначало дешифрования...");
    let (decrypted, decrypt_log) = aes_ofb_process_with_log(&key, &iv, &encrypted);
    println!("{}", decrypt_log);
    println!("дешифрование завершено");
    
    println!("\nрезультат дешифрования:");
    println!("----------------------------------------");
    println!("{}", String::from_utf8_lossy(&decrypted));
    println!("----------------------------------------");
    
    print!("\nсохранить вывод в файл? (y/n): ");
    io::stdout().flush()?;
    let mut save_choice = String::new();
    io::stdin().read_line(&mut save_choice)?;
    
    if save_choice.trim().to_lowercase() == "y" {
        print!("введите имя файла: ");
        io::stdout().flush()?;
        let mut filename = String::new();
        io::stdin().read_line(&mut filename)?;
        
        let encrypted_filename = format!("{}.enc", filename.trim());
        save_to_file(&encrypted_filename, &encrypted)?;
        println!("зашифрованные данные сохранены в {}", encrypted_filename);
        
        let decrypted_filename = format!("{}.dec", filename.trim());
        save_to_file(&decrypted_filename, &decrypted)?;
        println!("расшифрованные данные сохранены в {}", decrypted_filename);
    }
    
    Ok(())
}