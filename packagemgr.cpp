// Transpiled from CStar
#include "ext/stdcstar.h"

#include <print>
#include <ext/stdcstar.h>
#include <_defs.h>
#include <string>
#include <cstapi>
#include <fstream>
#include <dset_mgmt.hpp>


/*
*/
#ifndef _CA_PACKAGE_MGR
#define _CA_PACKAGE_MGR
// #include <git2.h>
using namespace std;
using namespace st;
class _CA_PACKAGE_MGR_LIBGIT_HANDLER {
public:
int count;
};
#endif

int main() {
    fstream mgr_links_dataset("packagelinks.txt");
    dset::mgr::read_and_print_dataset(mgr_links_dataset, 6);
    return 0;
}

