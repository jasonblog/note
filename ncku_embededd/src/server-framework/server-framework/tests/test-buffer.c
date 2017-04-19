#include "buffer.h"

int main(void)
{
    void *buf = Buffer.create(0);

    Buffer.destroy(buf);

    return 0;
}
