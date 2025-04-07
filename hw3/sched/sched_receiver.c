// ========================
// Author: Nanqi Gong
// UNI: ng2994
// Date: 04/01/2025
// ========================
// sched_receiver.c

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <time.h>

#define THRESHOLD 4326890
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
    printf("[Receiver] Receiving bits...\n");
    int bit_count = 0;

    while (1) {
        uint64_t loops = measure_loop_count(BIT_DURATION_MS);
        int bit = (loops < THRESHOLD) ? 1 : 0;
        //printf("loops: %d bit: %d\n",loops,bit);
        printf("%d", bit);
        fflush(stdout);

        if (++bit_count % 8 == 0) printf("\n");
    }

    return 0;
}

