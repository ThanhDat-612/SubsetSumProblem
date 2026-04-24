#pragma once
#include "SubsetSumSolver.h"

// ============================================================
// Class: BacktrackingSolver
// Backtracking với pruning: dừng sớm khi không thể cải thiện
// Complexity: O(2^n) worst case, thực tế nhanh hơn nhiều
// ============================================================
class BacktrackingSolver : public SubsetSumSolver {
private:
    // Trạng thái tìm kiếm
    struct SearchState {
        const vector<long long>& elems;
        long long target;
        vector<int>       currentIndices;
        vector<long long> currentChosen;
        bool found = false;

        // Tổng tối đa có thể thêm từ idx trở đi (prefix sum suffix)
        vector<long long> suffixSum; // suffixSum[i] = sum(elems[i..n-1])

        SearchState(const vector<long long>& e, long long t)
            : elems(e), target(t) {
            int n = (int)e.size();
            suffixSum.resize(n + 1, 0);
            for (int i = n - 1; i >= 0; --i)
                suffixSum[i] = suffixSum[i + 1] + (e[i] > 0 ? e[i] : 0);
        }
    };

    void backtrack(SearchState& state, int idx, long long currentSum) {
        if (state.found) return;
        if (currentSum == state.target) {
            state.found = true;
            return;
        }
        if (idx == (int)state.elems.size()) return;

        // Pruning: ngay cả khi chọn hết phần tử dương cũng không đủ
        if (currentSum + state.suffixSum[idx] < state.target) return;

        // Nhánh: CHỌN phần tử idx
        state.currentIndices.push_back(idx);
        state.currentChosen.push_back(state.elems[idx]);
        backtrack(state, idx + 1, currentSum + state.elems[idx]);

        if (!state.found) {
            state.currentIndices.pop_back();
            state.currentChosen.pop_back();
            // Nhánh: KHÔNG chọn phần tử idx
            backtrack(state, idx + 1, currentSum);
        }
    }

protected:
    Solution doSolve(const Dataset& ds) override {
        SearchState state(ds.elements(), ds.target());
        backtrack(state, 0, 0);

        if (state.found)
            return Solution::makeFound(ds.target(),
                state.currentChosen,
                state.currentIndices);
        return Solution::makeNotFound(ds.target());
    }

public:
    BacktrackingSolver() : SubsetSumSolver("Backtracking") {}
    string complexity() const override { return "O(2^n) w/ pruning"; }
};