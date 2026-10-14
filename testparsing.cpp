// Transpiled from CStar
#include "ext/stdcstar.h"


asm_util::eax add_impl(int b, int a) {
    return a + b;
}

asm_util::eax add(int a, int b) {
    return add_impl(b, a);
}



using namespace std;

int main() {
    System::out.println(add(54, 45));
    return 0;
}

