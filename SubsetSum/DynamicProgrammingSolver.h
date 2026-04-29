#pragma once
#include "SubsetSumSolver.h"
#include <unordered_map>
#include <unordered_set>
#include <limits>
#include <algorithm>
// ============================================================
// Class: DynamicProgrammingSolver
// Luu y: Subset Sum DP la gia da thuc (pseudo-polynomial),
// nhanh khi mien tong (maxSum - minSum) nho.
// Neu mien tong qua lon, fallback sang DP sparse theo tap tong dat duoc.
// ============================================================
class DynamicProgrammingSolver : public SubsetSumSolver {
private:
    static constexpr long long kDenseRangeLimit = 2'000'000; // ~ 2e6 trang thai

    Solution solveDense(const Dataset& ds, long long minSum,
                    long long maxSum, long long target) const {
        const auto& elems = ds.elements();
        int n = ds.size();
        const long long offset = -minSum;
        const size_t width = static_cast<size_t>(maxSum - minSum + 1);

        vector<unsigned char> reached(width, 0);
        vector<long long> parentPrev(width, std::numeric_limits<long long>::max());
        vector<int> parentIdx(width, -1);
        vector<long long> activeSums;
        activeSums.reserve(1024);

        reached[static_cast<size_t>(offset)] = 1; // sum = 0
        activeSums.push_back(0);

        if (target == 0) return Solution::makeFound(target, {}, {});

        for (int i = 0; i < n; ++i) {
            const long long e = elems[i];
            const size_t activeBefore = activeSums.size();
            vector<long long> newSums;
            newSums.reserve(activeBefore);

            for (size_t j = 0; j < activeBefore; ++j) {
                const long long s = activeSums[j];
                const long long ns = s + e;
                const size_t pos = static_cast<size_t>(ns + offset);

                if (!reached[pos]) {
                    reached[pos] = 1;
                    parentPrev[pos] = s;
                    parentIdx[pos] = i;
                    newSums.push_back(ns);
                }
            }

            activeSums.insert(activeSums.end(), newSums.begin(), newSums.end());

            const size_t targetPos = static_cast<size_t>(target + offset);
            if (reached[targetPos]) break;
        }

        const size_t targetPos = static_cast<size_t>(target + offset);
        if (!reached[targetPos]) return Solution::makeNotFound(target);

        vector<long long> chosen;
        vector<int> indices;
        long long cur = target;

        while (cur != 0) {
            const size_t pos = static_cast<size_t>(cur + offset);
            const int idx = parentIdx[pos];
            if (idx < 0) break;

            chosen.push_back(elems[idx]);
            indices.push_back(idx);
            cur = parentPrev[pos];
        }

        reverse(chosen.begin(), chosen.end());
        reverse(indices.begin(), indices.end());
        return Solution::makeFound(target, chosen, indices);
    }

    Solution solveSparse(const Dataset& ds, long long target) const {
        const auto& elems = ds.elements();
        int n = ds.size();

        unordered_set<long long> reached;
        unordered_map<long long, long long> parentPrev;
        unordered_map<long long, int> parentIdx;

        reached.reserve(2048);
        parentPrev.reserve(2048);
        parentIdx.reserve(2048);
        reached.insert(0);

        if (target == 0) return Solution::makeFound(target, {}, {});

        for (int i = 0; i < n; ++i) {
            const long long e = elems[i];
            vector<pair<long long, long long>> toAdd;
            toAdd.reserve(reached.size());

            for (const auto s : reached) {
                const long long ns = s + e;
                if (reached.find(ns) == reached.end() && parentIdx.find(ns) == parentIdx.end()) {
                    toAdd.emplace_back(ns, s);
                }
            }
            for (const auto& [ns, prev] : toAdd) {
                reached.insert(ns);
                parentPrev[ns] = prev;
                parentIdx[ns] = i;
            }
            if (reached.find(target) != reached.end()) break;
        }

        if (reached.find(target) == reached.end()) return Solution::makeNotFound(target);


        vector<long long> chosen;
        vector<int> indices;
        long long cur = target;
        while (cur != 0) {
            const int idx = parentIdx[cur];
            chosen.push_back(elems[idx]);
            indices.push_back(idx);
            cur = parentPrev[cur];
        }
        reverse(chosen.begin(), chosen.end());
        reverse(indices.begin(), indices.end());
        return Solution::makeFound(target, chosen, indices);
    }
protected:
    Solution doSolve(const Dataset& ds) override {
        const auto& elems = ds.elements();
        const long long target = ds.target();

        long long minSum = 0;
        long long maxSum = 0;
        for (const auto x : elems) {
            if (x < 0) minSum += x;
            else maxSum += x;
        }

        if (target < minSum || target > maxSum) {
            return Solution::makeNotFound(target);
        }

        const long long rangeWidth = maxSum - minSum + 1;
        if (rangeWidth > 0 && rangeWidth <= kDenseRangeLimit) {
            return solveDense(ds, minSum, maxSum, target);
        }

        return solveSparse(ds, target);
    }

public:
    DynamicProgrammingSolver() : SubsetSumSolver("DynamicProgramming") {}
    string complexity() const override { return "Pseudo-poly: O(n * rangeSum)"; }
};