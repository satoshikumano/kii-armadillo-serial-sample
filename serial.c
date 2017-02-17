#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <termios.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>

#define MAIN_C
#include "exitfail.h"
#include "kii_upload.h"

#define SERIAL_BAUDRATE B9600

#define BUF_SIZE 256

static int serial_fd = -1; /* シリアルポートファイルディスクリプタ */
static struct termios old_tio; /* 元のシリアルポート設定 */

static int terminated = 0; /* 終了シグナル発生フラグ */

#define BASENAME(p)   ((strrchr((p), '/') ? : ((p) - 1)) + 1)
#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

#define __unused __attribute__((unused))

/**
 * 終了シグナルハンドラ
 * @param signum シグナル番号(不使用)
 */
static void terminate_sig_handler(__unused int signum)
{
    /* 終了シグナル発生を記録 */
    terminated = 1;
}

/**
 * シグナルハンドラ設定関数
 * @param sig_list ハンドラを設定するシグナルのリスト
 * @param num シグナルリストの要素数
 * @param handler 設定するハンドラ関数
 */
static void set_sig_handler(int sig_list[], ssize_t num, __sighandler_t handler)
{
    struct sigaction sa;
    int i;

    /* ハンドラ関数を設定 */
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handler;

    /* 各シグナルに対して関連付け */
    for(i = 0; i < num; i++)
        if (sigaction(sig_list[i], &sa, NULL) < 0)
            exitfail_errno("sigaction");
}

/**
 * シリアルポート設定復元関数
 */
static void restore_serial(void)
{
    int ret;

    /* シリアルポートの設定を元に戻す */
    ret = tcsetattr(serial_fd, TCSANOW, &old_tio);
    if (ret < 0)
        exitfail_errno("tcsetattr");
}

/**
 * シリアルポート設定関数
 * @param fd 設定するシリアルポートファイルディスクリプタ
 */
static void setup_serial(int fd)
{
    struct termios tio;
    int ret;

    /* 現在のシリアルポートの設定を退避する */
    ret = tcgetattr(fd, &old_tio);
    if (ret)
        exitfail_errno("tcgetattr");

    /* 終了時に設定を復元するための関数を登録 */
    if (atexit(restore_serial))
        exitfail_errno("atexit");

    /* 新しいシリアルポートの設定 */
    memset(&tio, 0, sizeof(tio));
    tio.c_iflag = IGNBRK | IGNPAR; /* ブレーク文字無視/パリティなし */
    tio.c_cflag = CS8 | CLOCAL | CREAD; /* フロー制御なし/8bit/非モデム/受信可 */
    tio.c_cc[VTIME] = 0; /* キャラクタ間タイマー無効 */
    tio.c_cc[VMIN] = 1; /* 最低1文字送信/受信するまでブロックする */
    ret = cfsetspeed(&tio, SERIAL_BAUDRATE); /* 入出力ボーレート */
    if (ret < 0)
        exitfail_errno("cfsetspeed");

    /* バッファ内のデータをフラッシュ */
    ret = tcflush(fd, TCIFLUSH);
    if (ret < 0)
        exitfail_errno("tcflush");

    /* 新しいシリアルポート設定を適用 */
    ret = tcsetattr(fd, TCSANOW, &tio);
    if (ret)
        exitfail_errno("tcsetattr");
}

/**
 * main関数
 * @param argc 引数なしの場合はusage表示のみ
 * @param argv 第1引数としてシリアルデバイス名を指定
 * @return exit値
 */
int main(int argc, char *argv[])
{
    syslog(LOG_INFO, "started");
    int terminate_sig_list[] = { /* 終了シグナル種類 */
        SIGHUP, SIGINT, SIGQUIT, SIGPIPE, SIGTERM
    };
    char buf[BUF_SIZE];
    ssize_t ret, len, wrlen;

    exitfail_init();

    /* 引数が指定されなかった場合、usage表示して終了 */
    if (argc < 2) {
        printf("Usage: %s <device>\n", BASENAME(argv[0]));
        return EXIT_SUCCESS;
    }

    /* シリアルポートを読み書き可能な非制御端末としてオープン */
    serial_fd = open(argv[1], O_RDWR | O_NOCTTY);
    if (serial_fd < 0)
        exitfail_errno("open");

    /* 終了シグナルに対してハンドラを設定 */
    set_sig_handler(terminate_sig_list, ARRAY_SIZE(terminate_sig_list),
            terminate_sig_handler);

    /* シリアルポートを設定 */
    setup_serial(serial_fd);

    /* 終了シグナルが発生していない限りループ */
    while (!terminated) {
        char c = 0;
        int count = 0;
        const char start = '{';
        const char end = '}';
        while (c != start) {
            ret = read(serial_fd, &c, 1);
            if (ret < 0) {
                exitfail_errno("read");
            }
        }
        while (c != end && count < BUF_SIZE -2) {
	    ret = read(serial_fd, &c, 1);
            if (ret < 0) {
                exitfail_errno("read");
            }
            if (c != end) {
                buf[count] = c;
                count++;
            }
        }
        buf[count + 1] = '\n';
        syslog(LOG_INFO, "READ:");
        syslog(LOG_INFO, buf);
        // TODO: Send Data to Kii Cloud
	int ret = upload(buf);
	if (ret != 0) {
            syslog(LOG_WARNING, "Upload failed.");
	}
    }

    return EXIT_SUCCESS;
}
