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
    //  Tien ich filesystem
    // ═══════════════════════════════════════════════════════

    // Lay danh sach file .txt trong folder (khong de quy)
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

    // Lay danh sach thu muc con truc tiep trong folder
    vector<fs::path> listSubDirs(const fs::path& folder) const {
        vector<fs::path> dirs;
        if (!fs::exists(folder) || !fs::is_directory(folder)) return dirs;
        for (const auto& e : fs::directory_iterator(folder)) {
            if (e.is_directory())
                dirs.push_back(e.path());
        }
        sort(dirs.begin(), dirs.end());
        return dirs;
    }

    // Resolve duong dan: thu file truc tiep, roi them .txt
    fs::path resolvePath(const string& input) const {
        fs::path p(input);
        if (fs::exists(p)) return p;
        fs::path withExt = p;
        withExt.replace_extension(".txt");
        if (fs::exists(withExt)) return withExt;
        return p;
    }

    // Trich xuat so nguyen tu chuoi
    vector<long long> extractIntegers(const string& line) const {
        vector<long long> out;
        static const regex kPat(R"([-]?\d+)");
        sregex_iterator it(line.begin(), line.end(), kPat), end;
        for (; it != end; ++it) out.push_back(stoll(it->str()));
        return out;
    }

    // Parse file testcase: dong 1 = elements, dong 2 = target, dong 3+ = expected
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

    // ═══════════════════════════════════════════════════════
    //  chooseTestFolder
    //  Hien thi cay thu muc testcases (folder goc + cac
    //  thu muc con), cho nguoi dung chon 1 muc, tra ve
    //  danh sach file .txt trong muc do.
    //  Tra ve empty neu huy.
    // ═══════════════════════════════════════════════════════
    vector<fs::path> chooseTestFolder() const {
        const fs::path root = "testcases";

        auto rootFiles = listTxtFiles(root);
        auto subDirs = listSubDirs(root);

        if (rootFiles.empty() && subDirs.empty()) {
            cout << "  [!] Khong co file .txt hoac thu muc nao trong 'testcases'\n";
            cout << "  Working dir: " << fs::current_path().string() << "\n";
            return {};
        }

        cout << "\n  Thu muc 'testcases':\n";
        int idx = 0;

        // File nam ngay trong root
        int rootOption = -1;
        if (!rootFiles.empty()) {
            rootOption = ++idx;
            cout << "    " << rootOption << ". [thu muc goc]"
                << "  (" << rootFiles.size() << " file)\n";
        }

        // Cac thu muc con
        // Luu them so file trong moi thu muc de hien thi
        struct DirEntry { int menuIdx; fs::path path; };
        vector<DirEntry> dirEntries;
        for (const auto& d : subDirs) {
            auto cnt = listTxtFiles(d).size();
            ++idx;
            cout << "    " << idx << ". " << d.filename().string()
                << "/  (" << cnt << " file)\n";
            dirEntries.push_back({ idx, d });
        }

        cout << "  0. Huy\n\n  Chon: ";
        int c; cin >> c;
        if (c == 0) return {};

        // Chon folder goc
        if (c == rootOption && rootOption != -1)
            return rootFiles;

        // Chon thu muc con
        for (const auto& de : dirEntries) {
            if (c == de.menuIdx) {
                auto files = listTxtFiles(de.path);
                if (files.empty()) {
                    cout << "  [!] Khong co file .txt nao trong: "
                        << de.path.filename().string() << "\n";
                    return {};
                }
                cout << "  Da chon: " << de.path.filename().string()
                    << "/  (" << files.size() << " file)\n";
                return files;
            }
        }

        return {};
    }

    // ═══════════════════════════════════════════════════════
    //  selectDataset — chon 1 dataset don (option 1 nguon A)
    // ═══════════════════════════════════════════════════════
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
                << "  3. Nhap duong dan file cu the\n"
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
        else {
            cout << "  1. Tu nhap tay\n"
                << "  2. Nhap duong dan file cu the\n"
                << "  0. Huy\n"
                << "\n  Chon: ";
            int c; cin >> c;
            switch (c) {
            case 0: return false;
            case 1:
                currentDataset_ = inputManual();
                hasDataset_ = true;
                return true;
            case 2: {
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

    // ═══════════════════════════════════════════════════════
    //  Visualize
    // ═══════════════════════════════════════════════════════
    void printSubsetLine(const vector<long long>& subset, long long target) const {
        long long sum = 0;
        for (auto x : subset) { cout << x << " "; sum += x; }
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
            if (!found && sum == target) { found = true; firstFound = subset; }
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
    //  isExpectedMatched — dung chung cho ca option 1 va 2
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
        if (actSet.size() == 1) return expSet.count(*actSet.begin()) > 0;
        return actSet == expSet;
    }

    // ═══════════════════════════════════════════════════════
    //  Option 1: Chay mot thuat toan
    //    Nguon A — 1 dataset don  (nhap tay / file cu the)
    //    Nguon B — Chon thu muc testcases, chay toan bo file
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

        auto& solver = solvers_[choice - 1];

        // Chon nguon du lieu
        printLine();
        cout << "  NGUON DU LIEU\n";
        printLine();
        cout << "  1. Nhap / chon 1 dataset\n"
            << "  2. Chon thu muc testcases (chay tat ca file)\n"
            << "  0. Huy\n"
            << "\n  Chon: ";
        int src; cin >> src;
        if (src == 0) return;

        // ── Nguon A: 1 dataset don ──────────────────────────────────
        if (src == 1) {
            if (!selectDataset()) { pauseScreen(); return; }

            printLine();
            cout << "  Solver : " << solver->name() << "\n"
                << "  Dataset: " << currentDataset_.name()
                << "  n=" << currentDataset_.size()
                << "  target=" << currentDataset_.target() << "\n";
            printLine();

            cout << "  Visualize qua trinh duyet? (y/n): ";
            char viz; cin >> viz;
            if (viz == 'y' || viz == 'Y') {
                const string& sn = solver->name();
                if (sn == "BruteForce" || sn == "Backtracking") {
                    visualizeExhaustive(currentDataset_);
                    printLine();
                }
                else {
                    cout << "  [!] Visualize hien chi ho tro BruteForce/Backtracking.\n";
                    printLine();
                }
            }

            Solution sol = solver->solve(currentDataset_);
            sol.print();
            pauseScreen();
            return;
        }

        // ── Nguon B: Chon thu muc → chay tat ca file ───────────────
        if (src == 2) {
            auto files = chooseTestFolder();
            if (files.empty()) { pauseScreen(); return; }

            cout << "\n";
            printLine('=', 64);
            cout << "  Solver : " << solver->name() << "\n"
                << "  So file: " << files.size() << "\n";
            printLine('=', 64);

            int passCount = 0, failCount = 0, errCount = 0;

            for (const auto& fpath : files) {
                printLine('-', 64);
                cout << "  File: " << fpath.filename().string() << "\n";
                try {
                    auto tc = parseTestCaseFile(fpath);
                    currentDataset_ = tc.dataset;
                    hasDataset_ = true;

                    cout << "  Input : n=" << currentDataset_.size()
                        << ", target=" << currentDataset_.target() << "\n";

                    Solution sol = solver->solve(currentDataset_);
                    sol.print();

                    if (!tc.expectedResults.empty()) {
                        bool pass = isExpectedMatched(sol, tc.expectedResults);
                        cout << "  Kiem tra expected: "
                            << (pass ? "PASS" : "FAIL") << "\n";
                        if (pass) ++passCount; else ++failCount;
                    }
                }
                catch (const exception& ex) {
                    cout << "  [!] Loi: " << ex.what() << "\n";
                    ++errCount;
                }
            }

            printLine('=', 64);
            cout << "  Tong ket: "
                << passCount << " PASS, "
                << failCount << " FAIL, "
                << errCount << " LOI"
                << "  /  " << files.size() << " file\n";
            printLine('=', 64);
            pauseScreen();
        }
    }

    // ═══════════════════════════════════════════════════════
    //  Option 2: Chay testcase mau (so sanh tat ca solver)
    // ═══════════════════════════════════════════════════════
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

        auto files = chooseTestFolder();
        if (files.empty()) { pauseScreen(); return; }

        for (const auto& f : files) runOneTestCaseFile(f);
        pauseScreen();
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