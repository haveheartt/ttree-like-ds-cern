#include "CustomTTree.h"
#include <iostream>

using namespace std;

int main() {
    
    CustomTTree tree;
    tree.LoadFromROOT("prepared_data.root", "TotemNtuple");
//    tree.PrintStructure();
    arrow::Status status = tree.SaveToArrow("hep_data.arrow");
    if (!status.ok()) {
        std::cerr << "Error saving to Arrow: " << status.ToString() << std::endl;
    } else {
        std::cout << "Data successfully saved to hep_data.arrow!" << std::endl;
    }

    return 0;
}
