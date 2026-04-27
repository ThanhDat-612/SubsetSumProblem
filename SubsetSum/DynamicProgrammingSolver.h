#pragma once
#include "SubsetSumSolver.h"
#include <unordered_map>

// ============================================================
// Class: DynamicProgrammingSolver
// Toi uu: dung 2 unordered_map rieng biet (current / next)
// thay vi copy vector tam moi vong lap → giam overhead copy
// Dung reserve() de giam re-hash
// Complexity: O(n * |target|)
// ============================================================
class DynamicProgrammingSolver : public SubsetSumSolver {
protected:
    Solution doSolve(const Dataset& ds) override {
        const auto& elems = ds.elements();
        int n = ds.size();
        long long target = ds.target();

        // reached: tap cac tong dat duoc hien tai
        // parentSum[ns] = tong truoc do de trace nguoc
        // parentIdx[ns] = index phan tu da dung
        unordered_map<long long, long long> parentSum;
        unordered_map<long long, int>       parentIdx;
        // Dung unordered_set thay vi unordered_map<bool>
        // De kiem tra membership nhanh hon
        unordered_map<long long, bool> reached;
        reached.reserve(1 << 10); // pre-allocate giam re-hash
        parentSum.reserve(1 << 10);
        parentIdx.reserve(1 << 10);
        reached[0] = true;

        bool found = false;
        for (int i = 0; i < n && !found; ++i) {
            long long e = elems[i];

            // Thu thap cac (newSum, prevSum) can them
            // Dung snapshot cua reached truoc khi sua
            // → tranh iterate-while-modify va tranh copy toan bo map
            vector<pair<long long, long long>> toAdd;
            toAdd.reserve(reached.size());

            for (auto& [s, _] : reached) {
                long long ns = s + e;
                if (!reached.count(ns) && !parentSum.count(ns)) {
                    toAdd.emplace_back(ns, s);
                }
            }
            for (auto& [ns, s] : toAdd) {
                reached[ns] = true;
                parentSum[ns] = s;
                parentIdx[ns] = i;
            }
            if (reached.count(target)) found = true;
        }

        if (!found) return Solution::makeNotFound(target);

        // Trace nguoc lay tap con va index chinh xac
        vector<long long> chosen;
        vector<int>       indices;
        long long cur = target;
        while (cur != 0) {
            long long prev = parentSum[cur];
            int idx = parentIdx[cur];
            chosen.push_back(elems[idx]);
            indices.push_back(idx);
            cur = prev;
        }
        return Solution::makeFound(target, chosen, indices);
    }

public:
    DynamicProgrammingSolver() : SubsetSumSolver("DynamicProgramming") {}
    string complexity() const override { return "O(n * |target|)"; }
};