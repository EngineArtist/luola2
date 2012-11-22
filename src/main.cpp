#include <iostream>
#include <cstdlib>

#include <boost/program_options.hpp>

#include <GL/glew.h>
#include <GL/glfw.h>

#include "fs/paths.h"
#include "fs/datafile.h"
#include "util/threadpool.h"
#include "util/conftree.h"
#include "res/loader.h"

#include "gameinit.h"
#include "ship/shipdef.h"
#include "ship/engine.h"
#include "ship/power.h"
#include "equipment/equipment.h"
#include "weapon/weapon.h"
#include "projectile/projectiledef.h"

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
        string gamefile;

        string launchfile;
    };

    Args getCmdlineArgs(int argc, char **argv)
    {
        po::options_description opts("Options");
        opts.add_options()
            ("help", "show this message")
            ("data", po::value<string>(), "data directory")
            ("game", po::value<string>(), "game file (default: game.data)")
            ("threads", po::value<int>(), "number of background threads")
            ("launch", po::value<string>(), "quicklaunch file")
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

        if(vm.count("game"))
            args.gamefile = vm["game"].as<string>();
        else
            args.gamefile = "game.data";

        if(vm.count("launch"))
            args.launchfile = vm["launch"].as<string>();

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
 
		// Enable v-sync
		//glfwSwapInterval(1);

        glfwSetWindowTitle("Luola 2.0");
        return true;
    }

    bool loadGame(const string &gamefile)
    {
        DataFile df(gamefile);

        // Load resources
        {
            ResourceLoader rl(df, "resources.yaml");
        }

        // Load game configuration
        conftree::Node gameconf = conftree::parseYAML(df, "game.yaml");

        string title = gameconf.opt("title", conftree::Node("Luola 2.0")).value();
        glfwSetWindowTitle(title.c_str());

        // Load ship components
        conftree::Node ship = gameconf.at("ship");
        ShipDefs::loadAll(df, ship.at("hulls").value());
        Engines::loadAll(df, ship.at("engines").value());
        PowerPlants::loadAll(df, ship.at("power").value());
        Equipments::loadAll(df, ship.at("equipment").value());
        Projectiles::loadAll(df, ship.at("projectiles").value());
        Weapons::loadAll(df, ship.at("weapons").value());

        return true;
    }
}

int main(int argc, char **argv) {

    // Perform initializations
    gameinit::Hotseat launcher;
    {
        Args args = getCmdlineArgs(argc, argv);

        if(args.help)
            return 0;

        if(!Paths::init(args.data))
            return 1;

        if(!initOpengl(args.width, args.height))
            return 1;

        if(!loadGame(args.gamefile))
            return 1;

        if(args.launchfile.length() == 0) {
            cerr << "Game menu system not yet implemented! Use --launch <file> to start the game!\n";
            return 1;
        }
        launcher = gameinit::Hotseat::loadFromFile(args.launchfile);
 
        ThreadPool::initSingleton(args.threads);
        atexit(&ThreadPool::shutdownSingleton);
    }

    // Run the game
    gameloop(launcher);
 
    return 0;
}
