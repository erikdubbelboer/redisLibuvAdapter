
#ifndef __REDISLIBUVADAPTER_H__
#define __REDISLIBUVADAPTER_H__


#include "async.h"  /* hiredis */
#include "uv.h"     /* libuv   */


#ifdef __cplusplus
extern "C" {
#endif


int redisLibuvAttach(redisAsyncContext *ac, uv_loop_t* loop);


#ifdef __cplusplus
}
#endif


#endif /* __REDISLIBUVADAPTER_H__ */

