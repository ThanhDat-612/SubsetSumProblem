#include "BruteforceSolver.h"

Solution BruteForceSolver::doSolve(const Dataset& ds){
    const auto& elems = ds.elements();
    const int n = ds.size();
    const long long target = ds.target();
    
    vector<vector<long long>> allChosen;
    vector<vector<int>> allIndices;
    vector<long long> currentChosen;
    vector<int> currentIndices;

    function<void(int, long long)> dfs = [&](int idx, long long sum) {
        if (idx == n) {
            if (sum == target) {
                allChosen.push_back(currentChosen);
                allIndices.push_back(currentIndices);
            }
            return;
        }
        currentChosen.push_back(elems[idx]);
        currentIndices.push_back(idx);
        dfs(idx + 1, sum + elems[idx]);
        currentIndices.pop_back();
        currentChosen.pop_back();

        // Bỏ qua phần tử hiện tại
        dfs(idx + 1, sum);
        };

    dfs(0, 0LL);

    if (allChosen.empty())
        return Solution::makeNotFound(target);

    return Solution::makeFoundAll(target, allChosen, allIndices);
}

