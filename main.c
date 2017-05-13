#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include "pedrolio.h"
#include "module.h"

double sum = 0;
volatile unsigned int delivering = 0;

void blowup(char*);
void pd_deliver(oil_t oil) {
#ifdef __GCC_HAVE_SYNC_COMPARE_AND_SWAP_4
    if (__sync_lock_test_and_set(&delivering, 1))
        blowup("pd_deliver");
#endif /*__GCC_HAVE_SYNC_COMPARE_AND_SWAP_4 */
    sum += pd_price(oil);
#ifdef __GCC_HAVE_SYNC_COMPARE_AND_SWAP_4
    __sync_lock_release(&delivering);
#endif /*__GCC_HAVE_SYNC_COMPARE_AND_SWAP_4 */
}

int main(int argc, char** argv) {
    double final = 0;

    srand(1);

    printf("Setting up %s...\n", mod_name());
    mod_setup();
    printf("%s setup\n", mod_name());
    sleep(10);
    final = sum;
    mod_shutdown();
    printf("%s produced %.2f monetary units\n", mod_name(), final);
    
    return 0;
}
