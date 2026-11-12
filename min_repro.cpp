#include <fstream>
#include <iostream>
#include <string>
#include "include/dset_mgmt.hpp"
int main() {
    std::fstream f("packagelinks.txt");
    dset::mgr::read_and_print_dataset(f, 6);
    return 0;
}
