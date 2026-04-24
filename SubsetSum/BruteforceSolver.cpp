#include "BruteforceSolver.h"

Solution BruteForceSolver::doSolve(const Dataset& ds){
    const auto& elems = ds.elements();
    int n = ds.size();
    long long target = ds.target();

    int total = (1 << n); // chỉ dùng được khi n <= 30
    for (int mask = 0; mask < total; ++mask) {
        long long sum = 0;
        vector<int> indices;
        vector<long long> chosen;

        for (int i = 0; i < n; ++i) {
            if (mask & (1 << i)) {
                sum += elems[i];
                indices.push_back(i);
                chosen.push_back(elems[i]);
            }
        }
        if (sum == target)
            return Solution::makeFound(target, chosen, indices);
    }
    return Solution::makeNotFound(target);
}

