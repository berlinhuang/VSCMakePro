#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <semaphore.h>
#define NUM 5
int queue[NUM];
sem_t blank_number, product_number;//�ź���
void *producer(void *arg)
{
	int p = 0;
	while (1) {
		sem_wait(&blank_number);//blank_number��1���õ�һ����
		queue[p] = rand() % 1000 + 1;//���ζ��� �����
		printf("Produce %d\n", queue[p]);
		sem_post(&product_number);//product_number��1��consumer sem_wait(&product_number)���Ի���
		p = (p + 1) % NUM;
		sleep(rand() % 5);
	}
}
void *consumer(void *arg)
{
	int c = 0;
	while (1) {
		sem_wait(&product_number);
		printf("Consume %d\n", queue[c]);
		queue[c] = 0;
		sem_post(&blank_number);//������ո��1��sem_wait(&blank_number)����producer
		c = (c + 1) % NUM;
		sleep(rand() % 5);
	}
}
int main(int argc, char *argv[])
{
	pthread_t pid, cid;
	sem_init(&blank_number, 0, NUM);//num���ź�����num����
	sem_init(&product_number, 0, 0);
	pthread_create(&pid, NULL, producer, NULL);
	pthread_create(&cid, NULL, consumer, NULL);
	pthread_join(pid, NULL);//�����̵߳�tid��NULL�߳��Ƴ�ֵ
	pthread_join(cid, NULL);
	sem_destroy(&blank_number);
	sem_destroy(&product_number);
	return 0;
}