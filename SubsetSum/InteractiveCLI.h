#pragma once
#include "Dataset.h"
#include "DataLoader.h"
#include "Solution.h"
#include "SubsetSumSolver.h"
#include "BruteForceSolver.h"
#include "BacktrackingSolver.h"
#include "DynamicProgrammingSolver.h"
#include "MeetInTheMiddleSolver.h"
#include "GreedySolver.h"
#include "PerformanceAnalyzer.h"
#include <memory>
#include <vector>
#include <iostream>
#include <string>
#include <limits>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include <fstream>
#include <filesystem>
#include <regex>
#include <set>

using namespace std;
namespace fs = filesystem;

class InteractiveCLI {
private:
    struct TestCaseFileData {
        Dataset dataset;
        vector<vector<long long>> expectedResults;
    };

    vector<shared_ptr<SubsetSumSolver>> solvers_;
    PerformanceAnalyzer analyzer_;
    Dataset currentDataset_;
    bool hasDataset_ = false;

    void clearScreen() const {
#ifdef _WIN32
        system("cls");
#else
        system("clear");
#endif
    }

    void printLine(char c = '-', int len = 56) const {
        cout << string(len, c) << "\n";
    }

    void printHeader() const {
        printLine('=');
        cout << "  SUBSET SUM SOLVER  \n";
        printLine('=');
    }

    void pauseScreen() const {
        cout << "\nNhan Enter de tiep tuc...";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cin.get();
    }

    Dataset inputDataset() const {
        cout << "\n  Nhap cac phan tu (so nguyen, cach nhau bang dau cach):\n  > ";
        string line;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        getline(cin, line);

        vector<long long> elements;
        istringstream iss(line);
        long long x;
        while (iss >> x) elements.push_back(x);

        if (elements.empty()) {
            cout << "  [!] Khong co phan tu nao, dung dataset cu.\n";
            return currentDataset_;
        }

        long long target;
        cout << "  Nhap target: ";
        cin >> target;

        cout << "  Nhap ten dataset (Enter de bo qua): ";
        cin.ignore();
        string name;
        getline(cin, name);
        if (name.empty()) name = "manual";

        return Dataset(elements, target, name);
    }

    vector<long long> extractIntegers(const string& line) const {
        vector<long long> out;
        static const regex kIntPattern(R"([-]?\d+)");
        sregex_iterator it(line.begin(), line.end(), kIntPattern);
        sregex_iterator endIt;
        for (; it != endIt; ++it) {
            out.push_back(stoll(it->str()));
        }
        return out;
    }

    TestCaseFileData parseTestCaseFile(const fs::path& path) const {
        ifstream fin(path);
        if (!fin.is_open()) {
            throw runtime_error("Khong mo duoc file testcase: " + path.string());
        }

        string line;
        vector<string> nonEmptyLines;
        while (getline(fin, line)) {
            if (!line.empty() && line.back() == '\r') line.pop_back();
            if (!line.empty()) nonEmptyLines.push_back(line);
        }

        if (nonEmptyLines.size() < 3) {
            throw runtime_error("File testcase phai co it nhat 3 dong: input, target, expected result.");
        }

        vector<long long> elements = extractIntegers(nonEmptyLines[0]);
        vector<long long> targetVec = extractIntegers(nonEmptyLines[1]);
        if (elements.empty() || targetVec.empty()) {
            throw runtime_error("Khong doc duoc input/target tu file testcase.");
        }

        vector<vector<long long>> expected;
        for (size_t i = 2; i < nonEmptyLines.size(); ++i) {
            auto subset = extractIntegers(nonEmptyLines[i]);
            if (!subset.empty()) expected.push_back(move(subset));
        }
        if (expected.empty()) {
            throw runtime_error("Testcase bat buoc phai co expected output (cac dong result).");
        }

        string dsName = path.filename().string();
        return { Dataset(elements, targetVec[0], dsName), expected };
    }

    string canonicalSubset(const vector<long long>& subset) const {
        vector<long long> tmp = subset;
        sort(tmp.begin(), tmp.end());
        ostringstream oss;
        for (auto x : tmp) oss << x << ",";
        return oss.str();
    }

