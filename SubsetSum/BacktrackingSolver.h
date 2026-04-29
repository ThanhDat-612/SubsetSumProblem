#pragma once
#include "SubsetSumSolver.h"

// ============================================================
// Class: BacktrackingSolver
// Backtracking thuần: duyệt cây quyết định chọn/không chọn
// Complexity: O(2^n)
// ============================================================
class BacktrackingSolver : public SubsetSumSolver {
private:
    struct SearchState {
        const vector<long long>& elems;
        long long target;
        vector<int> currentIndices;
        vector<long long> currentChosen;
        vector<vector<int>> allIndices;
        vector<vector<long long>> allChosen;


        SearchState(const vector<long long>& e, long long t)
            : elems(e), target(t) {
            int n = (int)e.size();
        }
    };

    void backtrack(SearchState& state, int idx, long long currentSum) {
        if (idx == (int)state.elems.size()) {
            if (currentSum == state.target) {
                state.allIndices.push_back(state.currentIndices);
                state.allChosen.push_back(state.currentChosen);
            }
            return;
        }
        if (idx == (int)state.elems.size()) return;

        // Nhánh 1: chọn phần tử idx
        state.currentIndices.push_back(idx);
        state.currentChosen.push_back(state.elems[idx]);
        backtrack(state, idx + 1, currentSum + state.elems[idx]);
        state.currentChosen.pop_back();
        state.currentIndices.pop_back();

        // Nhánh 2: không chọn phần tử idx
        backtrack(state, idx + 1, currentSum);
    }

protected:
    Solution doSolve(const Dataset& ds) override {
        SearchState state(ds.elements(), ds.target());
        backtrack(state, 0, 0);

        if (state.allChosen.empty()) {
            return Solution::makeNotFound(ds.target());
        }
        return Solution::makeFoundAll(ds.target(), state.allChosen, state.allIndices);
    }

public:
    BacktrackingSolver() : SubsetSumSolver("Backtracking") {}
    string complexity() const override { return "O(2^n)"; }
};