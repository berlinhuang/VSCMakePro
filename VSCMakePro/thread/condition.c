

//��������һ��ͻ�����һ��ʹ��
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
struct msg {
	struct msg *next;
	int num;
};
struct msg *head;
pthread_cond_t has_product = PTHREAD_COND_INITIALIZER;//�������� ��̬��ʼ��
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;//������

void *consumer(void *p)
{
	struct msg *mp;
	for (;;) {
		pthread_mutex_lock(&lock);//1.��ȡ��������Ϊ�˵���pthread_cond_wait()
		//signal���ܻἤ�����߳�(�����������������߳�)���������ٻ���
		while (head == NULL)//ʹ��while ֱ���߳̽���pthread_cond_wait
			pthread_cond_wait(&has_product, &lock);
		//2.�ͷŻ�����lock��pthread_mutex_unlock(&lock) �����������޷���ȡ������
		//pthread_cond_wait(&has_product, &lock) �����ȴ�has_product ������
		//3.��������ʱ�������������������ȥ�����û����� pthread_mutex_lock(&lock)Ȼ�󷵻�,��ȡ����������
		mp = head;
		head = mp->next;
		pthread_mutex_unlock(&lock);//4.�������ͷŻ�����
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
		//����pthread_cond_signal()�ͷŻ�����
		//
		pthread_mutex_unlock(&lock);
		//����һ���ȴ����߳�
		//pthread_cond_broacast ��ỽ�ѵȴ����������е��̣߳���ʱ��������
		//���Է���pthread_mutex_lock() ��pthread_mutex_unlock()֮�� Ҳ���Է���֮���и�����ȱ��
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