#include "Solution.h"

Solution Solution::makeFound(ll target, const vector<ll>& elements,
				const vector<int>& indices, const string& solverName="") {
    Solution s;
    s.status_ = Status::FOUND;
    s.target_ = target;
    s.chosenElements_ = elements;
    s.chosenIndices_ = indices;
    s.actualSum_ = accumulate(elements.begin(), elements.end(), 0LL);
    s.solverName_ = solverName;
    return s;
}
Solution Solution::makeNotFound(long long target, const string& solverName = "") {
    Solution s;
    s.status_ = Status::NOT_FOUND;
    s.target_ = target;
    s.actualSum_ = 0;
    s.solverName_ = solverName;
    return s;
}

bool Solution::validate()const {
    if (!found()) return true; // NOT_FOUND không cần validate
    return actualSum_ == target_;
}

void Solution::print()const {
    cout << " [Solution] Solver: " << solverName_ << "\n";
    if (status_ == Status::NOT_RUN) {
        cout << "  Chua chay.\n└─\n";
        return;
    }
    if (!found()) {
        cout << "  Ket qua: KHONG CO nghiem (target=" << target_ << ")\n";
    }
    else {
        cout << "  Ket qua: TIM THAY\n"
            << "  Target : " << target_ << "\n"
            << "  Subset : { ";
        for (auto x : chosenElements_) cout << x << " ";
        cout << "}\n"
            << "  Indices: [ ";
        for (auto i : chosenIndices_) cout << i << " ";
        cout << "]\n"
            << "  Sum    : " << actualSum_
            << (validate() ? "  ✓" : "  ✗ SAI!") << "\n";
    }
    cout << "  Thoi gian: " << elapsedMs_ << " ms\n└─\n";
}

void Solution::printSummaryLine() const {
    string status = found() ? "FOUND" : "NOT_FOUND";
    cout << "  " << solverName_
        << "\t" << status
        << "\t" << elapsedMs_ << " ms\n";
}