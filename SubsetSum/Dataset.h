#pragma once
#include <vector>
#include <string>
#include <stdexcept>
#include <iostream>
using namespace std;
using ll = long long;
class Dataset {
private:
    vector<long long> elements_;
    long long target_;
    string name_;

public:
    Dataset();

    Dataset(const vector<long long>& , long long ,const string& );

    // --- Getters ---
    const vector<long long>& elements() const { return elements_; }
    long long target()                         const { return target_; }
    const string& name()                  const { return name_; }
    int  size()                                const { return (int)elements_.size(); }

    // --- Setters ---
    void setElements(const vector<long long>& v) { elements_ = v; }
    void setTarget(long long t) { target_ = t; }
    void setName(const string& n) { name_ = n; }

    // --- Hiển thị ---
    void print() const;
};