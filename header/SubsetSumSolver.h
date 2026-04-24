#pragma once
#include "Dataset.h"
#include "Solution.h"
#include <string>
#include <chrono>

// ============================================================
// Abstract Class: SubsetSumSolver
// Giao diện chung cho tất cả thuật toán giải Subset Sum
// ============================================================
class SubsetSumSolver {
protected:
    string name_;

    // Hook để subclass ghi đè: logic giải thuật
    virtual Solution doSolve(const Dataset& ds) = 0;

public:
    explicit SubsetSumSolver(const string& name) : name_(name) {}
    virtual ~SubsetSumSolver() = default;

    // Template Method: đo thời gian → gọi doSolve → gắn metadata
    Solution solve(const Dataset& ds) {
        auto t0 = chrono::high_resolution_clock::now();

        Solution sol = doSolve(ds);

        auto t1 = chrono::high_resolution_clock::now();
        double ms = chrono::duration<double, milli>(t1 - t0).count();

        sol.setElapsedMs(ms);
        sol.setSolverName(name_);
        return sol;
    }

    const string& name() const { return name_; }

    // Thông tin về độ phức tạp (subclass override nếu muốn)
    virtual string complexity() const { return "O(?)"; }

    // In tên + độ phức tạp
    void describe() const {
        cout << "[Solver] " << name_ << " | Complexity: " << complexity() << "\n";
    }
};