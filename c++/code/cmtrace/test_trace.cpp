#include "cm/trace.hpp"

void foo()
{
    CM_TRACE("foo()");
}

void bar()
{
    CM_TRACE("bar()");
    foo();
}

int main()
{
    CM_TRACE_FILE("trace.log");
    CM_TRACE("main()");
    foo();
    bar();
    return 0;
}
