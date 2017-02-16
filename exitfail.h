#ifndef EXITFAIL_H
#define EXITFAIL_H

#include <errno.h>

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#ifdef MAIN_C
static char *progname = NULL;

#define exitfail_init()                                                 \
        (progname = (strrchr(argv[0], '/') ? : (argv[0] - 1)) + 1)

/**
 * プログラムエラー終了関数
 * @param format 書式フォーマットと引数群
 */
void exitfail(const char *format, ...)
{
        va_list va;

        va_start(va, format);
        fprintf(stderr, "%s: ", progname);
        vfprintf(stderr, format, va);
        va_end(va);

        exit(EXIT_FAILURE);
}
#else
extern void exitfail(const char *__format, ...);
#endif /* MAIN_C */

#define exitfail_errno(msg) exitfail(msg " - %s\n", strerror(errno))

#endif /* EXITFAIL_H */
