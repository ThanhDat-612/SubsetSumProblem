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
#include <string>
#include <limits>

// ============================================================
// Class: InteractiveCLI
// Giao diện tương tác dòng lệnh với 3 chức năng:
//   1. Chạy một thuật toán tự chọn
//   2. Chạy và so sánh tất cả thuật toán
//   3. Xem data input
// ============================================================
class InteractiveCLI {
private:
    // Danh sách solvers
    std::vector<std::shared_ptr<SubsetSumSolver>> solvers_;
    PerformanceAnalyzer analyzer_;
    Dataset currentDataset_;
    bool hasDataset_ = false;

    // ── Helpers hiển thị ──────────────────────────────────

    void clearScreen() const {
#ifdef _WIN32
        system("cls");
#else
        system("clear");
#endif
    }

    void printLine(char c = '-', int len = 56) const {
        std::cout << std::string(len, c) << "\n";
    }

    void printHeader() const {
        printLine('=');
        std::cout << "  SUBSET SUM SOLVER  \n";
        printLine('=');
    }

    void pauseScreen() const {
        std::cout << "\nNhan Enter de tiep tuc...";
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cin.get();
    }

    // ── Nhập dataset từ bàn phím ───────────────────────────

    Dataset inputDataset() const {
        std::cout << "\n  Nhap cac phan tu (so nguyen, cach nhau bang dau cach):\n  > ";
        std::string line;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::getline(std::cin, line);

        std::vector<long long> elements;
        std::istringstream iss(line);
        long long x;
        while (iss >> x) elements.push_back(x);

        if (elements.empty()) {
            std::cout << "  [!] Khong co phan tu nao, dung dataset cu.\n";
            return currentDataset_;
        }

        long long target;
        std::cout << "  Nhap target: ";
        std::cin >> target;

        std::cout << "  Nhap ten dataset (Enter de bo qua): ";
        std::cin.ignore();
        std::string name;
        std::getline(std::cin, name);
        if (name.empty()) name = "manual";

        return Dataset(elements, target, name);
    }

    // ── Chức năng 1: Chạy một thuật toán ──────────────────

    void menuRunSingle() {
        printLine();
        std::cout << "  [1] CHON THUAT TOAN\n";
        printLine();

        // Hiển thị danh sách
        for (int i = 0; i < (int)solvers_.size(); i++) {
            std::cout << "  " << (i + 1) << ". "
                << solvers_[i]->name()
                << "  [" << solvers_[i]->complexity() << "]\n";
        }

        std::cout << "\n  Chon so (1-" << solvers_.size() << "): ";
        int choice;
        std::cin >> choice;

        if (choice < 1 || choice >(int)solvers_.size()) {
            std::cout << "  [!] Lua chon khong hop le.\n";
            pauseScreen(); return;
        }

        // Nhập dataset
        std::cout << "\n  Su dung dataset hien tai?";
        if (hasDataset_) {
            std::cout << " (\"" << currentDataset_.name()
                << "\", n=" << currentDataset_.size()
                << ", target=" << currentDataset_.target() << ")";
        }
        std::cout << "\n  (y = dung lai / n = nhap moi): ";
        char useOld;
        std::cin >> useOld;

        if (!hasDataset_ || useOld == 'n' || useOld == 'N') {
            currentDataset_ = inputDataset();
            hasDataset_ = true;
        }

        // Chạy
        auto& solver = *solvers_[choice - 1];
        printLine();
        std::cout << "  Dang chay " << solver.name() << "...\n";
        printLine();

        Solution sol = solver.solve(currentDataset_);
        sol.print();
        pauseScreen();
    }

    // ── Chức năng 2: So sánh tất cả thuật toán ────────────

    void menuRunCompare() {
        printLine();
        std::cout << "  [2] SO SANH TAT CA THUAT TOAN\n";
        printLine();

        // Nhập dataset
        std::cout << "  Su dung dataset hien tai?";
        if (hasDataset_) {
            std::cout << " (\"" << currentDataset_.name()
                << "\", n=" << currentDataset_.size()
                << ", target=" << currentDataset_.target() << ")";
        }
        std::cout << "\n  (y = dung lai / n = nhap moi): ";
        char useOld;
        std::cin >> useOld;

        if (!hasDataset_ || useOld == 'n' || useOld == 'N') {
            currentDataset_ = inputDataset();
            hasDataset_ = true;
        }

        // Chạy benchmark qua PerformanceAnalyzer
        analyzer_.clearRecords();
        std::vector<Solution> results = analyzer_.runAll(currentDataset_);

        // Tìm solver nhanh nhất có kết quả
        double fastest = std::numeric_limits<double>::max();
        std::string fastestName;
        for (const auto& sol : results) {
            if (sol.elapsedMs() < fastest) {
                fastest = sol.elapsedMs();
                fastestName = sol.solverName();
            }
        }
        std::cout << "\n  Nhanh nhat: " << fastestName
            << " (" << fastest << " ms)\n";

        // In chi tiết nghiệm đầu tiên tìm được
        for (const auto& sol : results) {
            if (sol.found()) {
                std::cout << "\n  Tap con tim duoc:\n  { ";
                for (auto x : sol.elements()) std::cout << x << " ";
                std::cout << "}  ->  tong = " << sol.actualSum() << "\n";
                break;
            }
        }

        pauseScreen();
    }

