#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

// 用文件作为信号标志
#define FLAG_FILE ".covert_flag"
// Adjust this value based on your system's performance (in nanoseconds)
#define THRESHOLD 150000  // 150 microseconds
// 忙等待指定时间（纳秒）
void busy_wait(long delay_ns) {
    struct timespec start, now;
    clock_gettime(CLOCK_MONOTONIC, &start);
    do {
        clock_gettime(CLOCK_MONOTONIC, &now);
    } while ((now.tv_sec - start.tv_sec) * 1e9 + 
             (now.tv_nsec - start.tv_nsec) < delay_ns);
}

int main() {
    const char *msg = "11110000";  // 要发送的消息
    const int msg_len = 8;
    FILE *flag_file;

    // 初始化标志文件
    flag_file = fopen(FLAG_FILE, "w");
    fputc('0', flag_file);
    fclose(flag_file);

    while (1)
    {
    for (int i = 0; i < msg_len; i++) {
        // 通过修改文件内容发送信号
        flag_file = fopen(FLAG_FILE, "w");
        fputc(msg[i], flag_file);  // 直接将消息写入文件
        fclose(flag_file);

        // 根据比特值调制延迟
        if (msg[i] == '1') {
            busy_wait(200000);  // 200μs (长延迟)
        } else {
            busy_wait(100000);  // 100μs (短延迟)
        }
    }
    }
    

    // 清理
    remove(FLAG_FILE);
    return 0;
}