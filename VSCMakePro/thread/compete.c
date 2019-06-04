
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#define NLOOP 5000
int counter; /* incremented by threads */
void *doit(void *);

int main(int argc, char **argv)
{
	pthread_t tidA, tidB;
	//&tidA用于保存新线程的tid(线程ID)
	//NULL线程属性设置,如使用默认属性,则传NULL
	//函数指针,指向新线程应该加载执行的函数模块
	//指定线程将要加载调用的那个函数的参数
	pthread_create(&tidA, NULL, &doit, NULL);
	pthread_create(&tidB, NULL, &doit, NULL);
	/* wait for both threads to terminate */
	pthread_join(tidA, NULL);
	pthread_join(tidB, NULL);
	return 0;
}

void *doit(void *vptr)
{
	int i, val;
	for (i = 0; i < NLOOP; i++) {
		val = counter;
		printf("%x: %d\n", (unsigned int)pthread_self(), val + 1);//pthread_self()返回tid
		counter = val + 1;
	}
	return NULL;
}