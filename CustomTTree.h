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

    template<typename T>
    vector<T>& GetColumn(const string& name) {
        auto iterator = columns.find(name);
        if (iterator == columns.end()) {
            throw runtime_error("Column " + name + " not found");
        }
        return get<vector<T>>(iterator->second);
    }

    bool HasColumn(const string& name) const {
        return columns.find(name) != columns.end();
    }

    template<typename T>
        void SetValue(const string& name, const T& value) {
            auto& column = GetColumn<T>(name);
            if (column.size() == numEntries) {
                column.push_back(value);
            } else {
                column[numEntries] = value;
            }
        }

    void Fill() {
        numEntries++;
    }

};

#endif // CUSTOM_TTREE_H
