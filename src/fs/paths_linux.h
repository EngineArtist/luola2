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
