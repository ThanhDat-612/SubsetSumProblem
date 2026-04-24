#pragma once
#include "Dataset.h"
#include "DataLoader.h"
#include "Solution.h"
#include "SubsetSumSolver.h"
#include "BruteForceSolver.h"
#include "BacktrackingSolver.h"
#include "DynamicProgrammingSolver.h"
#include "MeetInTheMiddleSolver.h"
#include "PerformanceAnalyzer.h"
#include <memory>
#include <vector>
#include <iostream>

// ============================================================
// Class: ExperimentRunner
// Lớp điều khiển: kết nối tất cả thành phần, chạy thí nghiệm
// ============================================================
class ExperimentRunner {
private:
    PerformanceAnalyzer analyzer_;
    DataGenerator       generator_;

    // Khởi tạo tất cả solvers
    void registerSolvers() {
        analyzer_.addSolver(make_shared<BruteForceSolver>());
        analyzer_.addSolver(make_shared<BacktrackingSolver>());
        analyzer_.addSolver(make_shared<DynamicProgrammingSolver>());
        analyzer_.addSolver(make_shared<MeetInTheMiddleSolver>());
    }

    void printBanner() const {
        cout << R"(
         SUBSET SUM SOLVER - MEET IN THE MIDDLE       
      BruteForce | Backtracking | DP | MITM           
)" << "\n";
    }

    // Chạy thí nghiệm 1: dataset tĩnh do người dùng định nghĩa
    void runStaticExperiments() {
        cout << "\n─── Thi nghiem 1: Dataset tinh ───\n";

        vector<Dataset> datasets = {
            DataLoader::loadFromVector({3, 1, 4, 1, 5, 9, 2, 6}, 10,   "small_found"),
            DataLoader::loadFromVector({7, 3, 2, 5, 8, 4, 1, 6}, 100,  "small_notfound"),
            DataLoader::loadFromVector({-5, -3, 2, 7, -1, 4, 6}, 3,    "with_negatives"),
            DataLoader::loadFromVector({1, 2, 3, 4, 5, 6, 7, 8,
                                        9, 10, 11, 12, 13, 14, 15, 16}, 57, "medium"),
        };

        analyzer_.runAllDatasets(datasets);
    }

    // Chạy thí nghiệm 2: benchmark tăng dần kích thước
    void runScalingBenchmark() {
        cout << "\n─── Thi nghiem 2: Scaling Benchmark (n tang dan) ───\n";

        // BruteForce / Backtracking chỉ dùng được n <= 25
        vector<int> sizes = { 8, 12, 16, 20, 24 };

        vector<Dataset> datasets;
        for (int n : sizes) {
            string name = "rand_n" + to_string(n);
            datasets.push_back(generator_.generate(n, 1, 100, 4, name));
        }

        analyzer_.runAllDatasets(datasets);
    }

    // Chạy thí nghiệm 3: trường hợp vô nghiệm
    void runNoSolutionBenchmark() {
        cout << "\n─── Thi nghiem 3: Vo nghiem (worst case) ───\n";

        vector<Dataset> datasets;
        for (int n : {8, 12, 16, 20}) {
            string name = "nosol_n" + to_string(n);
            datasets.push_back(generator_.generateNoSolution(n, 1, 50, name));
        }

        analyzer_.runAllDatasets(datasets);
    }

public:
    ExperimentRunner() : generator_(42) { // seed cố định để kết quả tái lập
        registerSolvers();
    }

    // Chạy toàn bộ thí nghiệm
    void runAll() {
        printBanner();
        runStaticExperiments();
        runScalingBenchmark();
        runNoSolutionBenchmark();
        analyzer_.printSummary();
    }

    // Chạy một dataset tuỳ chỉnh từ ngoài
    vector<Solution> runCustom(const Dataset& ds) {
        return analyzer_.runAll(ds);
    }

    // Chạy solver đơn lẻ trên dataset và in kết quả đầy đủ
    void runSingleSolver(SubsetSumSolver& solver, const Dataset& ds) {
        cout << "\n[ExperimentRunner] Chay don: ";
        solver.describe();
        ds.print();
        Solution sol = solver.solve(ds);
        sol.print();
    }
};