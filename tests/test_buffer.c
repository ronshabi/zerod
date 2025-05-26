#include "../buffer.h"
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

void test_pushing_zeros(uint64_t stride, uint64_t amount_of_items)
{
    struct buffer b;

    buffer_init(&b, stride);
    assert(b.data == NULL);

    for (uint64_t i = 0; i < amount_of_items; ++i)
    {
        assert(b.len == i);
        buffer_push_zeros(&b);
        assert(b.len == i + 1);
        assert(b.data);
    }

    buffer_free(&b);
}

int main()
{
    test_pushing_zeros(128, 90);
    return 0;
}