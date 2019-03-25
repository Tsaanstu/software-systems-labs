#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

//потоковая функция



void* first_thread(void* thread_data){
	sleep(4);
	printf("1\n");
	pthread_exit(0);
}

void* second_thread(void* thread_data){
	sleep(2);
	printf("2\n");
	pthread_exit(0);
}

int main(int argc, char **argv) {

	//какие то данные для потока (для примера)
	void* thread_data = NULL;

	//создаем идентификатор потока
	pthread_t f_thread, s_thread;

	//создаем поток по идентификатору thread и функции потока threadFunc
	//и передаем потоку указатель на данные thread_data
	pthread_create(&f_thread, NULL, first_thread, thread_data);
	pthread_create(&s_thread, NULL, second_thread, thread_data);

	printf("main\n");

	//ждем завершения потока

	pthread_join(f_thread, NULL);
	pthread_join(s_thread, NULL);

	return 0;
}
