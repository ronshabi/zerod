#pragma once

#include <stdint.h>

struct buffer
{
    uint64_t len;
    uint64_t cap;
    uint64_t stride;
    void    *data;
};

void buffer_init(struct buffer *buf, uint64_t stride);

void buffer_free(struct buffer *buf);

void buffer_ensure_capacity(struct buffer *buf, uint64_t wanted_cap);

void buffer_clear(struct buffer *buf);

void *buffer_push(struct buffer *buf, const void *elem);

void *buffer_get_item_ptr(struct buffer *buf, uint64_t index);