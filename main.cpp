#include "CustomTTree.h"
#include <iostream>

using namespace std;

int main() {
    
    CustomTTree tree;
    tree.LoadFromROOT("prepared_data.root", "mytree");
     
    return 0;
}
