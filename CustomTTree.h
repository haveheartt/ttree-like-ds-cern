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

    size_t GetNumEntries() {
        return numEntries;
    }

    void PrintStructure() const {
        cout << "===== Custom TTree Structure =====" << endl;
        cout << "Number of entries> " << numEntries << endl;
        cout << "Columns:" << endl;

        for (const auto& pair : columns) {
            cout << " " << pair.first << " (";
            
            visit([](const auto& column) {
                using T = decay_t<decltype(column)>;
                if constexpr (is_same_v<T, vector<int>>) cout << "int";
                else if constexpr (is_same_v<T, vector<unsigned int>>) cout << "unsigned int";
                else if constexpr (is_same_v<T, vector<float>>) cout << "float";
                else if constexpr (is_same_v<T, vector<double>>) cout << "double";
                else if constexpr (is_same_v<T, vector<bool>>) cout << "bool";
                else if constexpr (is_same_v<T, vector<string>>) cout << "string";
                else cout << "unknown";
            }, pair.second);
            
            cout << ")" << endl;
        }

        cout << "================================" << endl;
    }

    void LoadFromROOT(const string& filename, const string& treename) {
        TFile file(filename.c_str(), "READ");
        if (!file.IsOpen()) {
            cerr << "Error: Could not open file " << filename << endl;
            return;
        }

        TTree* tree = (TTree*)file.Get(treename.c_str());
        if (!tree) {
            cerr << "Error: Could not retrieve TTree " << treename << " from file " << filename << endl;
            file.Close();
            return;
        }

        //preallocate memory because of the array awkwardness nature of ROOT files
        for (auto& pair : columns) {
            visit([tree](auto& column) { column.reserve(tree->GetEntries()); }, pair.second);
        }

        vector<pair<void*, function<void(void*)>>> branchData;

        TObjArray* branches = tree->GetListOfBranches();
        for (int i = 0; i < branches->GetEntries(); i++) {
            TBranch* branch = (TBranch*)branches->At(i);
            string name = branch->GetName();
            string type = branch->GetClassName();

            if (type.empty()) {
                string title = branch->GetTitle();

                 if (title.find("/I") != string::npos) {
                    AddColumn<int>(name);
                    int* value = new int;
                    tree->SetBranchAddress(name.c_str(), value);
                    branchData.emplace_back(value, [this, name](void* val) {
                        SetValue(name, *static_cast<int*>(val));
                    });
                } else if (title.find("/i") != string::npos) {
                    AddColumn<unsigned int>(name);
                    unsigned int* value = new unsigned int;
                    tree->SetBranchAddress(name.c_str(), value);
                    branchData.emplace_back(value, [this, name](void* val) {
                        SetValue(name, *static_cast<unsigned int*>(val));
                    });
                } else if (title.find("/F") != string::npos) {
                    AddColumn<float>(name);
                    float* value = new float;
                    tree->SetBranchAddress(name.c_str(), value);
                    branchData.emplace_back(value, [this, name](void* val) {
                        SetValue(name, *static_cast<float*>(val));
                    });
                } else if (title.find("/D") != string::npos) {
                    AddColumn<double>(name);
                    double* value = new double;
                    tree->SetBranchAddress(name.c_str(), value);
                    branchData.emplace_back(value, [this, name](void* val) {
                        SetValue(name, *static_cast<double*>(val));
                    });
                } else if (title.find("/O") != string::npos) {
                    AddColumn<bool>(name);
                    bool* value = new bool;
                    tree->SetBranchAddress(name.c_str(), value);
                    branchData.emplace_back(value, [this, name](void* val) {
                        SetValue(name, *static_cast<bool*>(val));
                    }); 
                } else {
                    cerr << "Warning: Unknown type for branch " << name << " (title: " << title << "), skipping" << endl;
                }
            }
        }

        for (Long64_t j = 0; j < tree->GetEntries(); j++) {
            tree->GetEntry(j);

            for (auto& [value, setValue] : branchData) {
                setValue(value);
            }

            Fill();
        }

        for (auto& [value, _] : branchData) {
            delete static_cast<int*>(value);
        }

        file.Close();
        cout << "Loaded " << numEntries << " entries from " << filename << endl;
    }

};

#endif // CUSTOM_TTREE_H
