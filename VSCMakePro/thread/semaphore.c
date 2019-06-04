#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <semaphore.h>
#define NUM 5
int queue[NUM];
sem_t blank_number, product_number;//信号量
void *producer(void *arg)
{
	int p = 0;
	while (1) {
		sem_wait(&blank_number);//blank_number减1，得到一把锁
		queue[p] = rand() % 1000 + 1;//环形队列 入队列
		printf("Produce %d\n", queue[p]);
		sem_post(&product_number);//product_number加1，consumer sem_wait(&product_number)可以唤醒
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
		sem_post(&blank_number);//消费完空格加1，sem_wait(&blank_number)唤醒producer
		c = (c + 1) % NUM;
		sleep(rand() % 5);
	}
}
int main(int argc, char *argv[])
{
	pthread_t pid, cid;
	sem_init(&blank_number, 0, NUM);//num个信号量，num个锁
	sem_init(&product_number, 0, 0);
	pthread_create(&pid, NULL, producer, NULL);
	pthread_create(&cid, NULL, consumer, NULL);
	pthread_join(pid, NULL);//回收线程的tid，NULL线程推出值
	pthread_join(cid, NULL);
	sem_destroy(&blank_number);
	sem_destroy(&product_number);
	return 0;
}