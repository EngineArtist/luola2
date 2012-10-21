#include <iostream>
#include <cstdlib>

#include <boost/program_options.hpp>

#include <GL/glew.h>
#include <GL/glfw.h>

#include "fs/paths.h"
#include "util/threadpool.h"
#include "game.h"

using std::string;
using std::cout;
using std::cerr;

namespace {
    namespace po = boost::program_options;
    struct Args {
        bool help;
        string data;
        int threads;
        int width, height;
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

        args.width = 800;
        args.height = 600;

        return args;
    }

    bool initOpengl(int winWidth, int winHeight)
    {
        if(!glfwInit()) {
            cerr << "Couldn't initialize GLFW!\n";
            return false;
        }

        // Antialiasing: 4x
        glfwOpenWindowHint(GLFW_FSAA_SAMPLES, 4);

        // Use OpenGL 3.2 core profile
        glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 3);
        glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 2);
        glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
 
        // Open a window and create its OpenGL context
        if(!glfwOpenWindow(winWidth, winHeight, 0, 0, 0, 0, 32, 0, GLFW_WINDOW )) {
            cerr << "Couldn't open GLFW window!\n";
            glfwTerminate();
            return false;
        }
        atexit(&glfwTerminate);

        // Initialize OpenGL extension wrangler
        glewExperimental = GL_TRUE;
        if (glewInit() != GLEW_OK) {
            cerr << "Couldn't initialize GLEW!\n";
            return false;
        }
 
        glfwSetWindowTitle("Luola 2.0");
        return true;
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

        if(!initOpengl(args.width, args.height))
            return 1;

        ThreadPool::initSingleton(args.threads);
        atexit(&ThreadPool::shutdownSingleton);
    }

    // Run the game
    gameloop();
 
    return 0;
}
