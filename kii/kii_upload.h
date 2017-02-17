#ifndef __example
#define __example

#ifdef __cplusplus
extern 'C' {
#endif
const char EX_AUTH_VENDOR_ID[] = "my-first-thing";
const char EX_AUTH_VENDOR_PASS[] = "123456";

const char EX_APP_SITE[] = "JP";
const char EX_APP_ID[] = "9ab34d8b";
const char EX_APP_KEY[] = "7a950d78956ed39f3b0815f0f001b43b";

#define EX_BUFFER_SIZE 4096

const char EX_BUCKET_NAME[] = "my_bucket";
const char EX_OBJECT_DATA[] = "{}";

int upload(const char* message);

#ifdef __cplusplus
}
#endif

#endif
/* vim:set ts=4 sts=4 sw=4 et fenc=UTF-8 ff=unix: */
