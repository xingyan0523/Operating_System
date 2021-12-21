#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <stdatomic.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/syscall.h> 
#include <assert.h>
#include <string.h>
#include <math.h>

long long int loopCount = 1000000000;

int numCPU = -1;
int digi = 8;

/*long gettid() {
    return (long int)syscall(__NR_gettid);
}*/

//注意，我使用了「volatile」
volatile long double score[100];

void thread(void *givenName) {
    int id = (intptr_t)givenName;
    long double ans = 0, part = (long double)1. / loopCount;
    long avg = loopCount / numCPU;
    long start = avg * id;
    long end = start + avg;

    for(long i = start; i < end; i++){
        long double x = part * i;
        ans += part * sqrt(1 - x * x);
    }
    score[id] += ans;
}

int main(int argc, char **argv) {
    numCPU = sysconf( _SC_NPROCESSORS_ONLN );
    
    if(argc > 1) digi = atoi(argv[1]);
    
    long double up = 1, low = 0, tolerant = pow(10, -(digi + 1));
    int count = 1;
    while(up - low >= tolerant * 4){
        up = 1;
        low = 0;

        pthread_t* tid = (pthread_t*)malloc(sizeof(pthread_t) * numCPU);

        for (long i = 0; i < numCPU; i++)
            pthread_create(&tid[i], NULL, (void *) thread, (void*)i);

        for (int i = 0; i < numCPU; i++)
            pthread_join(tid[i], NULL);

        for (int i = 0; i < numCPU; i++) 
            low += score[i];

        up = low + (long double)1 / loopCount;
        long double ans = (up + low) * 2 / count; //(up + low) / 2 * 4
    	printf("pi = %.16LF\n", ans);

        loopCount += 1000000000;
        count++;
    }
    return 0;
}
