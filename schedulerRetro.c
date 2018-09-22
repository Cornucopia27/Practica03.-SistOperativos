#include <scheduler.h>

extern THANDLER threads[MAXTHREAD];
extern int currthread;
extern int blockevent;
extern int unblockevent;

#define TIME_SLICE 1

QUEUE ready;
QUEUE waitinginevent[MAXTHREAD];

int chooseThread(int th);

typedef struct{
	int id;
	int priority;
	bool flag = false;//para identificar cual sigue
}thread_priorities

struct thread_priorities usedThreads[10];

int contador = 0;

int time_slice = 0;

void scheduler(int arguments)
{
	int old,next;
	int changethread=0;
	int waitingthread=0;

	int event=arguments & 0xFF00;
	int callingthread=arguments & 0xFF;
	
	int chosen = 0;

	if(event==NEWTHREAD)
	{
		// Un nuevo hilo va a la cola de listos
		threads[callingthread].status=READY;
		usedThreads[contador].id = callingthread;//threads[callingthread].tid;
		usedThreads[contador].priority = 0;
		contador++;
		_enqueue(&ready,callingthread);
	}

	if(event==BLOCKTHREAD)
	{

		threads[callingthread].status=BLOCKED;
		_enqueue(&waitinginevent[blockevent],callingthread);

		changethread=1;
	}

	if(event==ENDTHREAD)
	{
		threads[callingthread].status=END;
		changethread=1;
	}

	if(event==UNBLOCKTHREAD)
	{
			threads[callingthread].status=READY;
			_enqueue(&ready,callingthread);
	}

	if(event==TIMER)
	{
			time_slice++;
			if(time_slice==TIME_SLICE)
			{
				chosen = chooseThread(callingthread);
				
				_enqueue(&ready,chosen);
				changethread=1;
				time_slice = 0;
			}
	}

	if(changethread)
	{
		old=currthread;
		next=_dequeue(&ready);

		threads[next].status=RUNNING;
		_swapthreads(old,next);
	}

}

int chooseThread(int th){
	int chosenOne = 0;

	if(th == usedThreads[0].id){
		usedThreads[0].flag = true;
		if(usedThreads[1].flag == true || usedThreads[0].priority == usedThreads[2].priority ){
			usedThreads[0].priority = usedThreads[0].priority++;
		{	
	}
	else if(th == usedThreads[1].id){
		usedThreads[1].flag = true;
		if(usedThreads[2].flag == true || usedThreads[1].priority == usedThreads[2].priority ){
			usedThreads[1].priority = usedThreads[1].priority++;
		{		
	}
	else if(th == usedThreads[2].id){
		usedThreads[2].flag = true;
		usedThreads[2].priority = usedThreads[2].priority++;
	}
	
	if((usedThreads[0].priority > usedThreads[1].priority) && (usedThreads[1].priority < usedThreads[2].priority))
		chosenOne = usedThreads[1].id;
	else if((usedThreads[1].priority > usedThreads[2].priority) && (usedThreads[2].priority < usedThreads[1].priority))
		chosenOne = usedThreads[2].id;
	else chosenOne = usedThreads[0].id;

	usedThreads[0].flag = false; usedThreads[1].flag = false; usedThreads[2].flag = false;	

	return chosenOne;

}
