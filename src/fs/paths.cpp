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
#include <iostream>
#include <boost/algorithm/string.hpp>

#include "paths.h"

namespace fs {

using boost::filesystem::path;
using std::string;

namespace {
    //! Paths singleton
    Paths *PATHS;

    /**
     * Get an environment variable
     * 
     * @param name the variable name
     * @param def default value
     * @return variable value or default value if undefined
     */
    string getEnv(const char *name, const string& def=string())
    {
    char *str = getenv(name);
    if(str)
        return str;
    return def;
    }

    /**
     * Check if path exists and is a directory
     */
    bool isDirectory(const path &path)
    {
        return boost::filesystem::exists(path) &&
            boost::filesystem::is_directory(path);
    }
}

// TODO ifdef platform specific paths here
#include "paths_linux.h"

Paths::Paths(const PathVector &datapaths)
    : m_datapaths(datapaths)
{
}

bool Paths::init(const string& extradata)
{
    PathVector datapaths = platformDataPaths();

    if(!extradata.empty()) {
        path ed(extradata);
        if(boost::filesystem::exists(ed) && boost::filesystem::is_directory(ed))
            datapaths.insert(datapaths.begin(), ed);
        else
            std::cerr << "Warning: " << extradata << " does not exist!\n";
    }

    if(datapaths.empty()) {
        std::cerr << "Error: No data directories found!\n";
        return false;
    }

#ifndef NDEBUG
    std::cout << "Found data directories:\n";
    for(const path &p : datapaths)
        std::cout << "\t" << p << "\n";
#endif

    PATHS = new Paths(datapaths);
    return true;
}

const Paths &Paths::get() {
    assert(PATHS);

    return *PATHS;
}

path Paths::findDataFile(const string& filename) const
{
    for(const path& dp : m_datapaths) {
        path p = dp / filename;
        if(boost::filesystem::exists(p))
            return p;
    }
    return path();
}

}
