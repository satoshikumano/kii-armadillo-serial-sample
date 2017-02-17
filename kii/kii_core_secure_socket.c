#include "kii_core_secure_socket.h"

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include <openssl/crypto.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/bio.h>

#include <syslog.h>
/* Suppress warnings, because OpenSSL was deprecated in Mac. */
#ifdef __APPLE__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

typedef struct _ssl_context
{
    SSL *ssl;
    SSL_CTX *ssl_ctx;

} ssl_context_t;

kii_socket_code_t
    s_connect_cb(kii_socket_context_t* socket_context, const char* host,
            unsigned int port)
{
    int sock, ret;
    struct hostent *servhost;
    struct sockaddr_in server;
    ssl_context_t *ctx = NULL;
    SSL *ssl = NULL;
    SSL_CTX *ssl_ctx = NULL;

    ctx = (ssl_context_t*)malloc(sizeof(ssl_context_t));
    memset(ctx, 0x00, sizeof(ssl_context_t));

    servhost = gethostbyname(host);
    if (servhost == NULL) {
        printf("failed to get host.\n");
        free(ctx);
        return KII_SOCKETC_FAIL;
    }
    memset(&server, 0x00, sizeof(server));
    server.sin_family = AF_INET;
    /* More secure. */
    memcpy(&(server.sin_addr), servhost->h_addr, servhost->h_length);

    /* Get Port number */
    server.sin_port = htons(port);

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        printf("failed to init socket.\n");
        free(ctx);
        return KII_SOCKETC_FAIL;
    }

    if (connect(sock, (struct sockaddr*) &server, sizeof(server)) == -1 ){
        printf("failed to connect socket.\n");
        free(ctx);
        return KII_SOCKETC_FAIL;
    }

    SSL_library_init();
    BIO* bio = BIO_new_socket(sock, BIO_NOCLOSE);
    if (bio == NULL) {
        printf("BIO new socket failed.\n");
        free(ctx);
        return KII_SOCKETC_FAIL;
    }
    ssl_ctx = SSL_CTX_new(SSLv23_client_method());
    if (ssl_ctx == NULL){
        printf("failed to init ssl context.\n");
        free(ctx);
        return KII_SOCKETC_FAIL;
    }

    ssl = SSL_new(ssl_ctx);
    if (ssl == NULL){
        printf("failed to init ssl.\n");
        free(ctx);
        return KII_SOCKETC_FAIL;
    }
    SSL_set_bio(ssl, bio, bio);
    SSL_set_connect_state(ssl);
    ret = SSL_connect(ssl);
    if (ret != 1) {
        int sslErr= SSL_get_error(ssl, ret);
        char sslErrStr[120];
        ERR_error_string_n(sslErr, sslErrStr, 120);
        printf("failed to connect: %s\n", sslErrStr);
        free(ctx);
        return KII_SOCKETC_FAIL;
    }
    socket_context->socket = sock;
    socket_context->app_context = (void*)ctx;
    ctx->ssl = ssl;
    ctx->ssl_ctx = ssl_ctx;
    return KII_SOCKETC_OK;
}

kii_socket_code_t
    s_send_cb(kii_socket_context_t* socket_context,
            const char* buffer,
            size_t length)
{
    ssl_context_t* ctx = (ssl_context_t*)socket_context->app_context;
    int ret = SSL_write(ctx->ssl, buffer, length);
    if (ret > 0) {
        return KII_SOCKETC_OK;
    } else {
	int ssl_error = SSL_get_error(ctx->ssl, ret);
	if (ssl_error == SSL_ERROR_WANT_READ || ssl_error == SSL_ERROR_WANT_WRITE) {
            syslog(LOG_INFO, "core.send.want_read_write");
	    return KII_SOCKETC_AGAIN;
	}
        syslog(LOG_WARNING, "core.failed to send");
        return KII_SOCKETC_FAIL;
    }
}

kii_socket_code_t
    s_recv_cb(kii_socket_context_t* socket_context,
            char* buffer,
            size_t length_to_read,
            size_t* out_actual_length)
{
    ssl_context_t* ctx = (ssl_context_t*)socket_context->app_context;
    int ret = SSL_read(ctx->ssl, buffer, length_to_read);
    if (ret > 0) {
        *out_actual_length = ret;
        return KII_SOCKETC_OK;
    } else {
        *out_actual_length = 0;
	int ssl_error = SSL_get_error(ctx->ssl, ret);
	if (ssl_error == SSL_ERROR_WANT_READ || ssl_error == SSL_ERROR_WANT_WRITE) {
            syslog(LOG_INFO, "core.read.want_read_write");
	    return KII_SOCKETC_AGAIN;
	}
	if (ssl_error == SSL_ERROR_ZERO_RETURN) {
            syslog(LOG_INFO, "core.zero_return");
            return KII_SOCKETC_OK;
	}
        syslog(LOG_WARNING, "core.failed to recv. ssl_error %d", ssl_error);
        return KII_SOCKETC_FAIL;
    }
}

kii_socket_code_t
    s_close_cb(kii_socket_context_t* socket_context)
{
    ssl_context_t* ctx = (ssl_context_t*)socket_context->app_context;
    int ret = SSL_shutdown(ctx->ssl);
    if (ret != 1) {
        int sslErr = SSL_get_error(ctx->ssl, ret);
        if (sslErr == SSL_ERROR_SYSCALL) {
            /* This is OK.*/
            /* See https://www.openssl.org/docs/ssl/SSL_shutdown.html */
            ret = 1;
        } else {
            char sslErrStr[120];
            ERR_error_string_n(sslErr, sslErrStr, 120);
            printf("failed to shutdown: %s\n", sslErrStr);
        }
    }
    close(socket_context->socket);
    SSL_free(ctx->ssl);
    SSL_CTX_free(ctx->ssl_ctx);
    if (ret != 1) {
        printf("failed to close:\n");
        return KII_SOCKETC_FAIL;
    }
    free(ctx);
    socket_context->app_context = NULL;
    return KII_SOCKETC_OK;
}

#ifdef __APPLE__
#pragma GCC diagnostic pop
#endif
