#include "libs.h"
#include "util.h"
#include "static.h"
#include "thread.h"

int ThreadPool::max_threads = 0;
std::vector<Thread *> ThreadPool::Threads = std::vector<Thread *>();

Thread *ThreadPool::lastThreadToQueue = nullptr;

Thread::Thread()
{
	wantExit = false;
	independent = false;
	thread = std::unique_ptr<std::thread>( new std::thread( std::bind(&Thread::Entry, this) ) );
}

// independent constructor (basically allows this to replace std::thread
Thread::Thread(job_t task, int flags)
{
	wantExit = false;
	independent = true;
	active = false;
	job_t job([&] () {
		if(flags & TF_WAIT)
		{
			std::unique_lock<std::mutex> lock(queueMutex);
			queuePending.wait( lock, [&] () { return active; } );
		}
		task();
	});
	thread = std::unique_ptr<std::thread>( new std::thread( job ) );
	//thread = std::unique_ptr<std::thread>( new std::thread( std::bind(&Thread::Entry, this) ) );
	//AddJob(task, flags);
	if(flags & TF_FORCEJOIN)
		thread->join();
	//wantExit = true;
}

Thread::~Thread()
{
	{
		std::lock_guard<std::mutex> lock(queueMutex);
		wantExit = true;
		queuePending.notify_one();
	}
	if(thread->joinable()) thread->join();
}

void Thread::AddJob( job_t job, int flags, int wait_msec )
{
	if(jobQueue.size() >= max_jobs && max_jobs > 0)
		return;
	std::lock_guard<std::mutex> lock(queueMutex);
	jobQueue.push_back(job);
	threadflags.push(flags);
	threadwait.push(wait_msec);

	// priority jobs get shuffled to the front of the queue to execute asap
	// however, they are not immediately run until the thread can do it!
	// do note that queuing multiple priority jobs will cause the last
	// job with priority to go first
	// I could fix this but I'm lazy, so maybe later if it's a problem
	if(flags & TF_PRIORITY && jobQueue.size() > 1)
	{
		for(int i = 0; i < jobQueue.size() - 1; i++)
		{
			jobQueue.push_back(jobQueue.front());
			threadflags.push(threadflags.front());
			threadwait.push(threadwait.front());

			jobQueue.pop_front();
			threadflags.pop();
			threadwait.pop();
		}
	}
	queuePending.notify_one();
}

void Thread::Entry()
{
	job_t job;
	int CurrentFlags = 0;
	int msec = 0;

	while(true)
	{
		{
			active = false;
			std::unique_lock<std::mutex> lock(queueMutex);
			queuePending.wait( lock, [&] () { return wantExit || !jobQueue.empty(); } );

			if(wantExit)
				return;

			active = true;
			job = jobQueue.front();
			CurrentFlags = threadflags.front();
			msec = threadwait.front();

			if(CurrentFlags & TF_REQUEUE)
			{
				jobQueue.push_back(jobQueue.front());
				threadflags.push(threadflags.front());
				threadwait.push(threadwait.front());
			}

			jobQueue.pop_front();
			threadflags.pop();
			threadwait.pop();
		}

		job();
		active = false;

		//if(msec > 0)
		std::this_thread::sleep_for(std::chrono::milliseconds(msec));
	}
}

void Thread::Launch()
{
	if(!independent)
		return;

	queuePending.notify_one();
}

void ThreadPool::SetMaxThreads( int val )
{
#if _WIN32
	SYSTEM_INFO sysinfo;
	GetSystemInfo(&sysinfo);
	max_threads = val;
	max_threads = Clamp(max_threads, 1, (int) sysinfo.dwNumberOfProcessors);
#else
	max_threads = val;
#endif
}

int ThreadPool::GetActiveThreadCount()
{
	int r = 0;
	for(int i = 0; i < Threads.size(); i++)
	{
		if(Threads.at(i)->IsActive()) r++;
	}

	return r;
}

Thread * ThreadPool::Spawn()
{
	if(Threads.size() < max_threads || max_threads == 0)
		Threads.push_back(new Thread);
	return Threads.back();
}

void ThreadPool::QueueTask( job_t task, int flags, int wait_msec )
{
	if(Threads.empty())
	{
		task();			// ensure the task runs even if no multithreading available
		return;
	}

	// concurrent but don't break for a new one -> queue to last thread
	if(lastThreadToQueue != nullptr && (flags & TF_CONCURRENT && !(flags & TF_BREAKCONCURRENT)))
	{
		lastThreadToQueue->AddJob(task, flags, wait_msec);
	}
	else
	{
		std::vector<Thread *>::iterator it = std::min_element(Threads.begin(), Threads.end(), _CompareTaskQueue);
		lastThreadToQueue = (*it);
		(*it)->AddJob(task, flags, wait_msec);
	}
}

ThreadPool::ThreadPool()
{
}

void ThreadPool::SpawnThreads()
{
	for(int i = 0; i < GetMaxThreads(); i++)
	{
		Threads.push_back(new Thread());
	}
}

ThreadPool::~ThreadPool()
{
	std::for_each(Threads.begin(), Threads.end(), delete_ptr<Thread>());
	Threads.clear();
}

void ThreadPool::Init()
{
	SetMaxThreads(8);
	Threads.reserve(GetMaxThreads());
	SpawnThreads();
}
