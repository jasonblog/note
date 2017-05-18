#include <stdio.h>
int main(int argc, char** argv)
{
    double f1 = -1e1; // Valid
    //double f2 = -1e0.2; Invalid
    double f3 = -.3; // Valid
    //double f3 = .; // Invalid
    f1 = 9.;
    f1 = 4e+;
    f1 = 46.e3;
    f1 = 47.0123e123;
    f1 = 1.e-12;
    f1 = 1.2123e-12;
    f1 = .e123 // Invalid;
         f1 = 1.e0001

              return 0;
}
