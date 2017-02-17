#include "kii.h"
#include "kii_init_impl.h"

#include "kii_upload.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <syslog.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdarg.h>

#define LOGGER_BUF_SIZE 1024
static void logger(const char* format, ...)
{
    char msg[LOGGER_BUF_SIZE];
    va_list list;
    msg[LOGGER_BUF_SIZE-1] = '\0';
    va_start(list, format);
    vsprintf(msg, format, list);
    va_end(list);
    syslog(LOG_INFO, msg);
}

int upload(const char* message)
{
    kii_t kii;
    char buffer[EX_BUFFER_SIZE];
    size_t buffer_size = EX_BUFFER_SIZE;
    int ret;
    kii_bucket_t bucket;
    char object_data[512];
    char scope_id[128];
    char object_id[KII_OBJECTID_SIZE + 1];
    char content_type[128];

    memset(buffer, 0x00, buffer_size);
    memset(object_id, 0x00, sizeof(object_id));

    // SDK Initilization.
    kii_impl_init(&kii, EX_APP_SITE, EX_APP_ID, EX_APP_KEY);
    kii.kii_core.http_context.buffer = buffer;
    kii.kii_core.http_context.buffer_size = buffer_size;
    kii.kii_core.http_context.socket_context.app_context = NULL;
    kii.kii_core.logger_cb = logger;

    // Prepare bucket.
    memset(&bucket, 0x00, sizeof(kii_bucket_t));
    bucket.scope = KII_SCOPE_THING;
    bucket.bucket_name = (char*)EX_BUCKET_NAME;
    memset(scope_id, 0x00, sizeof(scope_id));
    sprintf(scope_id, "VENDOR_THING_ID:%s", EX_AUTH_VENDOR_ID);
    bucket.scope_id = scope_id;

    // Authentication
    ret = kii_thing_authenticate(&kii, EX_AUTH_VENDOR_ID, EX_AUTH_VENDOR_PASS);
    if (ret != 0) {
	syslog(LOG_WARNING, "Failed to authenticate thing");
	return ret;
    }
    syslog(LOG_INFO, "Authenticate thing succeeded!");

    // Upload Object.
    memset(object_data, 0x00, sizeof(object_data));
    const char* json1 = "{\"message\":\"";
    const size_t json1_len = strlen(json1);
    const char* json2 = "\"}";
    const size_t json2_len = strlen(json2);
    const size_t message_len = strlen(message);
    const char* fallback = "message too large";

    strncpy(object_data, json1, strlen(json1));
    if (json1_len + message_len + json2_len > sizeof(object_data)) {
	// Avoid Overflow.
    	strncat(object_data, fallback, strlen(fallback));
    } else {
    	strncat(object_data, message, strlen(message));
    }
    strncat(object_data, json2, strlen(json2));
    syslog(LOG_INFO, "Payload: %s", object_data);
    ret = kii_object_create(&kii, &bucket, object_data, NULL, object_id);
    if (ret != 0) {
	syslog(LOG_WARNING, "Failed to upload object");
	return ret;
    }
    syslog(LOG_INFO, "Upload object succeeded!");
    return ret;
}

/* vim:set ts=4 sts=4 sw=4 et fenc=UTF-8 ff=unix: */
