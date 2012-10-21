#include "threadpool.h"

#ifndef NDEBUG
#include <iostream>
#endif

static ThreadPool *SINGLETON;

void ThreadPool::initSingleton(int threads)
{
    assert(SINGLETON == nullptr);
	int t;
	if(threads<1) {
		t = boost::thread::hardware_concurrency();
		if(t<2)
			t = 2;
	} else {
		t = threads;
	}

#ifndef NDEBUG
    std::cerr << "Starting thread pool singleton with " << t << " threads." << std::endl;
#endif
    SINGLETON = new ThreadPool(t);
}

TaskFuture ThreadPool::run(TaskFunction &&fn)
{
    assert(SINGLETON != nullptr);
    return SINGLETON->enqueue(std::move(fn));
}

void ThreadPool::shutdownSingleton()
{
    delete SINGLETON;
    SINGLETON = 0;
}

ThreadPool::ThreadPool(int threads)
{
    m_runflag = true;
    for(int i=0;i<threads;++i)
        m_threads.add_thread(new boost::thread(&ThreadPool::workerFunc, this));
}

ThreadPool::~ThreadPool()
{
    m_runflag = false;
    m_taskcond.notify_all();
    m_threads.join_all();
}

TaskFuture ThreadPool::enqueue(TaskFunction &&fn)
{
    boost::unique_lock<boost::mutex> lock(m_taskmutex);
    m_tasks.push(Task(std::move(fn)));
    m_taskcond.notify_one();
    return m_tasks.back().get_future();
}

void ThreadPool::workerFunc()
{
    while(m_runflag) {
        Task t;

        {
            boost::unique_lock<boost::mutex> lock(m_taskmutex);
            if(m_tasks.empty()) {
                m_taskcond.wait(lock);
            }

            if(!m_tasks.empty()) {
                t = std::move(m_tasks.front());
                m_tasks.pop();
            } else {
                continue;
            }
        }

        t();
    }
}

