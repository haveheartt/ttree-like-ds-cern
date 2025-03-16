#include "CustomTTree.h"
#include <arrow/api.h>
#include <arrow/io/file.h>
#include <arrow/ipc/api.h>
#include <arrow/ipc/feather.h>  // Use Feather format for compatibility

CustomTTree::CustomTTree() : numEntries(0) {}

template<typename T>
void CustomTTree::AddColumn(const string& name) {
    if (columns.find(name) != columns.end()) {
        cerr<< "Column " << name << " already exists." << endl;
        return;
    }
    columns[name] = vector<T>();
}

template<typename T>
vector<T>& CustomTTree::GetColumn(const string& name) {
    auto iterator = columns.find(name);
    if (iterator == columns.end()) {
        throw runtime_error("Column " + name + " not found");
    }
    return get<vector<T>>(iterator->second);
}

bool CustomTTree::HasColumn(const string& name) const {
    return columns.find(name) != columns.end();
}

template<typename T>
    void CustomTTree::SetValue(const string& name, const T& value) {
        auto& column = GetColumn<T>(name);
        if (column.size() == numEntries) {
            column.push_back(value);
        } else {
            column[numEntries] = value;
        }
    }

void CustomTTree::Fill() {
    numEntries++;
}

size_t CustomTTree::GetNumEntries() {
    return numEntries;
}

void CustomTTree::PrintStructure() const {
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

void CustomTTree::LoadFromROOT(const string& filename, const string& treename) {
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

arrow::Status CustomTTree::SaveToArrow(const std::string& filename) {
    arrow::MemoryPool* pool = arrow::default_memory_pool();
    vector<shared_ptr<arrow::Array>> arrowArrays;
    vector<shared_ptr<arrow::Field>> schemaFields;

    for (const auto& pair : columns) {
        const string& name = pair.first;

        arrow::Status status = visit([&](const auto& column) -> arrow::Status {
            using T = decay_t<decltype(column)>;
            shared_ptr<arrow::Array> arrowArray;
            shared_ptr<arrow::DataType> arrowType;

            if constexpr (is_same_v<T, vector<int>>) {
                arrow::Int32Builder builder;
                ARROW_RETURN_NOT_OK(builder.AppendValues(column));
                ARROW_RETURN_NOT_OK(builder.Finish(&arrowArray));
                arrowType = arrow::int32();
            } else if constexpr (is_same_v<T, vector<unsigned int>>) {
                arrow::UInt32Builder builder;
                ARROW_RETURN_NOT_OK(builder.AppendValues(column));
                ARROW_RETURN_NOT_OK(builder.Finish(&arrowArray));
                arrowType = arrow::uint32();
            } else if constexpr (is_same_v<T, vector<float>>) {
                arrow::FloatBuilder builder;
                ARROW_RETURN_NOT_OK(builder.AppendValues(column));
                ARROW_RETURN_NOT_OK(builder.Finish(&arrowArray));
                arrowType = arrow::float32();
            } else if constexpr (is_same_v<T, vector<double>>) {
                arrow::DoubleBuilder builder;
                ARROW_RETURN_NOT_OK(builder.AppendValues(column));
                ARROW_RETURN_NOT_OK(builder.Finish(&arrowArray));
                arrowType = arrow::float64();
            } else if constexpr (is_same_v<T, vector<bool>>) {
                arrow::BooleanBuilder builder;
                ARROW_RETURN_NOT_OK(builder.AppendValues(column));
                ARROW_RETURN_NOT_OK(builder.Finish(&arrowArray));
                arrowType = arrow::boolean();
            } else {
                cerr << "Skipping column " << name << " (unsupported type for Arrow)" << endl;
                return arrow::Status::OK();
            }

            schemaFields.push_back(arrow::field(name, arrowType));
            arrowArrays.push_back(arrowArray);
            return arrow::Status::OK();
        }, pair.second);

        if (!status.ok()) {
            return status;
        }
    }

    shared_ptr<arrow::Schema> schema = arrow::schema(schemaFields);
    shared_ptr<arrow::Table> table = arrow::Table::Make(schema, arrowArrays);

    ARROW_ASSIGN_OR_RAISE(auto outfile, arrow::io::FileOutputStream::Open(filename));

    ARROW_RETURN_NOT_OK(arrow::ipc::feather::WriteTable(*table, outfile.get()));

    std::cout << "Successfully saved data to " << filename << std::endl;

    return arrow::Status::OK();
}
