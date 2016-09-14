#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <pthread.h>
#include <semaphore.h>

int total = 0;
sem_t reader_sem, db_sem;
int readCount = 0;

void* reader(int* result);
void* writer(int* result);

void* handle_connection(void *arg)
{
    int client_sock = *(int*)arg;
    int server_value = 0;
    int client_type = 0;
    if ((client_type = recv(client_sock, &server_value, sizeof(int), 0)) < sizeof(int)){
        perror("receive: ");
    }
    else{
        int result = 0;
        if (client_type){
            writer(&result);
        }
        else{
            reader(&result);
        }
        if (send(client_sock,&result,1,0) <0)
            perror("send: ");
    }
    return NULL;
}

void* reader(int* result){
    
    sem_wait(&reader_sem);
        readCount++;
        if (readCount == 1){
            sem_wait(&db_sem);
        }
        sem_post(&reader_sem);
        result = &total;
        sem_wait(&reader_sem);
        readCount--;
        if(readCount==0){
            sem_post(&db_sem);
        }
        sem_post(&reader_sem);
}
void* writer(int* result){
    sem_wait(&db_sem);
        total++;
        result = &total;
    sem_post(&db_sem);
}

int main(int argc, char *argv[])
{
    sem_init(&reader_sem,0,1);
    sem_init(&db_sem,0,1);
    int listenfd = 0, connfd = 0;
    struct sockaddr_in serv_addr, clientaddr;
    char sendBuff[1025];
    socklen_t sz;


    memset(sendBuff, '0', sizeof(sendBuff)); 

    /* Passo 1 - Criar Socket */
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
	  perror("socket: ");
	  exit(1);
    }

    /* Passo 2 - Configurar estrutura sockaddr_in */
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(5000); 
    
    /* Passo 3 - Associar socket com a estrutura sockaddr_in     */
    if (bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0){
   	  perror("bind: ");
	  exit(1);
    }

    /* Passo 4 - Tornar o servidor ativo  */
    listen(listenfd, 10); 

    while(1)
    {
        /* Passo 5 - Aguardar conexão do cliente.  */
        sz = sizeof(clientaddr);
        pthread_t client_threadid;
        while((connfd = accept(listenfd, (struct sockaddr*)&clientaddr, &sz)) != -1)
        {
            pthread_create(&client_threadid,NULL,handle_connection, &connfd);
        }
        close(connfd);
     }
}

