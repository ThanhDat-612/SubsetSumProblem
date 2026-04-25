#pragma once
#include "SubsetSumSolver.h"
#include<functional>
// ============================================================
// Class: BruteForceSolver
// Duyệt tất cả 2^n tập con bằng DFS/backtracking
// Complexity: O(2^n * n)
// ============================================================
class BruteForceSolver : public SubsetSumSolver {
protected:
    Solution doSolve(const Dataset& ds) override;

public:
    BruteForceSolver() : SubsetSumSolver("BruteForce") {}
    string complexity() const override { return "O(2^n * n)"; }
};