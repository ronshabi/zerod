#include "buffer.h"
#include "panic.h"

#include <stdlib.h>
#include <string.h>

//
// Internal functions
//

static void prepare_push(struct buffer *buf)
{
    if (buf->len == buf->cap)
    {
        if (buf->data == NULL)
        {
            buf->data = malloc(buf->stride);
            if (buf->data == NULL)
            {
                panic("buffer malloc failed");
            }

            buf->len = 0;
            buf->cap = 1;
        }
        else
        {
            buf->data = realloc(buf->data, buf->cap * buf->stride * 2);
            if (buf->data == NULL)
            {
                panic("buffer realloc failed");
            }

            buf->cap *= 2;
        }
    }
}

//
// Public functions
//

void buffer_init(struct buffer *buf, uint64_t stride)
{
    memset(buf, 0, sizeof(struct buffer));
    buf->stride = stride;
}

void buffer_free(struct buffer *buf)
{
    if (buf->data)
    {
        free(buf->data);
        buf->data = NULL;
    }

    buf->cap = 0;
    buf->len = 0;
}

void buffer_clear(struct buffer *buf)
{
    if (buf->data)
    {
        memset(buf->data, 0, buf->cap * buf->stride);
    }
}

void *buffer_push(struct buffer *buf, const void *elem)
{
    prepare_push(buf);
    memcpy(buf->data + buf->len * buf->stride, elem, buf->stride);
    ++buf->len;
    return buf->data + (buf->len - 1) * buf->stride;
}

void *buffer_push_zeros(struct buffer *buf)
{
    prepare_push(buf);
    memset(buf->data + buf->len * buf->stride, 0, buf->stride);
    ++buf->len;
    return buf->data + (buf->len - 1) * buf->stride;
}
{
    if (index >= buf->len)
    {
        return NULL;
    }

    return buf->data + index * buf->stride;
}
