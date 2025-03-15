#ifndef CUSTOM_TTREE_H
#define CUSTOM_TTREE_H

#include <iostream>
#include <variant>
#include <string>
#include <TFile.h>
#include <TTree.h>
#include <TBranch.h>

using namespace std;

using ColumnVariant = variant<
    vector<int>,
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
        CustomTTree() : numEntries(0) {}

    template<typename T>
    void AddColumn(const string& name) {
        if (columns.find(name) != columns.end()) {
            cerr<< "Column " << name << " already exists." << endl;
            return;
        }
        columns[name] = vector<T>();
    }

};

#endif // CUSTOM_TTREE_H
