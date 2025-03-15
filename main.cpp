#include "CustomTTree.h"
#include <iostream>

using namespace std;

int main() {
    
    CustomTTree tree;
    tree.LoadFromROOT("prepared_data.root", "TotemNtuple");
    tree.PrintStructure();

    return 0;
}
