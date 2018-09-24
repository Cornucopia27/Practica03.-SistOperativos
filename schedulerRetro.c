#include <scheduler.h>
#include <stdio.h>

extern THANDLER threads[MAXTHREAD];
extern int currthread;
extern int blockevent;
extern int unblockevent;

#define TIME_SLICE 1

QUEUE ready;
QUEUE waitinginevent[MAXTHREAD];

int chooseThread(int th);

struct thread_priorities{
	int id;
	int priority;
	int callingthread;
};

struct thread_priorities usedThreads[10];

int contador = 0;
int bandera = 1;

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
		usedThreads[contador].id = threads[callingthread].tid;
		usedThreads[contador].callingthread = callingthread;
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
				chosen = chooseThread(threads[callingthread].tid);
				
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

	if((usedThreads[0].priority == usedThreads[1].priority) && (usedThreads[0].priority == usedThreads[2].priority)){
		chosenOne = usedThreads[0].callingthread;
		if(usedThreads[0].priority=!0)
			usedThreads[0].priority = usedThreads[0].priority = usedThreads[0].priority++;
	}
	else if((usedThreads[0].priority > usedThreads[1].priority)&&(usedThreads[1].priority == usedThreads[2].priority)){
		chosenOne = usedThreads[1].callingthread;
		if(usedThreads[1].priority=!0)
			usedThreads[1].priority = usedThreads[1].priority++;
	}
	else if((usedThreads[1].priority > usedThreads[2].priority) && (usedThreads[2].priority < usedThreads[0].priority)){
		chosenOne = usedThreads[2].callingthread;
		usedThreads[2].priority = usedThreads[2].priority++;
	}
	
		
	


	
	if(th == usedThreads[1].id && usedThreads[0].priority==0)
			usedThreads[0].priority = usedThreads[0].priority++;
			
	else if(th == usedThreads[2].id && usedThreads[1].priority==0)
			usedThreads[1].priority = usedThreads[1].priority++;
				
	return chosenOne;

}
