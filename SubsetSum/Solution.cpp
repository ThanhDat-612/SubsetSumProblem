#include "Solution.h"

Solution Solution::makeFound(ll target, const vector<ll>& elements,
    const vector<int>& indices, const string& solverName) {
    Solution s;
    s.status_ = Status::FOUND;
    s.target_ = target;
    s.chosenElements_ = elements;
    s.chosenIndices_ = indices;
    s.actualSum_ = accumulate(elements.begin(), elements.end(), 0LL);
    s.solverName_ = solverName;
    s.allChosenElements_.push_back(elements);
    s.allChosenIndices_.push_back(indices);
    return s;
}

Solution Solution::makeFoundAll(long long target,
    const vector<vector<long long>>& allElements,
    const vector<vector<int>>& allIndices,
    const string& solverName) {
    Solution s;
    s.status_ = allIndices.empty() ? Status::NOT_FOUND : Status::FOUND;
    s.target_ = target;
    s.allChosenElements_ = allElements;
    s.allChosenIndices_ = allIndices;
    s.solverName_ = solverName;

    if (!allElements.empty()) {
        s.chosenElements_ = allElements.front();
        s.chosenIndices_ = allIndices.front();
        s.actualSum_ = accumulate(s.chosenElements_.begin(), s.chosenElements_.end(), 0LL);
    }
    else {
        s.actualSum_ = 0;
    }
    return s;
}

Solution Solution::makeNotFound(long long target, const string& solverName) {
    Solution s;
    s.status_ = Status::NOT_FOUND;
    s.target_ = target;
    s.actualSum_ = 0;
    s.solverName_ = solverName;
    return s;
}

bool Solution::validate() const {
    if (!found()) return true;
    if (allChosenElements_.empty())
        return actualSum_ == target_;
    for (const auto& subset : allChosenElements_) {
        long long sum = accumulate(subset.begin(), subset.end(), 0LL);
        if (sum != target_) return false;
    }
    return actualSum_ == target_;
}

void Solution::print() const {
    cout << " [Solution] Solver: " << solverName_ << "\n";
    if (status_ == Status::NOT_RUN) {
        cout << "  Chua chay.\n---\n";
        return;
    }
    if (!found()) {
        cout << "  Ket qua: KHONG CO nghiem (target=" << target_ << ")\n";
    }
    else {
        cout << "  Ket qua: TIM THAY\n"
            << "  Target : " << target_ << "\n"
            << "  So nghiem: " << solutionCount() << "\n";
        for (size_t k = 0; k < allChosenElements_.size(); ++k) {
            cout << "    #" << (k + 1) << ": { ";
            for (auto x : allChosenElements_[k]) cout << x << " ";
            cout << "} Indices: [ ";
            for (auto idx : allChosenIndices_[k]) cout << idx << " ";
            cout << "]\n";
        }
        cout << "  Sum    : " << actualSum_
            << (validate() ? "  OK" : "  SAI!") << "\n";
    }
    cout << "  Thoi gian: " << elapsedMs_ << " ms\n---\n";
}

void Solution::printSummaryLine() const {
    string status = found() ? "FOUND" : "NOT_FOUND";
    cout << "  " << solverName_
        << "\t" << status
        << "\t" << elapsedMs_ << " ms\n";
}