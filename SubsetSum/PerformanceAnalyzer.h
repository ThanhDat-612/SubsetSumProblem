#pragma once
#include "SubsetSumSolver.h"
#include "Solution.h"
#include "Dataset.h"
#include <vector>
#include <memory>
#include <iomanip>
#include <map>

// ============================================================
// Struct: BenchmarkRecord
// Lưu kết quả benchmark của một solver trên một dataset
// ============================================================
struct BenchmarkRecord {
    string solverName;
    string datasetName;
    bool        found;
    bool        valid;
    double      elapsedMs;
    int         datasetSize;
};

// ============================================================
// Class: PerformanceAnalyzer
// Chạy và so sánh nhiều solver trên nhiều dataset
// ============================================================
class PerformanceAnalyzer {
private:
    vector<shared_ptr<SubsetSumSolver>> solvers_;
    vector<BenchmarkRecord> records_;

    void printSeparator(int width = 72) const {
        cout << string(width, '-') << "\n";
    }

public:
    // Thêm solver vào danh sách benchmark
    void addSolver(shared_ptr<SubsetSumSolver> solver);
    // Chạy tất cả solver trên một dataset
    vector<Solution> runAll(const Dataset& ds);
    // Chạy trên nhiều dataset
    void runAllDatasets(const vector<Dataset>& datasets);
    // In bảng tóm tắt tổng hợp
    void printSummary() const;

    // Lấy toàn bộ records (để xử lý ngoài)
    const vector<BenchmarkRecord>& records() const { return records_; }

    void clearRecords() { records_.clear(); }
};