#pragma once
#include "SubsetSumSolver.h"
#include <algorithm>
#include <unordered_map>

// ============================================================
// Class: MeetInTheMiddleSolver
// Chia đôi mảng, sinh tất cả tổng từng nửa, ghép cặp
// Complexity: O(2^(n/2) * n)
// ============================================================
class MeetInTheMiddleSolver : public SubsetSumSolver {
private:
    // Sinh toàn bộ (tổng, tập chỉ số) của mảng con [begin, end)
    struct SubsetEntry {
        long long sum;
        vector<int> indices; // chỉ số tuyệt đối trong mảng gốc
    };

    vector<SubsetEntry> generateAll(
        const vector<long long>& elems,
        int begin, int end) const
    {
        int len = end - begin;
        int total = (1 << len);
        vector<SubsetEntry> result;
        result.reserve(total);

        for (int mask = 0; mask < total; ++mask) {
            SubsetEntry entry;
            entry.sum = 0;
            for (int i = 0; i < len; ++i) {
                if (mask & (1 << i)) {
                    entry.sum += elems[begin + i];
                    entry.indices.push_back(begin + i);
                }
            }
            result.push_back(move(entry));
        }
        return result;
    }

protected:
    Solution doSolve(const Dataset& ds) override {
        const auto& elems = ds.elements();
        int n = ds.size();
        long long target = ds.target();
        int mid = n / 2;

        // Sinh tất cả tập con từ 2 nửa
        auto leftEntries = generateAll(elems, 0, mid);
        auto rightEntries = generateAll(elems, mid, n);

        // Build map: danh sach index trong rightEntries
        unordered_map<long long, vector<int>> rightMap;
        rightMap.reserve(rightEntries.size());
        for (int i = 0; i < (int)rightEntries.size(); ++i)
            rightMap[rightEntries[i].sum].push_back(i);

        vector<vector<long long>> allChosen;
        vector<vector<int>> allIndices;

        // Duyệt nửa trái, tìm complement trong nửa phải
        for (const auto& le : leftEntries) {
            long long need = target - le.sum;
            auto it = rightMap.find(need);
            if (it != rightMap.end()) {
                for (int idx : it->second) {
                    const auto& re = rightEntries[idx];
                    vector<int>       indices = le.indices;
                    vector<long long> chosen;
                    for (int i : le.indices) chosen.push_back(elems[i]);
                    for (int i : re.indices) {
                        indices.push_back(i);
                        chosen.push_back(elems[i]);
                    }
                    allIndices.push_back(std::move(indices));
                    allChosen.push_back(std::move(chosen));
                }
            }
        }
        if (!allChosen.empty())
            return Solution::makeFoundAll(target, allChosen, allIndices);
        return Solution::makeNotFound(target);
    }

public:
    MeetInTheMiddleSolver() : SubsetSumSolver("MeetInTheMiddle") {}
    string complexity() const override { return "O(2^(n/2) * n)"; }
};