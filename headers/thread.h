#pragma once

enum
{
	TF_NONE = 0,
	TF_REQUEUE			= BIT(1),		// move to back of queue after finishing, instead of popping
	TF_PRIORITY			= BIT(2),		// prioritize this job and move it to the front of the queue
	TF_CONCURRENT		= BIT(3),		// put subsequent jobs with TF_CONCURRENT into the same thread's job queue until no other TF_CONCURRENT is found
	TF_BREAKCONCURRENT	= BIT(4),		// break from TF_CONCURRENT to end concurrency. if enabled with TF_CONCURRENT, will create concurrency in a different thread's job queue

	TF_WAIT				= BIT(5),		// for standalone threads, start the thread asleep and wait until notified to launch
	TF_FORCEJOIN		= BIT(6),		// call join() after starting the task to force it to wait
};

class Thread
{
public:
	typedef std::function<void()> job_t;

	Thread();
	Thread::Thread(job_t task, int flags = 0);
	~Thread();


	void AddJob( job_t job, int flags = 0, int wait_msec = 0 );
	inline void SetMaxJobs(int x) { max_jobs = x; }
	bool IsActive() const { return active; }
	void Entry();
	void Launch();

private:
	std::unique_ptr<std::thread>    thread;
	std::queue<int>					threadflags;
	std::queue<int>					threadwait;
	std::condition_variable         queuePending;
	std::mutex                      queueMutex;
	std::list<job_t>                jobQueue;
	bool                            wantExit;
	bool							active;
	bool							independent;
	int								max_jobs;

	Thread(const Thread&);              // no copying!
	Thread& operator = (const Thread&); // no copying!

	friend class ThreadPool;
};

class ThreadPool
{
public:
	typedef std::function<void()> job_t;

	static void Init();
	static int GetActiveThreadCount();
	static int GetMaxThreads() { return max_threads; }
	static void SetMaxThreads(int val);

	static Thread *Spawn();
	static void SpawnThreads();
	static void QueueTask(job_t task, int flags = 0, int wait_msec = 0);

private:
	static inline bool _CompareTaskQueue(Thread *left, Thread *right)
	{
		return left->jobQueue.size() < right->jobQueue.size();
	}

	static int max_threads;
	static std::vector<Thread *> Threads;

	static Thread *lastThreadToQueue;

	ThreadPool();
	ThreadPool(const ThreadPool &) {};
	~ThreadPool();
};