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
pthread_mutex_t lockDeliver, lockCounter;
pthread_mutex_t lockTeta;
//cria o buffer
rock_t bufferRock[PROCESSORS_COUNT];
//cria as variáveis globais do buffer
int posReadBuffer = 0, posWriteBuffer = 0, countRock = 0;
sem_t semTeta;





//função que manipula o contador, criada pra evitar condição de corrida no acesso a variavel counter
int CountRock(int n){

    //variável criada pra evitar o acesso a variável "counter" durante a execução paralela entre threads
    int back = -1;

    //trava as threads enquanto incrementa o contador
    pthread_mutex_lock(&lockCounter);
    countRock = countRock + n;
    back = countRock;
    pthread_mutex_unlock(&lockCounter);
    printf("%d\n", back);

    //retorna o valor do contador
    return back;
}


/*************
*   Threads
*************/

//thread produtora
static void* prod() {

    while (cancelled == 0) {

        //trava a thread se o buffer estiver cheio
        while(countRock >= PROCESSORS_COUNT);

        //incrementa o posWrite
        posWriteBuffer++;
        if(posWriteBuffer == PROCESSORS_COUNT ) posWriteBuffer = 0;

        //pega a pedra e joga ela no buffer
        bufferRock[posWriteBuffer] = pd_read();

        //incrementa o contador
        CountRock(1);

        
    }

    return NULL;
}


//threads consumidoras (UPPs)
static void* work(void* ignored) {

    while ( cancelled == 0 ) {

        //trava a thread se o buffer estiver vazio
        while(countRock <= 0);

        sem_wait($semTeta);
        //incrementa o posRead
        posReadBuffer++;
        if ( posReadBuffer == PROCESSORS_COUNT ) posReadBuffer = 0;

        //processa a pedra do buffer e transforma em óleo
        oil_t oil = pd_process(bufferRock[posReadBuffer]);
        
        //decrementa o contador
        CountRock(-1);
        sem_post(&semTeta);

        //trava as outras threads enquanto pd_deliver() executa
        pthread_mutex_lock(&lockDeliver);
        pd_deliver(oil);
        pthread_mutex_unlock(&lockDeliver);

    }
    return NULL;
}

/************************
*   Funções do módulo
************************/

void mod_setup() {

    assert(setup == 0);
    setup = 1;
    cancelled = 0;

    //inicia as variáveis mutex
    pthread_mutex_init(&lockDeliver, NULL);
    pthread_mutex_init(&lockCounter, NULL);
   
    sem_init(&semTeta, NULL, NULL);


    //cria as threads
    pthread_create(&prodThread, NULL, &prod, NULL);
    for (int i = 0; i < PROCESSORS_COUNT; ++i) 
    {   
        pthread_create(&workThread[i], NULL, &work, NULL);
    }
   
}


char* mod_name() {
    return "grupo9";
}


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
}

