#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <queue>
#include <semaphore.h>
#include <iostream>


sem_t empty, full, mutex;

using namespace std;

class myqueue{
	queue<int> stlqueue;
public:
	void push(int sock){
			 
	cout<<"in push ";
		sem_wait(&empty);
		sem_wait(&mutex);
		stlqueue.push(sock);
		sem_post(&mutex);
		sem_post(&empty);
	}
	int pop (){
		 sem_wait(&full);
		 sem_wait(&mutex);
		int rval = stlqueue.front();
		stlqueue.pop();
	
		 sem_post(&mutex);
		 sem_post(&empty);
		return(rval);

	}

} sockqueue;

void *serve(void *arg){
	int tid;
	tid = (long)arg;
	printf("Hi %d\n", tid);
	for(;;){
		cout<< "GOT: "<<sockqueue.pop()<<endl;
	}
}


int main(){
#define NQUEUE 20
#define NTHREADS 10
	long threadid;
	pthread_t threads[NTHREADS];
	cout<<"before push";
	sem_init(&empty,0,NQUEUE);
	sem_init(&full,0,0);
	sem_init(&mutex,0,1);	

	for(int i = 0; i<NQUEUE; i++){
		cout<<"in push";
		sockqueue.push(i);
	}
cout<<"ended";
//	for(int i = 0; i<NQUEUE; i++){
//		cout<< "GOT: "<<sockqueue.pop()<<endl;
//	}	

	//exit(0);
	 for(threadid = 0; threadid <NTHREADS; threadid++){
	 //	cout<<"what is ";
		pthread_create(&threads[threadid],NULL,serve,(void *)threadid);
	 }


}

