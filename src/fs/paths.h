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
#ifndef LUOLA_FILESYSTEM_H
#define LUOLA_FILESYSTEM_H

#include <boost/filesystem.hpp>
#include <vector>
#include <unordered_set>

using std::string;

namespace fs {

typedef std::vector<boost::filesystem::path> PathVector;

/**
 * Data directory path lookups.
 */
class Paths {
public:
    Paths() = delete;

    /**
     * Initialize paths.
     * 
     * @param extradata 
     * @return true if all required paths were found
     */
    static bool init(const string& extradata);

    /**
     * Get the Paths singleton.
     * 
     * init() must have been called first.
     * 
     * @return Paths instance
     */
    static const Paths& get();
 
    /**
     * Get the paths to data directories.
     * The list of paths should be searched in order for the desired
     * data files.
     * 
     * @return vector of paths
     */
    const PathVector& datapaths() const { return m_datapaths; }

    /**
     * Find all files from all data directories that end with the given pattern.
     * 
     * @param pattern the string matched filenames must end with (e.g. ".level")
     */
    std::unordered_set<string> dataglob(const string &pattern) const;

    /**
     * Search the data directories for a file.
     * 
     * @return full path of file or invalid path if not found.
     */
    boost::filesystem::path findDataFile(const string& filename) const;

private:
    Paths(const PathVector &datapaths);
    PathVector m_datapaths;
};

}

#endif