    void printExpectedCheck(const Solution& sol,
        const vector<vector<long long>>& expected) const {
        set<string> expectedSet;
        for (const auto& e : expected) expectedSet.insert(canonicalSubset(e));

        set<string> actualSet;
        for (const auto& a : sol.allElements()) actualSet.insert(canonicalSubset(a));

        int matched = 0;
        for (const auto& key : expectedSet) {
            if (actualSet.count(key)) matched++;
        }

        cout << "  [Check] Tim thay " << matched << "/" << expectedSet.size()
            << " nghiem ky vong trong ket qua brute force.\n";
    }

    // Kiem tra PASS/FAIL theo kieu solver:
    // - Solver tim TAT CA nghiem (BruteForce, Backtracking, MITM):
    //     actualSet phai bang dung expectedSet
    // - Solver chi tim MOT nghiem (DP):
    //     chi can nghiem tim duoc nam trong expectedSet la PASS
    bool isExpectedMatched(const Solution& sol,
        const vector<vector<long long>>& expected) const {
        if (!sol.found()) return false;

        set<string> expectedSet;
        for (const auto& e : expected) expectedSet.insert(canonicalSubset(e));

        set<string> actualSet;
        for (const auto& a : sol.allElements()) actualSet.insert(canonicalSubset(a));

        // Neu solver chi tra ve 1 nghiem → kiem tra nghiem do co trong expected
        if (actualSet.size() == 1) {
            return expectedSet.count(*actualSet.begin()) > 0;
        }
        // Neu solver tra ve nhieu nghiem → phai khop hoan toan
        return actualSet == expectedSet;
    }

    // ── Chay mot file testcase cu the ─────────────────────
    void runOneTestCaseFile(const fs::path& p) {
        printLine('-', 64);
        cout << "  File: " << p.filename().string() << "\n";
        try {
            auto tc = parseTestCaseFile(p);
            currentDataset_ = tc.dataset;
            hasDataset_ = true;

            cout << "  Input  n=" << currentDataset_.size()
                << ", target=" << currentDataset_.target() << "\n";

            cout << "  Expected (" << tc.expectedResults.size() << " nghiem):\n";
            for (size_t i = 0; i < tc.expectedResults.size(); ++i) {
                cout << "    #" << (i + 1) << ": { ";
                for (auto x : tc.expectedResults[i]) cout << x << " ";
                cout << "}\n";
            }

            analyzer_.clearRecords();
            auto allResults = analyzer_.runAll(currentDataset_);
            for (const auto& sol : allResults) {
                bool passExpected = isExpectedMatched(sol, tc.expectedResults);
                cout << "  [Expected] " << sol.solverName() << ": "
                    << (passExpected ? "PASS" : "FAIL") << "\n";
            }
        }
        catch (const exception& ex) {
            cout << "  [!] Loi testcase: " << ex.what() << "\n";
        }
    }

    // ── Chuc nang 1 ───────────────────────────────────────
    void menuRunSingle() {
        printLine();
        cout << "  [1] CHON THUAT TOAN\n";
        printLine();

        for (int i = 0; i < (int)solvers_.size(); i++) {
            cout << "  " << (i + 1) << ". "
                << solvers_[i]->name()
                << "  [" << solvers_[i]->complexity() << "]\n";
        }

        cout << "\n  Chon so (1-" << solvers_.size() << "): ";
        int choice;
        cin >> choice;

        if (choice < 1 || choice >(int)solvers_.size()) {
            cout << "  [!] Lua chon khong hop le.\n";
            pauseScreen(); return;
        }

        cout << "\n  Su dung dataset hien tai?";
        if (hasDataset_) {
            cout << " (\"" << currentDataset_.name()
                << "\", n=" << currentDataset_.size()
                << ", target=" << currentDataset_.target() << ")";
        }
        cout << "\n  (y = dung lai / n = nhap moi): ";
        char useOld;
        cin >> useOld;

        if (!hasDataset_ || useOld == 'n' || useOld == 'N') {
            currentDataset_ = inputDataset();
            hasDataset_ = true;
        }

        auto& solver = *solvers_[choice - 1];
        printLine();
        cout << "  Dang chay " << solver.name() << "...\n";
        printLine();

        Solution sol = solver.solve(currentDataset_);
        sol.print();
        pauseScreen();
    }

