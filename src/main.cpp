#include <iostream>

#include <boost/program_options.hpp>

#include "fs/paths.h"
#include "util/threadpool.h"

using std::string;
using std::cout;
namespace po = boost::program_options;

boost::any threadTest()
{
    cout << "thread test!\n";
    return boost::any();
}

namespace {
    struct Args {
        bool help;
        string data;
        int threads;
    };

    Args getCmdlineArgs(int argc, char **argv)
    {
        po::options_description opts("Options");
        opts.add_options()
            ("help", "show this message")
            ("data", po::value<string>(), "data directory")
            ("threads", po::value<int>(), "number of background threads")
            ;

        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, opts), vm);
        po::notify(vm);

        Args args;
        args.help = vm.count("help");

        if(args.help) {
            cout << opts << "\n";
        }

        if(vm.count("data"))
            args.data = vm["data"].as<string>();

        if(vm.count("threads"))
            args.threads = vm["threads"].as<int>();
        else
            args.threads = 0;

        return args;
    }
}

int main(int argc, char **argv) {

    // Perform initializations
    {
        Args args = getCmdlineArgs(argc, argv);

        if(args.help)
            return 0;

        if(!Paths::init(args.data))
            return 1;

        ThreadPool::initSingleton(args.threads);
    }

    cout << "Hello world.\n";

    TaskFuture tf = ThreadPool::run(&threadTest);

    tf.wait();

    ThreadPool::shutdownSingleton();
    return 0;
}

