#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h> 
#include <pthread.h>
#include "queue.h"

int running = 1;
Queue* Q;


int communicate(int type){
    int sockfd = 0;
    struct sockaddr_in serv_addr;
    
    /* Passo 1 - Criar socket */
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket: ");
        exit(1);
    }
    /* Passo 2 - Configura struct sockaddr_in */
    memset(&serv_addr, '0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(5555);
    /* converte Ip em formato string para o formato exigido pela struct sockaddr_in*/
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0){
        printf("\n inet_pton error occured\n");
        exit(1);
    }
    if( connect(sockfd, (struct sockaddr_in*) &serv_addr, sizeof(serv_addr)) < 0){
        perror("connect: ");
        exit(1);
    }
    int  n = 0;
    int server_value =0 ;
    if (send(sockfd,&type,1,0) <0)
        printf ("error\n");
    else
        printf ("packet send done");
    /* Enviar ou receber dados do servidor */
    /* Enquanto o numero de bytes recebidos for differente de 0*/
    if (recv(sockfd, &server_value, sizeof(int), 0) < sizeof(int))
    {
    	perror("receive error: ");
    }
    else{
        /*Print value*/
    }
    return 1;
}

void * producer(int size){
    int i;
	for(i=0; i<size; i = i+2){
            Enqueue(Q,0);
            Enqueue(Q,1);
	}
    return NULL;
}

void * consumer(){
	while(running){
            int type = front(Q);
            Dequeue(Q);
            communicate(type);
	}
    return NULL;
}

int main(int argc, char *argv[])
{
    int i;
    int sockfd = 0, n = 0, numthr = 0;
    pthread_t prod, *con;
    char recvBuff[1024];

    if(argc != 3)
    {
        printf("\n Usage: %s <num_workers> <num_threads>\n",argv[0]);
        return 1;
    }
    if(atoi(argv[3]) % 2 != 0){
        printf("\n Num of workers must be even!\n");
        return 1;
    }
    int num_workers = 0;
    num_workers = (atoi(argv[2]));
    Q = createQueue(num_workers);
    numthr = atoi(argv[3]);
    con = (pthread_t *)malloc(numthr * sizeof(pthread_t));
    if(pthread_create(&prod, NULL, producer, num_workers)) {
        fprintf(stderr, "Error creating thread\n");
	return 1;
    }
    for(i=0; i<numthr; i++){
        if(pthread_create(&con[i], NULL, consumer, NULL)) {
            fprintf(stderr, "Error creating thread\n");
            return 2;
        }
	}
    if(pthread_join(prod, NULL)) {
        fprintf(stderr, "Error joining thread\n");
        return 3;
    }
    /* encerra execução das threads produtoras. */
    running = 0;
    for(i=0; i<numthr; i++){
        if(pthread_join(con[i], NULL)) {
            fprintf(stderr, "Error joining thread\n");
            return 4;
        }
    }
        
    return 0;
}
