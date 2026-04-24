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
    explicit SubsetSumSolver(const string& );
    virtual ~SubsetSumSolver() = default;

    // Template Method: đo thời gian → gọi doSolve → gắn metadata
    Solution solve(const Dataset& ds);
    //Getter
    const string& name() const { return name_; }

    // Thông tin về độ phức tạp (subclass override nếu muốn)
    virtual string complexity() const;

    // In tên + độ phức tạp
    void describe() const;
};