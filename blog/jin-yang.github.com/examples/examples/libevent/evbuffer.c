#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>

int main(int argc, char** argv)
{
    struct evbuffer* buff = NULL;
    char c, c2[3] = {0};

    buff = evbuffer_new();
    assert(buff != NULL);

    evbuffer_add(buff, "1", 1);
    evbuffer_add(buff, "2", 1);
    evbuffer_add(buff, "3", 1);
    evbuffer_add_printf(buff, "%d%d", 4, 5);
    assert(buff->off == 5);

    evbuffer_remove(buff, &c, sizeof(char));
    assert(c == '1');
    evbuffer_remove(buff, &c, sizeof(char));
    assert(c == '2');
    evbuffer_remove(buff, &c, sizeof(char));
    assert(c == '3');
    evbuffer_remove(buff, c2, 2);
    assert(strcmp(c2, "45") == 0);

    assert(buff->off == 0);

    evbuffer_add(buff, "test\r\n", 6);
    assert(buff->off == 6);

    char* line = evbuffer_readline(buff);
    assert(strcmp(line, "test") ==0);
    assert(buff->off == 0);
    free(line);

    evbuffer_free(buff);

    printf("ok\n");

    return 0;
}
