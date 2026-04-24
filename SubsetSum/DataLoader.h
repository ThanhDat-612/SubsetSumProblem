#pragma once
#include "Dataset.h"
#include <fstream>
#include <sstream>
#include <random>
#include <chrono>
#include <algorithm>
#include <stdexcept>

// ============================================================
// Class: DataLoader
// Đọc dataset từ file hoặc stdin
// Format file:
//   Dòng 1: target
//   Dòng 2: n phần tử cách nhau bởi dấu cách
// ============================================================
class DataLoader {
public:
    // Đọc từ file
    static Dataset loadFromFile(const string& , const string&);

    // Đọc từ stdin
    static Dataset loadFromStdin();

    // Đọc từ vector trực tiếp (tiện cho test)
    static Dataset loadFromVector(const vector<long long>& ,long long , const string& );

    // Lưu dataset ra file
    static void saveToFile(const Dataset& , const string& );
};

// ============================================================
// Class: DataGenerator
// Tạo dataset ngẫu nhiên cho mục đích benchmark
// ============================================================
class DataGenerator {
private:
    mt19937_64 rng_;

public:
    // Seed mặc định từ system clock
    DataGenerator();
    explicit DataGenerator(uint64_t seed) : rng_(seed) {}
    // Tạo dataset ngẫu nhiên: n phần tử trong [minVal, maxVal]
    // Target là tổng của k phần tử bất kỳ (đảm bảo có nghiệm)
    Dataset generate(int , long long , long long ,int,const string&);

    // Tạo dataset không có nghiệm (target rất lớn)
    Dataset generateNoSolution(int n, long long minVal, long long maxVal,
        const string& name = "no_solution");
};