#pragma once
#include "SubsetSumSolver.h"
#include <unordered_map>

// ============================================================
// Class: DynamicProgrammingSolver
// DP bảng: dp[s] = true nếu có tập con tổng = s
// Dùng unordered_map để xử lý target âm và giá trị lớn
// Complexity: O(n * |target|)
// ============================================================
class DynamicProgrammingSolver : public SubsetSumSolver {
protected:
    Solution doSolve(const Dataset& ds) override {
        const auto& elems = ds.elements();
        int n = ds.size();
        long long target = ds.target();

        // dp[sum] = index phần tử vừa thêm vào để đạt sum đó (-1 = xuất phát)
        unordered_map<long long, int> dp;
        dp[0] = -1; // tổng 0 từ tập rỗng

        // parent[sum_after] = sum_before (để trace ngược)
        unordered_map<long long, long long> parent;

        for (int i = 0; i < n; ++i) {
            // Duyệt ngược để tránh dùng cùng phần tử 2 lần
            vector<pair<long long, int>> toAdd;
            for (auto& [s, _] : dp) {
                long long ns = s + elems[i];
                if (dp.find(ns) == dp.end()) {
                    toAdd.push_back({ ns, i });
                    parent[ns] = s;
                }
            }
            for (auto& [ns, idx] : toAdd) dp[ns] = idx;

            if (dp.count(target)) break; // tìm được rồi, dừng sớm
        }

        if (!dp.count(target))
            return Solution::makeNotFound(target);

        // Trace ngược để lấy tập con
        vector<long long> chosen;
        vector<int>       indices;
        long long cur = target;
        while (cur != 0) {
            long long prev = parent[cur];
            long long elem = cur - prev;
            chosen.push_back(elem);
            // Tìm chỉ số phần tử trong mảng gốc (chưa dùng)
            for (int i = 0; i < n; ++i) {
                if (elems[i] == elem) {
                    // Kiểm tra không trùng chỉ số
                    bool used = false;
                    for (int j : indices) if (j == i) { used = true; break; }
                    if (!used) { indices.push_back(i); break; }
                }
            }
            cur = prev;
        }
        return Solution::makeFound(target, chosen, indices);
    }

public:
    DynamicProgrammingSolver() : SubsetSumSolver("DynamicProgramming") {}
    string complexity() const override { return "O(n * |target|)"; }
};