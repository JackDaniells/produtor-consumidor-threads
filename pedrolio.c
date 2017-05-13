#include "pedrolio.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

volatile unsigned int reading = 0;
volatile unsigned int processing = 0;

void blowup(char*);

rock_t pd_read() {
#ifdef __GCC_HAVE_SYNC_COMPARE_AND_SWAP_4
    if (__sync_lock_test_and_set(&reading, 1)) 
      blowup("pd_read");
#endif /*__GCC_HAVE_SYNC_COMPARE_AND_SWAP_4 */
    double random = rand()/(double)RAND_MAX;
    rock_t rock = 5000 + (random*50000);
    usleep(3500);
#ifdef __GCC_HAVE_SYNC_COMPARE_AND_SWAP_4
    __sync_lock_release(&reading);
#endif /*__GCC_HAVE_SYNC_COMPARE_AND_SWAP_4 */
    return rock;
}

oil_t pd_process(rock_t rock) {
#ifdef __GCC_HAVE_SYNC_COMPARE_AND_SWAP_4
    if (__sync_add_and_fetch(&processing, 1) > PROCESSORS_COUNT)
        blowup("pd_process");
#endif /*__GCC_HAVE_SYNC_COMPARE_AND_SWAP_4 */
    usleep(rock);
#ifdef __GCC_HAVE_SYNC_COMPARE_AND_SWAP_4
    __sync_sub_and_fetch(&processing, 1);
#endif /*__GCC_HAVE_SYNC_COMPARE_AND_SWAP_4 */
    return 0.10*rock;
}

double pd_price(oil_t oil) {
    return 0.01*oil;
}

void blowup(char* function_name) {
    printf("Concurrent access to %s() detected!\n", function_name);
    printf("       _.-^^---....,,--    \n"   
           " _--                  --_  \n"
           "<                        >)\n"
           "|                         |\n"
          " \\._                   _./ \n" 
           "    ```--. . , ; .--'''    \n"   
           "          | |   |          \n"   
           "       .-=||  | |=-.       \n"
           "       `-=#$&&&$#=-'       \n"
           "          | ;  :|          \n"
           " _____.,-#&&$@&#&#~,._____ \n");
    abort();
}
