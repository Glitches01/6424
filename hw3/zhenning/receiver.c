#define THRESHOLD 21539372
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define FLAG_FILE ".covert_flag"

int main() {
    FILE *flag_file;
    char last_bit = '0';
    struct timespec prev_time, curr_time;
    char received_msg[9] = {0};
    int bit_count = 0;

    printf("正在监听消息...\n");
    clock_gettime(CLOCK_MONOTONIC, &prev_time);

    while (bit_count < 8) {
        flag_file = fopen(FLAG_FILE, "r");
        if (flag_file) {
            char current_bit = fgetc(flag_file);
            fclose(flag_file);

            if (current_bit != last_bit && current_bit != EOF) {
                // 测量时间间隔
                clock_gettime(CLOCK_MONOTONIC, &curr_time);
                long interval = (curr_time.tv_sec - prev_time.tv_sec) * 1e9 + 
                                (curr_time.tv_nsec - prev_time.tv_nsec);
                
                printf("interval: %d\n",interval);
                // 根据阈值解码比特
                received_msg[bit_count] = (interval > THRESHOLD) ? '1' : '0';
                
                last_bit = current_bit;
                prev_time = curr_time;
                bit_count++;
            }
        }
    }

    printf("接收到的消息: %s\n", received_msg);
    return 0;
}