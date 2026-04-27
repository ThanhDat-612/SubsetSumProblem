#pragma once
#include "SubsetSumSolver.h"

// ============================================================
// Class: BacktrackingSolver
// Backtracking voi pruning
// Complexity: O(2^n) worst case
// FIX: pruning chinh xac cho ca so am va so duong
// ============================================================
class BacktrackingSolver : public SubsetSumSolver {
private:
    struct SearchState {
        const vector<long long>& elems;
        long long target;
        vector<int>               currentIndices;
        vector<long long>         currentChosen;
        vector<vector<int>>       allIndices;
        vector<vector<long long>> allChosen;

        // suffixMaxPos[i] = tong cac so duong trong elems[i..n-1]
        // suffixMinNeg[i] = tong cac so am trong elems[i..n-1]
        vector<long long> suffixMaxPos;
        vector<long long> suffixMinNeg;

        SearchState(const vector<long long>& e, long long t)
            : elems(e), target(t) {
            int n = (int)e.size();
            suffixMaxPos.resize(n + 1, 0);
            suffixMinNeg.resize(n + 1, 0);
            for (int i = n - 1; i >= 0; --i) {
                suffixMaxPos[i] = suffixMaxPos[i + 1] + (e[i] > 0 ? e[i] : 0);
                suffixMinNeg[i] = suffixMinNeg[i + 1] + (e[i] < 0 ? e[i] : 0);
            }
        }
    };

    void backtrack(SearchState& state, int idx, long long currentSum) {
        if (currentSum == state.target) {
            state.allIndices.push_back(state.currentIndices);
            state.allChosen.push_back(state.currentChosen);
        }
        if (idx == (int)state.elems.size()) return;

        // Pruning: kiem tra khoang co the dat duoc tu idx tro di
        // max co the dat = currentSum + tong so duong con lai
        // min co the dat = currentSum + tong so am con lai
        long long maxReachable = currentSum + state.suffixMaxPos[idx];
        long long minReachable = currentSum + state.suffixMinNeg[idx];
        if (state.target > maxReachable || state.target < minReachable) return;

        // Nhanh: CHON phan tu idx
        state.currentIndices.push_back(idx);
        state.currentChosen.push_back(state.elems[idx]);
        backtrack(state, idx + 1, currentSum + state.elems[idx]);
        state.currentIndices.pop_back();
        state.currentChosen.pop_back();

        // Nhanh: KHONG chon phan tu idx
        backtrack(state, idx + 1, currentSum);
    }

protected:
    Solution doSolve(const Dataset& ds) override {
        SearchState state(ds.elements(), ds.target());
        backtrack(state, 0, 0);

        if (!state.allChosen.empty())
            return Solution::makeFoundAll(ds.target(), state.allChosen, state.allIndices);
        return Solution::makeNotFound(ds.target());
    }

public:
    BacktrackingSolver() : SubsetSumSolver("Backtracking") {}
    string complexity() const override { return "O(2^n) w/ pruning"; }
};