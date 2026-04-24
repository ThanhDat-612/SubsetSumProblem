#pragma once
#include "SubsetSumSolver.h"

// ============================================================
// Class: BruteForceSolver
// Duyệt tất cả 2^n tập con bằng bitmask
// Complexity: O(2^n * n)
// ============================================================
class BruteForceSolver : public SubsetSumSolver {
protected:
    Solution doSolve(const Dataset& ds) override;

public:
    BruteForceSolver() : SubsetSumSolver("BruteForce") {}
    string complexity() const override { return "O(2^n * n)"; }
};