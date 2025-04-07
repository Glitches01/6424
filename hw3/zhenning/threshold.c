#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>

#define WARMUP_ITER   100     // 预热迭代次数
#define TEST_ITER     1000    // 正式测试迭代次数
#define SHORT_DELAY   100000  // 100μs (模拟0-bit)
#define LONG_DELAY    200000  // 200μs (模拟1-bit)

// 精确延时函数（单位：纳秒）
void precise_delay(long delay_ns) {
    struct timespec start, now;
    clock_gettime(CLOCK_MONOTONIC, &start);
    do {
        clock_gettime(CLOCK_MONOTONIC, &now);
    } while ((now.tv_sec - start.tv_sec)*1e9 + 
            (now.tv_nsec - start.tv_nsec) < delay_ns);
}

// 测量实际延迟时间（单位：纳秒）
uint64_t measure_actual_delay(long target_delay) {
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    precise_delay(target_delay);
    clock_gettime(CLOCK_MONOTONIC, &end);
    return (end.tv_sec - start.tv_sec)*1e9 + 
          (end.tv_nsec - start.tv_nsec);
}

int main() {
    uint64_t short_sum = 0, long_sum = 0;

    printf("[Threshold] System calibration started...\n");
    
    // 预热阶段（稳定CPU频率）
    for(int i=0; i<WARMUP_ITER; i++){
        precise_delay(100000);
    }

    // 测量短延迟（模拟0-bit）
    printf("Measuring 0-bit delays (%d iterations)...\n", TEST_ITER);
    for(int i=0; i<TEST_ITER; i++){
        short_sum += measure_actual_delay(SHORT_DELAY);
        usleep(1000); // 防止CPU过热限频
    }

    // 测量长延迟（模拟1-bit）
    printf("Measuring 1-bit delays (%d iterations)...\n", TEST_ITER);
    for(int i=0; i<TEST_ITER; i++){
        long_sum += measure_actual_delay(LONG_DELAY);
        usleep(1000);
    }

    // 计算结果
    uint64_t avg_0 = short_sum / TEST_ITER;
    uint64_t avg_1 = long_sum / TEST_ITER;
    uint64_t threshold = (avg_0 + avg_1) / 2;

    // 输出报告
    printf("\n=== Calibration Report ===\n");
    printf("Average 0-bit delay: %lu ns (%.3fμs)\n", 
          avg_0, avg_0/1000.0);
    printf("Average 1-bit delay: %lu ns (%.3fμs)\n", 
          avg_1, avg_1/1000.0);
    printf("Recommended threshold: %lu ns (%.3fμs)\n",
          threshold, threshold/1000.0);
    printf("Store this in threshold.h:\n");
    printf("#define THRESHOLD %lu\n", threshold);

    return 0;
}