// ========================
// Author: Nanqi Gong
// UNI: ng2994
// Date: 04/01/2025
// ========================
// sched_sender.c 

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

void busy_wait(uint64_t duration_ms) {
    struct timespec start, now;
    clock_gettime(CLOCK_MONOTONIC, &start);
    do {
        for (volatile int i = 0; i < 10000; ++i);
        clock_gettime(CLOCK_MONOTONIC, &now);
    } while ((now.tv_sec - start.tv_sec) * 1000 +
             (now.tv_nsec - start.tv_nsec) / 1000000 < duration_ms);
}

int main(int argc, char *argv[]) {
    const char *msg = "11111111" "00000000";
    int bit_duration_ms = 200;

    if (argc > 1) msg = argv[1];

    printf("[Sender] Sending bitstring: %s\n", msg);

    while (1) {
        for (int i = 0; msg[i] != '\0'; ++i) {
            if (msg[i] == '1')
                busy_wait(bit_duration_ms);
            else
                usleep(bit_duration_ms * 1000);
        }
    }

    return 0;
}

