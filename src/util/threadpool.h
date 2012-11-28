//
// This file is part of Luola2.
// Copyright (C) 2012 Calle Laakkonen
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
#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <queue>
#include <boost/any.hpp>
#include <boost/thread.hpp>

typedef std::function<boost::any()> TaskFunction;
typedef boost::packaged_task<boost::any> Task;
typedef boost::unique_future<boost::any> TaskFuture;

/**
 * A thread pool.
 *
 * You will typically use this through the singleton instance, by using
 * the static ThreadPool::run() function.
 *
 */
class ThreadPool {
    public:
        /**
         * Construct a new thread pool with the given number of threads.
         *
         * \param threads number of threads. Must be greater than zero.
         */
        ThreadPool(int threads);

        /**
         * Destruct the pool and stop all threads.
         *
         * All running threads are joined.
         */
        ~ThreadPool();

        /**
         * Enqueue a task to be executed in a thread.
         *
         * \param fn the functor to execute
         * \return a future of the functors return value
         */
        TaskFuture enqueue(TaskFunction &&fn);

        /**
         * Initialize the singleton pool.
         * The pool will contain at least two threads, or more
         * if more processors are available.
		 *
		 * @param threads number of threads. If zero or below, the number is determined automatically.
         */
        static void initSingleton(int threads=0);

        /**
         * Enqueue a task to be executed on the Singleton pool.
         *
         * initSingleton() must have been called before.
         */
        static TaskFuture run(TaskFunction &&fn);

        /**
         * Shut down the singleton thread pool.
         */
        static void shutdownSingleton();

    private:
        boost::thread_group m_threads;
        std::queue<Task> m_tasks;
        boost::mutex m_taskmutex;
        boost::condition_variable m_taskcond;

        bool m_runflag;

        void workerFunc();
};

#endif