    // ── Chức năng 3: Xem data input ───────────────────────

    void menuViewData() {
        printLine();
        std::cout << "  [3] XEM DATA INPUT\n";
        printLine();

        if (!hasDataset_) {
            std::cout << "  Chua co dataset. Nhap moi:\n";
            currentDataset_ = inputDataset();
            hasDataset_ = true;
        }

        const auto& elems = currentDataset_.elements();
        int n = currentDataset_.size();

        // Thông tin chung
        std::cout << "\n  Ten      : " << currentDataset_.name() << "\n"
            << "  So phan tu: " << n << "\n"
            << "  Target   : " << currentDataset_.target() << "\n";

        // Tổng, min, max, avg
        long long total = 0, mn = elems[0], mx = elems[0];
        for (auto x : elems) {
            total += x;
            mn = std::min(mn, x);
            mx = std::max(mx, x);
        }
        double avg = (double)total / n;
        int pos = 0, neg = 0, zer = 0;
        for (auto x : elems) {
            if (x > 0) pos++;
            else if (x < 0) neg++;
            else zer++;
        }

        printLine('-', 40);
        std::cout << "  Tong     : " << total << "\n"
            << "  Trung binh: " << avg << "\n"
            << "  Min / Max: " << mn << " / " << mx << "\n"
            << "  Duong / Am / Zero: "
            << pos << " / " << neg << " / " << zer << "\n";

        // Hiển thị phần tử với index
        printLine('-', 40);
        std::cout << "  Cac phan tu:\n  ";
        for (int i = 0; i < n; i++) {
            std::cout << "[" << i << "]=" << elems[i];
            if (i < n - 1) std::cout << "  ";
            if ((i + 1) % 8 == 0 && i < n - 1) std::cout << "\n  ";
        }
        std::cout << "\n";

        // Histogram đơn giản trên terminal
        printLine('-', 40);
        std::cout << "  Phan bo gia tri (histogram):\n";
        const int bins = 8;
        long long range = mx - mn;
        std::vector<int> counts(bins, 0);
        for (auto x : elems) {
            int b = (range == 0) ? 0
                : (int)std::min((long long)(bins - 1),
                    (x - mn) * bins / (range + 1));
            counts[b]++;
        }
        int cmax = *std::max_element(counts.begin(), counts.end());
        for (int b = 0; b < bins; b++) {
            long long lo = mn + (long long)b * (range + 1) / bins;
            long long hi = mn + (long long)(b + 1) * (range + 1) / bins - 1;
            int bar = cmax > 0 ? counts[b] * 20 / cmax : 0;
            std::cout << "  " << std::setw(5) << lo
                << " - " << std::setw(5) << hi
                << " | " << std::string(bar, '#')
                << " (" << counts[b] << ")\n";
        }

        // Nhập dataset mới nếu muốn
        printLine('-', 40);
        std::cout << "  Nhap dataset moi? (y/n): ";
        char c; std::cin >> c;
        if (c == 'y' || c == 'Y') {
            currentDataset_ = inputDataset();
        }

        pauseScreen();
    }

    // ── Menu chính ────────────────────────────────────────

    void printMenu() const {
        printHeader();
        if (hasDataset_) {
            std::cout << "  Dataset: \"" << currentDataset_.name()
                << "\"  n=" << currentDataset_.size()
                << "  target=" << currentDataset_.target() << "\n";
            printLine('-', 56);
        }
        std::cout << "\n"
            << "  1.  Chay mot thuat toan tu chon\n"
            << "  2.  Chay va so sanh tat ca thuat toan\n"
            << "  3.  Xem data input\n"
            << "\n"
            << "  0.  Thoat\n"
            << "\n"
            << "  Chon: ";
    }

public:
    InteractiveCLI() {
        // Đăng ký tất cả solver
        solvers_.push_back(std::make_shared<BruteForceSolver>());
        solvers_.push_back(std::make_shared<BacktrackingSolver>());
        solvers_.push_back(std::make_shared<DynamicProgrammingSolver>());
        solvers_.push_back(std::make_shared<MeetInTheMiddleSolver>());

        // Đăng ký vào analyzer để dùng ở chức năng 2
        for (auto& s : solvers_) analyzer_.addSolver(s);
    }

    // Khởi chạy vòng lặp giao diện
    void run() {
        int choice = -1;
        while (choice != 0) {
            clearScreen();
            printMenu();
            std::cin >> choice;

            switch (choice) {
            case 1: clearScreen(); menuRunSingle();  break;
            case 2: clearScreen(); menuRunCompare(); break;
            case 3: clearScreen(); menuViewData();   break;
            case 0: std::cout << "\n  Tam biet!\n\n"; break;
            default:
                std::cout << "  [!] Lua chon khong hop le.\n";
                pauseScreen();
            }
        }
    }
};