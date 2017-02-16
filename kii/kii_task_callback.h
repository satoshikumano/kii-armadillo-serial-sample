#ifndef _KII_TASK_CALLBACK
#define _KII_TASK_CALLBACK

#ifdef __cplusplus
extern "C" {
#endif

typedef enum kii_task_code_t {
    KII_TASKC_OK,
    KII_TASKC_FAIL
} kii_task_code_t;

typedef void* (*KII_TASK_ENTRY)(void* value);

typedef kii_task_code_t
(*KII_TASK_CREATE)
    (const char* name,
     KII_TASK_ENTRY entry,
     void* param);

typedef void
(*KII_DELAY_MS)
    (unsigned int msec);

#ifdef __cplusplus
}
#endif

#endif /* _KII_TASK_CALLBACK  */
/* vim:set ts=4 sts=4 sw=4 et fenc=UTF-8 ff=unix: */
