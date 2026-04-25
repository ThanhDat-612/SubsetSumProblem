#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <numeric>
using namespace std;
using ll = long long;
// ============================================================
// Class: Solution
// Lưu kết quả của một lần chạy thuật toán
// ============================================================
class Solution {
public:
    enum class Status { FOUND, NOT_FOUND, NOT_RUN };

private:
    Status status_;
    long long target_;
    vector<long long> chosenElements_;  // Các phần tử trong tập con
    vector<int> chosenIndices_;   // Chỉ số trong mảng gốc
    vector<vector<long long>> allChosenElements_;
    vector<vector<int>> allChosenIndices_;
    long long actualSum_;   
    string solverName_;
    double  elapsedMs_;       // Thời gian chạy (ms)

public:
    // Khởi tạo kết quả rỗng
    Solution()
        : status_(Status::NOT_RUN), target_(0),
        actualSum_(0), elapsedMs_(0.0) {
    }

    // --- Builders ---
    static Solution makeFound(long long,const vector<long long>& ,const vector<int>& , const string& solverName = "");
    
    static Solution makeFoundAll(long long, const vector<vector<long long>>&,
        const vector<vector<int>>&, const string& solverName = "");

    static Solution makeNotFound(long long, const string& solverName = "");
    // --- Getters ---
    bool   found()          const { return status_ == Status::FOUND; }
    Status status()         const { return status_; }
    long long target()      const { return target_; }
    long long actualSum()   const { return actualSum_; }
    double elapsedMs()      const { return elapsedMs_; }
    const string& solverName()        const { return solverName_; }
    const vector<long long>& elements() const { return chosenElements_; }
    const vector<int>& indices()   const { return chosenIndices_; }
    const vector<vector<long long>>& allElements() const { return allChosenElements_; }
    const vector<vector<int>>& allIndices() const { return allChosenIndices_; }
    size_t solutionCount() const { return allChosenIndices_.size(); }
    // --- Setters ---
    void setElapsedMs(double ms) { elapsedMs_ = ms; }
    void setSolverName(const string& n) { solverName_ = n; }

    // --- Validate: kiểm tra kết quả có đúng không ---
    bool validate() const;

    // --- Hiển thị ---
    void print() const;
    // Tóm tắt một dòng (dùng trong bảng so sánh)
    void printSummaryLine() const;
};