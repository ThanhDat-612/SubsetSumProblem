#include "DataLoader.h"

// Class DataLoader
Dataset DataLoader::loadFromFile(const string& filepath, const string& name) {
    ifstream fin(filepath);
    if (!fin.is_open())
        throw runtime_error("[DataLoader] Khong mo duoc file: " + filepath);

    long long target;
    fin >> target;

    vector<long long> elements;
    long long x;
    while (fin >> x) elements.push_back(x);

    if (elements.empty())
        throw runtime_error("[DataLoader] File khong co du lieu phan tu.");

    string datasetName = name.empty() ? filepath : name;
    return Dataset(elements, target, datasetName);
}

Dataset DataLoader::loadFromStdin() {
    long long target;
        cout << "Nhap target: ";
        cin >> target;

        int n;
        cout << "Nhap so phan tu n: ";
        cin >> n;

        vector<long long> elements(n);
        cout << "Nhap " << n << " phan tu: ";
        for (auto& e : elements) cin >> e;

        return Dataset(elements, target, "stdin");
}

Dataset DataLoader::loadFromVector(const vector<long long>& elements, long long target,
			const string& name) {
    return Dataset(elements, target, name);
}

void DataLoader::saveToFile(const Dataset& ds, const string& filepath) {
    ofstream fout(filepath);
    if (!fout.is_open())
        throw runtime_error("[DataLoader] Khong the ghi file: " + filepath);
    fout << ds.target() << "\n";
    for (auto x : ds.elements()) fout << x << " ";
    fout << "\n";
    cout << "[DataLoader] Da luu dataset '" << ds.name()
        << "' vao: " << filepath << "\n";
}

// Class Generator

DataGenerator::DataGenerator() {
    auto seed = chrono::steady_clock::now().time_since_epoch().count();
    rng_ = mt19937_64(seed);
}

Dataset DataGenerator::generate(int n, long long minVal, long long maxVal,
	int guaranteedSubsetSize, const string& name) {
    if (n <= 0)     throw invalid_argument("[DataGenerator] n phai > 0");
    if (minVal > maxVal) throw invalid_argument("[DataGenerator] minVal > maxVal");
    if (guaranteedSubsetSize > n)
        guaranteedSubsetSize = n;

    uniform_int_distribution<long long> dist(minVal, maxVal);
    vector<long long> elements(n);
    for (auto& e : elements) e = dist(rng_);

    // Tính target là tổng của guaranteedSubsetSize phần tử đầu
    // → đảm bảo bài toán luôn có nghiệm
    vector<long long> tmp = elements;
    shuffle(tmp.begin(), tmp.end(), rng_);
    long long target = 0;
    for (int i = 0; i < guaranteedSubsetSize; ++i) target += tmp[i];

    return Dataset(elements, target, name);
}

Dataset DataGenerator::generateNoSolution(int n, long long minVal, long long maxVal,
	const string& name) {
    uniform_int_distribution<long long> dist(minVal, maxVal);
    vector<long long> elements(n);
    long long totalSum = 0;
    for (auto& e : elements) {
        e = abs((long long)dist(rng_));
        totalSum += e;
    }
    // target lớn hơn tổng tất cả → chắc chắn vô nghiệm
    long long target = totalSum + 1;
    return Dataset(elements, target, name);
}