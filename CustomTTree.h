#ifndef CUSTOM_TTREE_H
#define CUSTOM_TTREE_H

#include <iostream>
#include <variant>
#include <string>
#include <functional>
#include <TFile.h>
#include <TTree.h>
#include <TBranch.h>

using namespace std;

using ColumnVariant = variant<
    vector<int>,
    vector<unsigned int>,
    vector<float>,
    vector<double>,
    vector<bool>,
    vector<string>
>;

class CustomTTree {
private:
    map<string, ColumnVariant> columns;
    size_t numEntries;

public:
    CustomTTree();

    template<typename T>
    void AddColumn(const string& name);

    template<typename T>
    vector<T>& GetColumn(const string& name);

    bool HasColumn(const string& name) const;

    template<typename T>
    void SetValue(const string& name, const T& value);

    void Fill();

    size_t GetNumEntries();

    void PrintStructure() const;

    void LoadFromROOT(const string& filename, const string& treename);
};

#endif // CUSTOM_TTREE_H
