#include <stdio.h>
#include "module.h"
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <semaphore.h>

int cancelled;
int setup = 0;


//cria as threads
pthread_t workThread[PROCESSORS_COUNT], prodThread;
//cria os mutex
pthread_mutex_t lockDeliver, lockCounter, lockWorker;
//cria os semáforos
sem_t lockEmpty, lockFull;
//cria o buffer
rock_t bufferRock[PROCESSORS_COUNT];
//cria as variáveis globais do buffer
int posReadBuffer = 0, posWriteBuffer = 0, countRock = 0;




/*************************************************************
*
*                           Threads
*
/*************************************************************/
//thread produtora
static void* prod() {

    while (cancelled == 0) {

        sem_wait(&lockEmpty);

        //pega a pedra e joga ela no buffer
        bufferRock[posWriteBuffer] = pd_read();

        //incrementa o posWrite
        posWriteBuffer = (posWriteBuffer + 1) % PROCESSORS_COUNT;

        /*pthread_mutex_lock(&lockCounter);
        countRock++;
        pthread_mutex_unlock(&lockCounter);*/

        sem_post(&lockFull);
        
    }

    //sai da thread
    pthread_exit(NULL);
    return NULL;
}
/*************************************************************/
//threads consumidoras (UPPs)
static void* work(void* ignored) {

    while ( cancelled == 0 ) {


        sem_wait(&lockFull);
        pthread_mutex_lock(&lockWorker);

        //incrementa o posRead
        posReadBuffer = (posReadBuffer + 1) % PROCESSORS_COUNT;
        
        /*pthread_mutex_lock(&lockCounter);
        countRock--;
        printf("%d\n", countRock );
        pthread_mutex_unlock(&lockCounter);*/

        pthread_mutex_unlock(&lockWorker);
        sem_post(&lockEmpty);

        //processa a pedra do buffer e transforma em óleo
        oil_t oil = pd_process(bufferRock[posReadBuffer]);
    

        //trava as outras threads enquanto pd_deliver() executa
        pthread_mutex_lock(&lockDeliver);
        pd_deliver(oil);
        pthread_mutex_unlock(&lockDeliver);
    }

    //sai da thread
    pthread_exit(NULL);
    return NULL;
}
/*************************************************************
*
*   Funções do módulo
*
/*************************************************************/
void mod_setup() {

    assert(setup == 0);
    setup = 1;
    cancelled = 0;


    //inicia as variáveis mutex
    pthread_mutex_init(&lockDeliver, NULL);
    pthread_mutex_init(&lockCounter, NULL);
    pthread_mutex_init(&lockWorker, NULL);


    //inicia os semáforos
    sem_init(&lockFull, 0, 0);
    sem_init(&lockEmpty, 0, PROCESSORS_COUNT);
 

    //cria as threads
    pthread_create(&prodThread, NULL, &prod, NULL);
    for (int i = 0; i < PROCESSORS_COUNT; ++i) 
    {   
        pthread_create(&workThread[i], NULL, &work, NULL);
    }
   
}
/*************************************************************/
char* mod_name() {
    return "grupo9";
}
/*************************************************************/
void mod_shutdown() {
    assert(setup == 1);
    cancelled = 1;

    //da join nas threads
    pthread_join(prodThread, NULL);
    for (int i = 0; i < PROCESSORS_COUNT; ++i) 
    { 
        pthread_join(workThread[i], NULL);
    }

    //destroi as variáveis mutex
    pthread_mutex_destroy(&lockDeliver);
    pthread_mutex_destroy(&lockCounter);

    //destroi os semáforos
    sem_destroy(&lockEmpty);
    sem_destroy(&lockFull);
}

