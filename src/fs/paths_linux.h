#include <boost/algorithm/string.hpp>

namespace {

    /**
     * Data directory finder for Linux.
     * Finds data directories according to the XDG Base Directory Specification.
     *
     * Preferred data directory is the user's DATA_HOME
     * Rest of the directories come from XDG_DATA_DIRS.
     */
    PathVector platformDataPaths() {
        PathVector pv;

        // Default system data directories according to XDG spec.
        string data_dirs = getEnv("XDG_DATA_DIRS", "/usr/local/share");
        std::vector<string> datadirs;
        boost::split(datadirs, data_dirs, boost::is_any_of(":"));

        // Home data directory
        string home = getEnv("HOME");
        datadirs.insert(datadirs.begin(),
            getEnv("XDG_DATA_HOME", home + "/.local/share"));

        // Find our data directories
        for(const string& dir : datadirs) {
            path p = path(dir) / "luola2";
            if(isDirectory(p))
                pv.push_back(p);
        }

        return pv;
    }
}
