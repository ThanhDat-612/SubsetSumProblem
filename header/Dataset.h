#pragma once
#include <vector>
#include <string>
#include <stdexcept>
#include <iostream>
using namespace std;
class Dataset {
private:
    vector<long long> elements_;
    long long target_;
    string name_;  
 
public:
    Dataset() : target_(0), name_("unnamed") {}
 
    Dataset(const vector<long long>& elements, long long target,
            const string& name = "unnamed")
        : elements_(elements), target_(target), name_(name) {
        if (elements.empty())
            throw invalid_argument("[Dataset] Mang phan tu khong duoc rong.");
    }
 
    // --- Getters ---
    const vector<long long>& elements() const { return elements_; }
    long long target()                         const { return target_;   }
    const string& name()                  const { return name_;     }
    int  size()                                const { return (int)elements_.size(); }
 
    // --- Setters ---
    void setElements(const vector<long long>& v) { elements_ = v; }
    void setTarget(long long t)                        { target_ = t;   }
    void setName(const string& n)                 { name_ = n;     }
 
    // --- Hiển thị ---
    void print() const {
        cout << "[Dataset: " << name_ << "] "
                  << "n=" << size() << ", target=" << target_ << "\n"
                  << "  Elements: { ";
        for (auto x : elements_) cout << x << " ";
        cout << "}\n";
    }
};