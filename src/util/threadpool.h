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

