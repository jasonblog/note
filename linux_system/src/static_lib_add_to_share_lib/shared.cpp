#include <iostream>
#include "shared.h"
#include "static.h"

void shared_print()
{
    std::cout << "This is shared_print function" << std::endl;
    static_print();
}
