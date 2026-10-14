// Transpiled from CStar
#include "ext/stdcstar.h"

#include <_defs.h>


int main() {
    
    using namespace std;
    
    cstdecl __cstdecl;
    
    // using eax main() {} not a good practice
        asm_util::eax something;
        something = 42;
        cout << something << endl;
        return 0;
}

