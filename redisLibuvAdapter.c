
#include <stdlib.h>  // malloc(), free()
#include <string.h>  // memset()

#include "redisLibuvAdapter.h"


typedef struct poll_s {
  redisAsyncContext* ac;
  uv_poll_t          handle;
  int                events;
} poll_t;


static void on_poll(uv_poll_t* handle, int status, int events) {
  poll_t* p = (poll_t*)handle->data;

  if (status != 0) {
    return;
  }

  if (events & UV_READABLE) {
    redisAsyncHandleRead(p->ac);
  }
  if (events & UV_WRITABLE) {
    redisAsyncHandleWrite(p->ac);
  }
}


static void add_read(void *privdata) {
  poll_t* p = (poll_t*)privdata;

  p->events |= UV_READABLE;

  uv_poll_start(&p->handle, p->events, on_poll);
}


static void del_read(void *privdata) {
  poll_t* p = (poll_t*)privdata;

  p->events &= ~UV_READABLE;

  if (p->events) {
    uv_poll_start(&p->handle, p->events, on_poll);
  } else {
    uv_poll_stop(&p->handle);
  }
}


static void add_write(void *privdata) {
  poll_t* p = (poll_t*)privdata;

  p->events |= UV_WRITABLE;

  uv_poll_start(&p->handle, p->events, on_poll);
}


static void del_write(void *privdata) {
  poll_t* p = (poll_t*)privdata;

  p->events &= ~UV_WRITABLE;

  if (p->events) {
    uv_poll_start(&p->handle, p->events, on_poll);
  } else {
    uv_poll_stop(&p->handle);
  }
}


static void on_close(uv_handle_t* handle) {
  poll_t* p = (poll_t*)handle->data;

  free(p);
}


static void cleanup(void *privdata) {
  poll_t* p = (poll_t*)privdata;

  uv_close((uv_handle_t*)&p->handle, on_close);
}


int redisLibuvAttach(redisAsyncContext* ac, uv_loop_t* loop) {
  redisContext *c = &(ac->c);

  if (ac->ev.data != NULL) {
    return REDIS_ERR;
  }

  ac->ev.addRead  = add_read;
  ac->ev.delRead  = del_read;
  ac->ev.addWrite = add_write;
  ac->ev.delWrite = del_write;
  ac->ev.cleanup  = cleanup;

  poll_t* p = malloc(sizeof(*p));

  if (!p) {
    return REDIS_ERR;
  }

  memset(p, 0, sizeof(*p));

  if (uv_poll_init(loop, &p->handle, c->fd) != 0) {
    return REDIS_ERR;
  }

  ac->ev.data    = p;
  p->handle.data = p;
  p->ac          = ac;

  return REDIS_OK;
}

