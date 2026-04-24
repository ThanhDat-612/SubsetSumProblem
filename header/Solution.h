#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <numeric>
using namespace std;
// ============================================================
// Class: Solution
// Lưu kết quả của một lần chạy thuật toán
// ============================================================
class Solution {
public:
    enum class Status { FOUND, NOT_FOUND, NOT_RUN };

private:
    Status status_;
    long long target_;
    vector<long long> chosenElements_;  // Các phần tử trong tập con
    vector<int>       chosenIndices_;   // Chỉ số trong mảng gốc
    long long              actualSum_;
    string            solverName_;
    double                 elapsedMs_;       // Thời gian chạy (ms)

public:
    // Khởi tạo kết quả rỗng
    Solution()
        : status_(Status::NOT_RUN), target_(0),
          actualSum_(0), elapsedMs_(0.0) {}

    // --- Builders ---
    static Solution makeFound(long long target,
                              const vector<long long>& elements,
                              const vector<int>& indices,
                              const string& solverName = "") {
        Solution s;
        s.status_         = Status::FOUND;
        s.target_         = target;
        s.chosenElements_ = elements;
        s.chosenIndices_  = indices;
        s.actualSum_      = accumulate(elements.begin(), elements.end(), 0LL);
        s.solverName_     = solverName;
        return s;
    }

    static Solution makeNotFound(long long target,
                                 const string& solverName = "") {
        Solution s;
        s.status_     = Status::NOT_FOUND;
        s.target_     = target;
        s.actualSum_  = 0;
        s.solverName_ = solverName;
        return s;
    }

    // --- Getters ---
    bool   found()          const { return status_ == Status::FOUND; }
    Status status()         const { return status_;          }
    long long target()      const { return target_;          }
    long long actualSum()   const { return actualSum_;       }
    double elapsedMs()      const { return elapsedMs_;       }
    const string& solverName()        const { return solverName_;       }
    const vector<long long>& elements() const { return chosenElements_; }
    const vector<int>&    indices()   const { return chosenIndices_;   }

    // --- Setters ---
    void setElapsedMs(double ms)             { elapsedMs_  = ms;   }
    void setSolverName(const string& n) { solverName_ = n;    }

    // --- Validate: kiểm tra kết quả có đúng không ---
    bool validate() const {
        if (!found()) return true; // NOT_FOUND không cần validate
        return actualSum_ == target_;
    }

    // --- Hiển thị ---
    void print() const {
        cout << "┌─ [Solution] Solver: " << solverName_ << "\n";
        if (status_ == Status::NOT_RUN) {
            cout << "│  Chua chay.\n└─\n";
            return;
        }
        if (!found()) {
            cout << "│  Ket qua: KHONG CO nghiem (target=" << target_ << ")\n";
        } else {
            cout << "│  Ket qua: TIM THAY\n"
                      << "│  Target : " << target_ << "\n"
                      << "│  Subset : { ";
            for (auto x : chosenElements_) cout << x << " ";
            cout << "}\n"
                      << "│  Indices: [ ";
            for (auto i : chosenIndices_) cout << i << " ";
            cout << "]\n"
                      << "│  Sum    : " << actualSum_
                      << (validate() ? "  ✓" : "  ✗ SAI!") << "\n";
        }
        cout << "│  Thoi gian: " << elapsedMs_ << " ms\n└─\n";
    }

    // Tóm tắt một dòng (dùng trong bảng so sánh)
    void printSummaryLine() const {
        string status = found() ? "FOUND" : "NOT_FOUND";
        cout << "  " << solverName_
                  << "\t" << status
                  << "\t" << elapsedMs_ << " ms\n";
    }
};