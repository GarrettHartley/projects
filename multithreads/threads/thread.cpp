#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <queue>
#include <semaphore.h>
#include <iostream>


sem_t full, empty, mutex;

using namespace std;

class myqueue{
	queue<int> stlqueue;
public:
	void push(int sock){
			 
//	cout<<"in push ";
		sem_wait(&full);
		sem_wait(&mutex);
		stlqueue.push(sock);
		sem_post(&mutex);
		sem_post(&full);
	}
	int pop (){
		sem_wait(&empty);
		sem_wait(&mutex);
		int rval = stlqueue.front();
		stlqueue.pop();
	
		sem_post(&mutex);
		sem_post(&full);
		return(rval);

	}

} sockqueue;

void *serve(void *arg){

	cout<<"inside the serve function";
	int tid;
	tid = (long)arg;
	printf("Hi %d\n", tid);
	for(;;){

// serve files 
		cout<< "GOT: "<<sockqueue.pop()<<endl;




		
	}
}


int main(){
#define NQUEUE 20
#define NTHREADS 10
	long threadid;
	pthread_t threads[NTHREADS];
	
	sem_init(&full,0,NQUEUE);
	sem_init(&empty,0,0);
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

