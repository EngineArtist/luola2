#include <iostream>

#include "util/threadpool.h"

boost::any threadTest()
{
    std::cout << "thread test!\n";
    return boost::any();
}

int main(int argc, char **argv) {
    ThreadPool::initSingleton();

    std::cout << "Hello world.\n";

    TaskFuture tf = ThreadPool::run(&threadTest);

    tf.wait();

    ThreadPool::shutdownSingleton();
    return 0;
}

