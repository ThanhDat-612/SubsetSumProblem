#include "PerformanceAnalyzer.h"


void PerformanceAnalyzer::addSolver(shared_ptr<SubsetSumSolver> solver) {
    solvers_.push_back(solver);
}
vector<Solution> PerformanceAnalyzer::runAll(const Dataset& ds) {
    vector<Solution> solutions;
    cout << "\n[Benchmark] Dataset: " << ds.name()
        << " | n=" << ds.size()
        << " | target=" << ds.target() << "\n";
    printSeparator();
    cout << left
        << setw(24) << "Solver"
        << setw(12) << "Result"
        << setw(14) << "Time (ms)"
        << "Valid?\n";
    printSeparator();

    for (auto& solver : solvers_) {
        Solution sol = solver->solve(ds);
        bool valid = sol.validate();

        cout << left
            << setw(24) << sol.solverName()
            << setw(12) << (sol.found() ? "FOUND" : "NOT FOUND")
            << setw(14) << fixed << setprecision(4)
            << sol.elapsedMs()
            << (valid ? "OK" : "FAIL") << "\n";

        records_.push_back({
            sol.solverName(), ds.name(),
            sol.found(), valid,
            sol.elapsedMs(), ds.size()
            });
        solutions.push_back(sol);
    }
    printSeparator();
    return solutions;
}
void PerformanceAnalyzer::runAllDatasets(const vector<Dataset>& datasets) {
    for (const auto& ds : datasets) runAll(ds);
}

void PerformanceAnalyzer::printSummary()const {
    if (records_.empty()) {
        cout << "[PerformanceAnalyzer] Chua co ket qua nao.\n";
        return;
    }
    cout << "\n====== TONG HOP BENCHMARK ======\n";
    // Nhom theo solver
    map<string, vector<double>> times;
    for (const auto& r : records_)
        times[r.solverName].push_back(r.elapsedMs);

    cout << left
        << setw(24) << "Solver"
        << setw(10) << "Runs"
        << setw(14) << "Avg (ms)"
        << setw(14) << "Min (ms)"
        << "Max (ms)\n";
    printSeparator();
    for (auto& [name, ts] : times) {
        double avg = 0, mn = ts[0], mx = ts[0];
        for (double t : ts) { avg += t; mn = min(mn, t); mx = max(mx, t); }
        avg /= ts.size();
        cout << left
            << setw(24) << name
            << setw(10) << ts.size()
            << setw(14) << fixed << setprecision(4) << avg
            << setw(14) << mn
            << mx << "\n";
    }
    printSeparator();
}