    // ── Chuc nang 2 ───────────────────────────────────────
    void menuRunCompare() {
        printLine();
        cout << "  [2] SO SANH TAT CA THUAT TOAN\n";
        printLine();

        cout << "  Su dung dataset hien tai?";
        if (hasDataset_) {
            cout << " (\"" << currentDataset_.name()
                << "\", n=" << currentDataset_.size()
                << ", target=" << currentDataset_.target() << ")";
        }
        cout << "\n  (y = dung lai / n = nhap moi): ";
        char useOld;
        cin >> useOld;

        if (!hasDataset_ || useOld == 'n' || useOld == 'N') {
            currentDataset_ = inputDataset();
            hasDataset_ = true;
        }

        analyzer_.clearRecords();
        vector<Solution> results = analyzer_.runAll(currentDataset_);

        double fastest = numeric_limits<double>::max();
        string fastestName;
        for (const auto& sol : results) {
            if (sol.elapsedMs() < fastest) {
                fastest = sol.elapsedMs();
                fastestName = sol.solverName();
            }
        }
        cout << "\n  Nhanh nhat: " << fastestName
            << " (" << fastest << " ms)\n";

        for (const auto& sol : results) {
            if (sol.found()) {
                cout << "\n  " << sol.solverName() << " tim duoc "
                    << sol.solutionCount() << " nghiem.\n";
                for (size_t i = 0; i < sol.allElements().size(); ++i) {
                    cout << "  #" << (i + 1) << ": { ";
                    for (auto x : sol.allElements()[i]) cout << x << " ";
                    cout << "} -> tong = " << currentDataset_.target() << "\n";
                }
                break;
            }
        }

        pauseScreen();
    }

    // ── Chuc nang 4: FIX - Nhan file hoac folder ──────────
    void menuRunSampleTestcases() {
        printLine();
        cout << "  [4] CHAY CAC TESTCASE MAU\n";
        printLine();

        cout << "  Nhap ten FILE (.txt) hoac FOLDER testcase\n"
            << "  Vi du: sample_01   hoac   testcases\n"
            << "  (Enter = folder './testcases'): ";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        string input;
        getline(cin, input);
        if (input.empty()) input = "testcases";

        fs::path inputPath(input);

        // Truong hop 1: la file ton tai
        if (fs::exists(inputPath) && fs::is_regular_file(inputPath)) {
            runOneTestCaseFile(inputPath);
            pauseScreen();
            return;
        }

        // Truong hop 2: them .txt va thu lai
        if (!fs::exists(inputPath)) {
            fs::path withExt = inputPath;
            withExt.replace_extension(".txt");
            if (fs::exists(withExt) && fs::is_regular_file(withExt)) {
                runOneTestCaseFile(withExt);
                pauseScreen();
                return;
            }
        }

        // Truong hop 3: la folder
        if (fs::exists(inputPath) && fs::is_directory(inputPath)) {
            vector<fs::path> files;
            for (const auto& e : fs::directory_iterator(inputPath)) {
                if (e.is_regular_file() && e.path().extension() == ".txt") {
                    files.push_back(e.path());
                }
            }
            sort(files.begin(), files.end());

            if (files.empty()) {
                cout << "  [!] Khong co file .txt nao trong folder: " << input << "\n";
                pauseScreen();
                return;
            }

            cout << "  Tim thay " << files.size() << " file testcase.\n";
            for (const auto& p : files) runOneTestCaseFile(p);
            pauseScreen();
            return;
        }

        // Khong tim thay - in working dir de debug
        cout << "  [!] Khong tim thay: \"" << input << "\"\n";
        cout << "  Working directory hien tai: " << fs::current_path().string() << "\n";
        cout << "  => Hay dat file/folder vao dung thu muc tren.\n";
        pauseScreen();
    }

