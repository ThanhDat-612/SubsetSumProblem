#include "Dataset.h"

Dataset::Dataset() {
	target_ = 0;
	name_ = "unnamed";
}
Dataset::Dataset(const vector<long long>& elements, long long target,
	const string& name = "unnamed") {
	elements_ = elements;
	target_ = target;
	name_ = name;
	if (elements.empty()) throw invalid_argument("[Dataset] Cannot be empty.");
}

void Dataset::print() const {
	cout << "[Dataset: " << name_ << "] "
		<< "n=" << size() << ", target=" << target_ << "\n"
		<< "  Elements: { ";
	for (auto x : elements_) cout << x << " ";
	cout << "}\n";
}