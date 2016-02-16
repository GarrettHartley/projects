#include <iostream>
#include <unistd.h>
#include <wait.h>
using namespace std;
main(){

	int pid = fork();
	cout<<"Fork Returned " <<pid<<endl;
	if(pid == 0){
	cout<<"Child about to xec "<<endl;	
	execl("/bin/ls","/bin/ls", (char *)0);
	}else{
	int status;
	cout<<"Parent stuff "<<cout;
	wait(&status);
}

}