    // ── Chuc nang 3 ───────────────────────────────────────
    void menuViewData() {
        printLine();
        cout << "  [3] XEM DATA INPUT\n";
        printLine();

        if (!hasDataset_) {
            cout << "  Chua co dataset. Nhap moi:\n";
            currentDataset_ = inputDataset();
            hasDataset_ = true;
        }

        const auto& elems = currentDataset_.elements();
        int n = currentDataset_.size();

        cout << "\n  Ten      : " << currentDataset_.name() << "\n"
            << "  So phan tu: " << n << "\n"
            << "  Target   : " << currentDataset_.target() << "\n";

        long long total = 0, mn = elems[0], mx = elems[0];
        for (auto x : elems) { total += x; mn = min(mn, x); mx = max(mx, x); }
        double avg = (double)total / n;
        int pos = 0, neg = 0, zer = 0;
        for (auto x : elems) { if (x > 0) pos++; else if (x < 0) neg++; else zer++; }

        printLine('-', 40);
        cout << "  Tong     : " << total << "\n"
            << "  Trung binh: " << avg << "\n"
            << "  Min / Max: " << mn << " / " << mx << "\n"
            << "  Duong / Am / Zero: " << pos << " / " << neg << " / " << zer << "\n";

        printLine('-', 40);
        cout << "  Cac phan tu:\n  ";
        for (int i = 0; i < n; i++) {
            cout << "[" << i << "]=" << elems[i];
            if (i < n - 1) cout << "  ";
            if ((i + 1) % 8 == 0 && i < n - 1) cout << "\n  ";
        }
        cout << "\n";

        printLine('-', 40);
        cout << "  Phan bo gia tri (histogram):\n";
        const int bins = 8;
        long long range = mx - mn;
        vector<int> counts(bins, 0);
        for (auto x : elems) {
            int b = (range == 0) ? 0
                : (int)min((long long)(bins - 1), (x - mn) * bins / (range + 1));
            counts[b]++;
        }
        int cmax = *max_element(counts.begin(), counts.end());
        for (int b = 0; b < bins; b++) {
            long long lo = mn + (long long)b * (range + 1) / bins;
            long long hi = mn + (long long)(b + 1) * (range + 1) / bins - 1;
            int bar = cmax > 0 ? counts[b] * 20 / cmax : 0;
            cout << "  " << setw(5) << lo << " - " << setw(5) << hi
                << " | " << string(bar, '#') << " (" << counts[b] << ")\n";
        }

        printLine('-', 40);
        cout << "  Nhap dataset moi? (y/n): ";
        char c; cin >> c;
        if (c == 'y' || c == 'Y') currentDataset_ = inputDataset();

        pauseScreen();
    }

    void printMenu() const {
        printHeader();
        if (hasDataset_) {
            cout << "  Dataset: \"" << currentDataset_.name()
                << "\"  n=" << currentDataset_.size()
                << "  target=" << currentDataset_.target() << "\n";
            printLine('-', 56);
        }
        cout << "\n"
            << "  1.  Chay mot thuat toan tu chon\n"
            << "  2.  Chay va so sanh tat ca thuat toan\n"
            << "  3.  Xem data input\n"
            << "  4.  Chay cac testcase mau (.txt)\n"
            << "\n"
            << "  0.  Thoat\n"
            << "\n"
            << "  Chon: ";
    }

public:
    InteractiveCLI() {
        solvers_.push_back(make_shared<BruteForceSolver>());
        solvers_.push_back(make_shared<BacktrackingSolver>());
        solvers_.push_back(make_shared<DynamicProgrammingSolver>());
        solvers_.push_back(make_shared<MeetInTheMiddleSolver>());
        solvers_.push_back(make_shared<GreedySolver>());
        for (auto& s : solvers_) analyzer_.addSolver(s);
    }

    void run() {
        int choice = -1;
        while (choice != 0) {
            clearScreen();
            printMenu();
            cin >> choice;

            switch (choice) {
            case 1: clearScreen(); menuRunSingle();          break;
            case 2: clearScreen(); menuRunCompare();         break;
            case 3: clearScreen(); menuViewData();           break;
            case 4: clearScreen(); menuRunSampleTestcases(); break;
            case 0: cout << "\n  Ket thuc!\n\n";            break;
            default:
                cout << "  [!] Lua chon khong hop le.\n";
                pauseScreen();
            }
        }
    }
};