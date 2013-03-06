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
#include <iostream>
#include <boost/algorithm/string.hpp>

#include "paths.h"

namespace bfs = boost::filesystem;
namespace fs {

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
    bool isDirectory(const bfs::path &path)
    {
        return bfs::exists(path) &&
            bfs::is_directory(path);
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
        bfs::path ed(extradata);
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
    for(const bfs::path &p : datapaths)
        std::cout << "\t" << p << "\n";
#endif

    PATHS = new Paths(datapaths);
    return true;
}

const Paths &Paths::get() {
    assert(PATHS);

    return *PATHS;
}

bfs::path Paths::findDataFile(const string& filename) const
{
    for(const bfs::path& dp : m_datapaths) {
        bfs::path p = dp / filename;
        if(bfs::exists(p))
            return p;
    }
    return bfs::path();
}

}

