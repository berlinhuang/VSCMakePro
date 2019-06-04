

//条件变量一般和互斥锁一起使用
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
struct msg {
	struct msg *next;
	int num;
};
struct msg *head;
pthread_cond_t has_product = PTHREAD_COND_INITIALIZER;//条件变量 静态初始化
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;//互斥量

void *consumer(void *p)
{
	struct msg *mp;
	for (;;) {
		pthread_mutex_lock(&lock);//1.获取互斥锁，为了调用pthread_cond_wait()
		//signal可能会激活多个线程(阻塞在条件变量的线程)，这就是虚假唤醒
		while (head == NULL)//使用while 直到线程进入pthread_cond_wait
			pthread_cond_wait(&has_product, &lock);
		//2.释放互斥锁lock，pthread_mutex_unlock(&lock) 否则生产者无法获取互斥锁
		//pthread_cond_wait(&has_product, &lock) 阻塞等待has_product 被唤醒
		//3.当被唤醒时，解除阻塞，并且重新去申请获得互斥锁 pthread_mutex_lock(&lock)然后返回,获取不到会阻塞
		mp = head;
		head = mp->next;
		pthread_mutex_unlock(&lock);//4.消费完释放互斥锁
		printf("Consume %d\n", mp->num);
		free(mp);
		sleep(rand() % 5);
	}
}

void *producer(void *p)
{
	struct msg *mp;
	for (;;) {
		mp = malloc(sizeof(struct msg));
		mp->num = rand() % 1000 + 1;
		printf("Produce %d\n", mp->num);
		pthread_mutex_lock(&lock);
		mp->next = head;
		head = mp;
		//调用pthread_cond_signal()释放互斥锁
		//
		pthread_mutex_unlock(&lock);
		//唤醒一个等待的线程
		//pthread_cond_broacast 则会唤醒等待队列上所有的线程，此时发生竞争
		//可以放在pthread_mutex_lock() 和pthread_mutex_unlock()之间 也可以放在之后，有各自优缺点
		pthread_cond_signal(&has_product);
		sleep(rand() % 5);
	}
}
int main(int argc, char *argv[])
{
	pthread_t pid, cid;
	srand(time(NULL));
	pthread_create(&pid, NULL, producer, NULL);
	pthread_create(&cid, NULL, consumer, NULL);
	pthread_join(pid, NULL);
	pthread_join(cid, NULL);
	return 0;
}