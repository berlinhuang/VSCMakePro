#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#define NLOOP 5000
int counter; /* incremented by threads */
pthread_mutex_t counter_mutex = PTHREAD_MUTEX_INITIALIZER;//静态初始化
void *doit(void *);

int main(int argc, char **argv)
{
	//pthread_mutex_t mutex;
	//pthread_mutex_init(&mutex, NULL);//函数动态初始化

	pthread_t tidA, tidB;
	pthread_create(&tidA, NULL, doit, NULL);
	pthread_create(&tidB, NULL, doit, NULL);
	/* wait for both threads to terminate */
	pthread_join(tidA, NULL);//回收线程 NULL接收退出线程传递出的返回值
	pthread_join(tidB, NULL);
	return 0;
}

void *doit(void *vptr)
{
	int i, val;
	for (i = 0; i < NLOOP; i++) {
		pthread_mutex_lock(&counter_mutex);//拿锁和解锁的区域是临界区 锁住的范围
		val = counter;
		printf("%x: %d\n", (unsigned int)pthread_self(), val + 1);
		counter = val + 1;
		pthread_mutex_unlock(&counter_mutex);
	}
	return NULL;
}