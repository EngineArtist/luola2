//
// This file is part of Luola2.
//
// Luola2 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Luola2 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Luola2.  If not, see <http://www.gnu.org/licenses/>.
//
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

