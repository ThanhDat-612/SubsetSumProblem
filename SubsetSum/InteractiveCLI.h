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

    // ═══════════════════════════════════════════════════════
    //  Tien ich hien thi
    // ═══════════════════════════════════════════════════════
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
        cout << "        SUBSET SUM SOLVER\n";
        printLine('=');
    }

    void pauseScreen() const {
        cout << "\nNhan Enter de tiep tuc...";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cin.get();
    }
    bool readIntSafe(int& out) const {
        if (cin >> out) return true;
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return false;
    }

    // ═══════════════════════════════════════════════════════
    //  Nhap / chon dataset  (dung chung cho option 1 va 2)
    // ═══════════════════════════════════════════════════════

    // Nhap thu cong tu ban phim
    Dataset inputManual() const {
        cout << "\n  Nhap cac phan tu (so nguyen, cach nhau dau cach):\n  > ";
        string line;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        getline(cin, line);

        vector<long long> elements;
        istringstream iss(line);
        long long x;
        while (iss >> x) elements.push_back(x);

        if (elements.empty()) {
            cout << "  [!] Khong co phan tu nao.\n";
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

    // Lay danh sach file .txt trong folder
    vector<fs::path> listTxtFiles(const fs::path& folder) const {
        vector<fs::path> files;
        if (!fs::exists(folder) || !fs::is_directory(folder)) return files;
        for (const auto& e : fs::directory_iterator(folder)) {
            if (e.is_regular_file() && e.path().extension() == ".txt")
                files.push_back(e.path());
        }
        sort(files.begin(), files.end());
        return files;
    }

    // Resolve duong dan: thu file truc tiep, roi them .txt, roi folder mac dinh
    fs::path resolvePath(const string& input) const {
        fs::path p(input);
        if (fs::exists(p)) return p;
        fs::path withExt = p; withExt.replace_extension(".txt");
        if (fs::exists(withExt)) return withExt;
        return p; // tra ve nguyen goc, goi se kiem tra lai
    }

    // Parse file testcase: dong 1 = elements, dong 2 = target, dong 3+ = expected
    vector<long long> extractIntegers(const string& line) const {
        vector<long long> out;
        static const regex kPat(R"([-]?\d+)");
        sregex_iterator it(line.begin(), line.end(), kPat), end;
        for (; it != end; ++it) out.push_back(stoll(it->str()));
        return out;
    }

    TestCaseFileData parseTestCaseFile(const fs::path& path) const {
        ifstream fin(path);
        if (!fin.is_open())
            throw runtime_error("Khong mo duoc: " + path.string());

        vector<string> lines;
        string line;
        while (getline(fin, line)) {
            if (!line.empty() && line.back() == '\r') line.pop_back();
            if (!line.empty()) lines.push_back(line);
        }
        if (lines.size() < 2)
            throw runtime_error("File phai co it nhat 2 dong (elements, target).");

        auto elems = extractIntegers(lines[0]);
        auto tgVec = extractIntegers(lines[1]);
        if (elems.empty() || tgVec.empty())
            throw runtime_error("Khong doc duoc elements / target.");

        vector<vector<long long>> expected;
        for (size_t i = 2; i < lines.size(); ++i) {
            auto sub = extractIntegers(lines[i]);
            if (!sub.empty()) expected.push_back(move(sub));
        }
        return { Dataset(elems, tgVec[0], path.filename().string()), expected };
    }

    // Cho nguoi dung chon mot file tu folder testcases
    // Tra ve dataset da chon, hoac empty optional neu huy
    bool chooseFromFolder(const string& folderHint, Dataset& out) {
        string folder = folderHint.empty() ? "testcases" : folderHint;
        auto files = listTxtFiles(folder);

        if (files.empty()) {
            cout << "  [!] Khong co file .txt nao trong: " << folder << "\n";
            cout << "  Working dir: " << fs::current_path().string() << "\n";
            return false;
        }

        cout << "\n  Cac file trong '" << folder << "':\n";
        for (int i = 0; i < (int)files.size(); ++i)
            cout << "    " << (i + 1) << ". " << files[i].filename().string() << "\n";
        cout << "  Chon so (0 = huy): ";
        int idx; cin >> idx;
        if (idx < 1 || idx >(int)files.size()) return false;

        try {
            auto tc = parseTestCaseFile(files[idx - 1]);
            out = tc.dataset;
            cout << "  Da tai: " << out.name()
                << "  n=" << out.size()
                << "  target=" << out.target() << "\n";
            return true;
        }
        catch (const exception& ex) {
            cout << "  [!] Loi doc file: " << ex.what() << "\n";
            return false;
        }
    }

    // ── Menu con: chon nguon du lieu ─────────────────────
    //   1. Tu nhap
    //   2. Chon file tu folder testcases
    //   3. Nhap duong dan file / folder cu the
    //   (neu da co dataset cu thi them lua chon dung lai)
    // Tra ve true neu co dataset hop le
    bool selectDataset() {
        printLine();
        cout << "  CHON NGUON DU LIEU\n";
        printLine();

        if (hasDataset_) {
            cout << "  Dataset hien tai: \"" << currentDataset_.name()
                << "\"  n=" << currentDataset_.size()
                << "  target=" << currentDataset_.target() << "\n\n";
            cout << "  1. Dung lai dataset hien tai\n"
                << "  2. Tu nhap tay\n"
                << "  3. Chon file tu folder testcases\n"
                << "  4. Nhap duong dan file cu the\n"
                << "  0. Huy\n"
                << "\n  Chon: ";
            int c; cin >> c;
            switch (c) {
            case 0: return false;
            case 1: return true;
            case 2:
                currentDataset_ = inputManual();
                hasDataset_ = true;
                return true;
            case 3:
                if (chooseFromFolder("testcases", currentDataset_)) {
                    hasDataset_ = true; return true;
                }
                return false;
            case 4: {
                cout << "  Nhap duong dan file (.txt): ";
                string path; cin.ignore(); getline(cin, path);
                fs::path rp = resolvePath(path);
                if (!fs::exists(rp) || !fs::is_regular_file(rp)) {
                    cout << "  [!] Khong tim thay file: " << path << "\n";
                    cout << "  Working dir: " << fs::current_path().string() << "\n";
                    return false;
                }
                try {
                    auto tc = parseTestCaseFile(rp);
                    currentDataset_ = tc.dataset;
                    hasDataset_ = true;
                    return true;
                }
                catch (const exception& ex) {
                    cout << "  [!] " << ex.what() << "\n";
                    return false;
                }
            }
            default: return false;
            }
        }
        else {
            // Chua co dataset: khong co lua chon "dung lai"
            cout << "  1. Tu nhap tay\n"
                << "  2. Chon file tu folder testcases\n"
                << "  3. Nhap duong dan file cu the\n"
                << "  0. Huy\n"
                << "\n  Chon: ";
            int c; cin >> c;
            switch (c) {
            case 0: return false;
            case 1:
                currentDataset_ = inputManual();
                hasDataset_ = true;
                return true;
            case 2:
                if (chooseFromFolder("testcases", currentDataset_)) {
                    hasDataset_ = true; return true;
                }
                return false;
            case 3: {
                cout << "  Nhap duong dan file (.txt): ";
                string path; cin.ignore(); getline(cin, path);
                fs::path rp = resolvePath(path);
                if (!fs::exists(rp) || !fs::is_regular_file(rp)) {
                    cout << "  [!] Khong tim thay file: " << path << "\n";
                    cout << "  Working dir: " << fs::current_path().string() << "\n";
                    return false;
                }
                try {
                    auto tc = parseTestCaseFile(rp);
                    currentDataset_ = tc.dataset;
                    hasDataset_ = true;
                    return true;
                }
                catch (const exception& ex) {
                    cout << "  [!] " << ex.what() << "\n";
                    return false;
                }
            }
            default: return false;
            }
        }
    }
    void printSubsetLine(const vector<long long>& subset, long long target) const {
        long long sum = 0;
        for (auto x : subset) {
            cout << x << " ";
            sum += x;
        }
        if (subset.empty()) cout << "{}";
        if (sum == target) cout << "[DUNG]";
        cout << "\n";
    }

    void visualizeExhaustive(const Dataset& ds) const {
        const auto& elems = ds.elements();
        const long long target = ds.target();
        const int n = ds.size();
        const int total = (1 << n);

        cout << "\n  [Visualize] Liet ke cac tap con theo thu tu brute-force:\n";
        bool found = false;
        vector<long long> firstFound;
        long long currentSum = 0;

        for (int mask = 1; mask < total; ++mask) {
            vector<long long> subset;
            subset.reserve(n);
            long long sum = 0;
            for (int i = 0; i < n; ++i) {
                if (mask & (1 << i)) {
                    subset.push_back(elems[i]);
                    sum += elems[i];
                }
            }
            currentSum = sum;
            printSubsetLine(subset, target);
            if (!found && sum == target) {
                found = true;
                firstFound = subset;
            }
        }

        cout << "\n  Ket luan visualize: ";
        if (!found) {
            cout << "khong tim thay tap con co tong = " << target << ".\n";
            cout << "  currentSum (tap con cuoi cung da duyet) = " << currentSum << "\n";
            return;
        }

        cout << "tim thay tong = " << target << " voi tap con dau tien: { ";
        for (auto x : firstFound) cout << x << " ";
        cout << "}\n";
    }

    // ═══════════════════════════════════════════════════════
    //  Option 1: Chay mot thuat toan
    // ═══════════════════════════════════════════════════════
    void menuRunSingle() {
        printLine();
        cout << "  [1] CHAY MOT THUAT TOAN\n";
        printLine();

        // Chon solver
        cout << "  Cac thuat toan:\n";
        for (int i = 0; i < (int)solvers_.size(); ++i)
            cout << "    " << (i + 1) << ". "
            << left << setw(20) << solvers_[i]->name()
            << "[" << solvers_[i]->complexity() << "]\n";
        cout << "\n  Chon so (1-" << solvers_.size() << ", 0 = huy): ";
        int choice; cin >> choice;
        if (choice < 1 || choice >(int)solvers_.size()) return;

        // Chon du lieu
        if (!selectDataset()) { pauseScreen(); return; }

        printLine();
        cout << "  Solver : " << solvers_[choice - 1]->name() << "\n"
            << "  Dataset: " << currentDataset_.name()
            << "  n=" << currentDataset_.size()
            << "  target=" << currentDataset_.target() << "\n";
        printLine();
        cout << "  Visualize qua trinh duyet? (y/n): ";
        char viz; cin >> viz;
        if (viz == 'y' || viz == 'Y') {
            const string& solverName = solvers_[choice - 1]->name();
            if (solverName == "BruteForce" || solverName == "Backtracking") {
                visualizeExhaustive(currentDataset_);
                printLine();
            }
            else {
                cout << "  [!] Visualize hien chi ho tro BruteForce/Backtracking.\n";
                printLine();
            }
        }
        Solution sol = solvers_[choice - 1]->solve(currentDataset_);
        sol.print();
        pauseScreen();
    }

    // ═══════════════════════════════════════════════════════
    //  Option 2: Chay testcase mau (voi check expected)
    // ═══════════════════════════════════════════════════════
    string canonicalSubset(const vector<long long>& s) const {
        vector<long long> tmp = s;
        sort(tmp.begin(), tmp.end());
        ostringstream oss;
        for (auto x : tmp) oss << x << ",";
        return oss.str();
    }

    bool isExpectedMatched(const Solution& sol,
        const vector<vector<long long>>& expected) const {
        if (!sol.found()) return false;
        set<string> expSet;
        for (const auto& e : expected) expSet.insert(canonicalSubset(e));
        set<string> actSet;
        for (const auto& a : sol.allElements()) actSet.insert(canonicalSubset(a));
        // Solver 1 nghiem (DP, Greedy): chi can 1 nghiem nam trong expected
        if (actSet.size() == 1) return expSet.count(*actSet.begin()) > 0;
        // Solver nhieu nghiem: phai khop hoan toan
        return actSet == expSet;
    }

    void runOneTestCaseFile(const fs::path& p) {
        printLine('-', 64);
        cout << "  File  : " << p.filename().string() << "\n";
        try {
            auto tc = parseTestCaseFile(p);
            currentDataset_ = tc.dataset;
            hasDataset_ = true;

            cout << "  Input : n=" << currentDataset_.size()
                << ", target=" << currentDataset_.target() << "\n";

            if (!tc.expectedResults.empty()) {
                cout << "  Expected (" << tc.expectedResults.size() << " nghiem):\n";
                for (size_t i = 0; i < tc.expectedResults.size(); ++i) {
                    cout << "    #" << (i + 1) << ": { ";
                    for (auto x : tc.expectedResults[i]) cout << x << " ";
                    cout << "}\n";
                }
            }
            else {
                cout << "  (Khong co expected output trong file)\n";
            }

            analyzer_.clearRecords();
            auto allResults = analyzer_.runAll(currentDataset_);

            if (!tc.expectedResults.empty()) {
                cout << "\n  Kiem tra expected:\n";
                for (const auto& sol : allResults) {
                    bool pass = isExpectedMatched(sol, tc.expectedResults);
                    cout << "    " << left << setw(20) << sol.solverName()
                        << (pass ? "PASS" : "FAIL") << "\n";
                }
            }
        }
        catch (const exception& ex) {
            cout << "  [!] Loi: " << ex.what() << "\n";
        }
    }

    void menuRunTestcases() {
        printLine();
        cout << "  [2] CHAY TESTCASE MAU\n";
        printLine();

        cout << "  1. Chon file tu folder testcases\n"
            << "  2. Nhap duong dan file / folder cu the\n"
            << "  0. Huy\n"
            << "\n  Chon: ";
        int c;
        if (!readIntSafe(c)) {
            cout << "  [!] Vui long nhap so hop le.\n";
            pauseScreen();
            return;
        }


        if (c == 0) return;

        if (c == 1) {
            auto files = listTxtFiles("testcases");
            if (files.empty()) {
                cout << "  [!] Khong co file .txt nao trong 'testcases'\n";
                cout << "  Working dir: " << fs::current_path().string() << "\n";
                pauseScreen(); return;
            }
            cout << "\n  Cac file:\n";
            for (int i = 0; i < (int)files.size(); ++i)
                cout << "    " << (i + 1) << ". " << files[i].filename().string() << "\n";
            cout << "  Chon so (0 = chay tat ca): ";
            int idx; cin >> idx;
            if (idx == 0) {
                for (const auto& f : files) runOneTestCaseFile(f);
            }
            else if (idx >= 1 && idx <= (int)files.size()) {
                runOneTestCaseFile(files[idx - 1]);
            }
            pauseScreen(); return;
        }

        if (c == 2) {
            cout << "  Nhap duong dan (file .txt hoac folder): ";
            string input; cin.ignore(); getline(cin, input);
            fs::path rp = resolvePath(input);

            if (fs::exists(rp) && fs::is_regular_file(rp)) {
                runOneTestCaseFile(rp);
                pauseScreen(); return;
            }
            if (fs::exists(rp) && fs::is_directory(rp)) {
                auto files = listTxtFiles(rp);
                if (files.empty()) {
                    cout << "  [!] Khong co file .txt nao trong folder.\n";
                }
                else {
                    for (const auto& f : files) runOneTestCaseFile(f);
                }
                pauseScreen(); return;
            }
            cout << "  [!] Khong tim thay: " << input << "\n";
            cout << "  Working dir: " << fs::current_path().string() << "\n";
            pauseScreen();
        }
    }

    // ═══════════════════════════════════════════════════════
    //  Menu chinh
    // ═══════════════════════════════════════════════════════
    void printMenu() const {
        printHeader();
        if (hasDataset_) {
            cout << "  Dataset: \"" << currentDataset_.name()
                << "\"  n=" << currentDataset_.size()
                << "  target=" << currentDataset_.target() << "\n";
            printLine('-', 56);
        }
        cout << "\n"
            << "  1.  Chay mot thuat toan\n"
            << "  2.  Chay testcase mau\n"
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
            case 1: clearScreen(); menuRunSingle();    break;
            case 2: clearScreen(); menuRunTestcases(); break;
            case 0: cout << "\n  Ket thuc!\n\n"; break;
            default:
                cout << "  [!] Lua chon khong hop le.\n";
                pauseScreen();
            }
        }
    }
};
