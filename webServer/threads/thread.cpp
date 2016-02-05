#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <queue>
#include <semaphore.h>
#include <iostream>


sem_t on_q, to_do, mutex;

using namespace std;

class myqueue{
	queue<int> stlqueue;
public:
	void push(int sock){
			 
//	cout<<"in push ";
		sem_wait(&on_q);
		sem_wait(&mutex);
		stlqueue.push(sock);
		sem_post(&mutex);
		sem_post(&on_q);
	}
	int pop (){
cout<<"pop ";
		 sem_wait(&to_do);
		 sem_wait(&mutex);
		int rval = stlqueue.front();
		stlqueue.pop();
	
		 sem_post(&mutex);
		 sem_post(&on_q);
		return(rval);

	}

} sockqueue;

void *serve(void *arg){
	cout<<"inside the serve function";
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
	
	sem_init(&on_q,0,NQUEUE);
	sem_init(&to_do,0,0);
	sem_init(&mutex,0,1);	

	for(int i = 0; i<NQUEUE; i++){
//		cout<<"in push";
		sockqueue.push(i);
	}
//cout<<"ended";
//	for(int i = 0; i<NQUEUE; i++){
//		cout<< "GOT: "<<sockqueue.pop()<<endl;
//	}	

	//exit(0);
	 for(threadid = 0; threadid <NTHREADS; threadid++){
	 	cout<<"should create a thread"<<endl;
		pthread_create(&threads[threadid],0,serve,(void *)&threadid);	
 }
sleep(5);

}

