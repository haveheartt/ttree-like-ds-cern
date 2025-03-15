#ifndef CUSTOM_TTREE_H
#define CUSTOM_TTREE_H

#include <iostream>
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

#endif // CUSTOM_TTREE_H
