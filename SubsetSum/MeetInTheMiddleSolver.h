#pragma once
#include "SubsetSumSolver.h"
#include <algorithm>
#include <unordered_map>

// ===========================================================================
// Class: MeetInTheMiddleSolver
// Chia doi mang, sinh tat ca tong tung nua, ghep cap
// Complexity: O(2^(n/2) * n)
// ===========================================================================
class MeetInTheMiddleSolver : public SubsetSumSolver {
private:
    void buildSubsetSums(const vector<long long>& elems,
        int begin, int end,
        vector<long long>& sums,
        vector<int>& masks) const
    {
        const int len = end - begin;
        const int total = (1ULL << len);
        sums.resize(total);
        masks.resize(total);

        for (int mask = 0; mask < total; ++mask) {
            long long s = 0;
            for (int i = 0; i < len; ++i) {
                if (mask & (1 << i)) {
                    s += elems[begin + i];
                }
            }
            sums[mask] = s;
            masks[mask] = mask;
        }
    }

    static void appendFromMask(const vector<long long>& elems,
        int begin, int len, int mask,
        vector<long long>& chosen,
        vector<int>& indices)
    {
        for (int i = 0; i < len; ++i) {
            if (mask & (1 << i)) {
                indices.push_back(begin + i);
                chosen.push_back(elems[begin + i]);
            }
        }
    }

protected:
    Solution doSolve(const Dataset& ds) override {
        const auto& elems = ds.elements();
        const int n = ds.size();
        const long long target = ds.target();
        const int mid = n / 2;

        vector<long long> leftSums, rightSums;
        vector<int> leftMasks, rightMasks;

        buildSubsetSums(elems, 0, mid, leftSums, leftMasks);
        buildSubsetSums(elems, mid, n, rightSums, rightMasks);

        unordered_map<long long, int> rightMap;
        rightMap.reserve(rightSums.size());
        for (size_t i = 0; i < rightSums.size(); ++i) {
            if (rightMap.find(rightSums[i]) == rightMap.end()) {
                rightMap[rightSums[i]] = rightMasks[i];
            }
        }

        for (size_t i = 0; i < leftSums.size(); ++i) {
            const long long need = target - leftSums[i];
            auto it = rightMap.find(need);
            if (it != rightMap.end()) {
                vector<long long> chosen;
                vector<int> indices;
                chosen.reserve(n);
                indices.reserve(n);

                appendFromMask(elems, 0, mid, leftMasks[i], chosen, indices);
                appendFromMask(elems, mid, n - mid, it->second, chosen, indices);

                return Solution::makeFound(target, chosen, indices);
            }
        }

        return Solution::makeNotFound(target);
    }

public:
    MeetInTheMiddleSolver() : SubsetSumSolver("MeetInTheMiddle") {}
    string complexity() const override { return "O(2^(n/2) * n)"; }
};
