// Transpiled from CStar
#include "ext/stdcstar.h"


int demo_impl(int b, int a, int c) {
    return a + b + c;
}

int demo(int a, int b, int c) {
    return demo_impl(b, a, c);
}




int main() {
    int x = demo(10, 20, 30);
    return x;
}

