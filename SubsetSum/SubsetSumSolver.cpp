#include "SubsetSumSolver.h"
SubsetSumSolver::SubsetSumSolver(const string& name) {
	name_ = name;
}

Solution SubsetSumSolver::solve(const Dataset& ds) {
    auto t0 = chrono::high_resolution_clock::now();

    Solution sol = doSolve(ds);

    auto t1 = chrono::high_resolution_clock::now();
    double ms = chrono::duration<double, milli>(t1 - t0).count();

    sol.setElapsedMs(ms);
    sol.setSolverName(name_);
    return sol;
}
string SubsetSumSolver::complexity() const {
    return "O(?)";
}
void SubsetSumSolver::describe()const {
    cout << "[Solver] " << name_ << " | Complexity: " << complexity() << "\n";
}