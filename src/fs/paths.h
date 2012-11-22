#ifndef LUOLA_FILESYSTEM_H
#define LUOLA_FILESYSTEM_H

#include <boost/filesystem.hpp>
#include <vector>

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
    static bool init(const std::string& extradata);

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
     * Search the data directories for a file.
     * 
     * @return full path of file or invalid path if not found.
     */
    boost::filesystem::path findDataFile(const std::string& filename) const;

private:
    Paths(const PathVector &datapaths);
    PathVector m_datapaths;
};

}

#endif

