// ========================
// Author: Nanqi Gong
// UNI: ng2994
// Date: 04/01/2025
// ========================
// sched_threshold.c 

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <time.h>

#define SAMPLE_COUNT 100
#define BIT_DURATION_MS 200

uint64_t measure_loop_count(int ms) {
    struct timespec start, now;
    uint64_t count = 0;

    clock_gettime(CLOCK_MONOTONIC, &start);
    do {
        for (int i = 0; i < 100; ++i) asm volatile("");
        count++;
        clock_gettime(CLOCK_MONOTONIC, &now);
    } while ((now.tv_sec - start.tv_sec) * 1000 +
             (now.tv_nsec - start.tv_nsec) / 1000000 < ms);

    return count;
}

int main() {
    printf("[Threshold] Measuring loop capacity...\n");

    for (int i = 0; i < SAMPLE_COUNT; ++i) {
        uint64_t loops = measure_loop_count(BIT_DURATION_MS);
        printf("Sample %3d: %lu loops\n", i + 1, loops);
        usleep(100000); // 100ms
    }

    return 0;
}

