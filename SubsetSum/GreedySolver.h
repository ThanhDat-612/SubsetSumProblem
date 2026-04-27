#pragma once
#include "SubsetSumSolver.h"
#include <algorithm>

// ============================================================
// Class: GreedySolver
// Chon tham lam: sap xep giam dan, lan luot chon phan tu
// neu tong hien tai + phan tu <= target thi chon.
// KHONG DAM BAO KET QUA DUNG trong moi truong hop.
// Chi dung de minh hoa va so sanh toc do.
// Complexity: O(n log n)
// ============================================================
class GreedySolver : public SubsetSumSolver {
protected:
    Solution doSolve(const Dataset& ds) override {
        const auto& elems = ds.elements();
        int n = ds.size();
        long long target = ds.target();

        // Tao cac cap (gia tri, index goc) de sort ma giu lai index
        vector<pair<long long, int>> sorted(n);
        for (int i = 0; i < n; ++i) sorted[i] = { elems[i], i };

        // Sap xep giam dan theo gia tri tuyet doi
        // → uu tien chon cac phan tu lon truoc (greedy chuan)
        sort(sorted.begin(), sorted.end(),
            [](const pair<long long, int>& a, const pair<long long, int>& b) {
                return a.first > b.first;
            });

        long long currentSum = 0;
        vector<long long> chosen;
        vector<int>       indices;

        for (auto& [val, idx] : sorted) {
            if (currentSum + val <= target) {
                currentSum += val;
                chosen.push_back(val);
                indices.push_back(idx);
                if (currentSum == target) break;
            }
        }

        if (currentSum == target)
            return Solution::makeFound(target, chosen, indices);
        return Solution::makeNotFound(target);
    }

public:
    GreedySolver() : SubsetSumSolver("Greedy") {}
    string complexity() const override { return "O(n log n) - KO dam bao dung"; }
};