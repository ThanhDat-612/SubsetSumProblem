#pragma once
#include "SubsetSumSolver.h"

// ============================================================
// Class: BruteForceSolver
// Duyệt tất cả 2^n tập con bằng bitmask
// Complexity: O(2^n * n)
// ============================================================
class BruteForceSolver : public SubsetSumSolver {
protected:
    Solution doSolve(const Dataset& ds) override {
        const auto& elems = ds.elements();
        int n = ds.size();
        long long target = ds.target();

        int total = (1 << n); // chỉ dùng được khi n <= 30
        for (int mask = 0; mask < total; ++mask) {
            long long sum = 0;
            vector<int>       indices;
            vector<long long> chosen;

            for (int i = 0; i < n; ++i) {
                if (mask & (1 << i)) {
                    sum += elems[i];
                    indices.push_back(i);
                    chosen.push_back(elems[i]);
                }
            }
            if (sum == target)
                return Solution::makeFound(target, chosen, indices);
        }
        return Solution::makeNotFound(target);
    }

public:
    BruteForceSolver() : SubsetSumSolver("BruteForce") {}
    string complexity() const override { return "O(2^n * n)"; }
};