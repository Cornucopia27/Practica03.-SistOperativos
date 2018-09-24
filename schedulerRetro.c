#include <scheduler.h>
#include <stdio.h>

extern THANDLER threads[MAXTHREAD];
extern int currthread;
extern int blockevent;
extern int unblockevent;

#define TIME_SLICE 1
#define PRIORITIES 10

QUEUE ready;
QUEUE waitinginevent[MAXTHREAD];

QUEUE PR_QUEUE[PRIORITIES];

int time_slice = 0;
int thread_num = 0;
int threads_priorities[PRIORITIES];

int init = 1;

void scheduler(int arguments)
{
	int old,next;
	int changethread=0;
	int waitingthread=0;

	int event=arguments & 0xFF00;
	int callingthread=arguments & 0xFF;

	if(init == 1)
	{
		int init_counter;
		init--;
		for(init_counter = 0; init_counter<PRIORITIES; init_counter++)
		{
			PR_QUEUE[init_counter].head = 0;
			PR_QUEUE[init_counter].tail = 0;
			threads_priorities[init_counter] = 0;
		}
	}

	if(event==NEWTHREAD)
	{
		// Un nuevo hilo va a la cola de listos
		threads[callingthread].status=READY;
		threads_priorities[callingthread] = 0;
		thread_num++;
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
		thread_num--;
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
					if(thread_num == 1)
					{
						threads_priorities[callingthread] = 0;
					}
					if(threads_priorities[callingthread] < PRIORITIES)
					{
						threads_priorities[callingthread]++;
					}
						_enqueue(&PR_QUEUE[threads_priorities[callingthread]], callingthread);

					int contador, index = 0;
					for(contador = PRIORITIES-1; contador>=0; contador--)
					{
						if((threads_priorities[contador] <= threads_priorities[index] && (threads[contador].status == READY || threads[contador].status == RUNNING)))
						{
							index = contador;
						}
					}

					_enqueue(&ready,_dequeue(&PR_QUEUE[threads_priorities[index]]));
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
