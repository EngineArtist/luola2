#ifndef LUOLA_RESOURCE_LOADER_H
#define LUOLA_RESOURCE_LOADER_H

#include <boost/property_tree/ptree.hpp>

#include "../fs/datafile.h"
#include "resources.h"

using std::string;

class ResourceLoader {
public:
    /**
     * Construct a resource loader instance.
     *
     * @param datafile the data file from which the resources are loaded
     * @param filename the resource descriptor file
     */
    ResourceLoader(DataFile &datafile, const string& filename);

    /**
     * Load the named resource and all its dependancies from the datafile.
     *
     * The loaded resources will automatically be registered. If the resource
     * has already been registered, it will not be reloaded.
     * 
     * @param name name of the resource to load
     * @return resource instance or null if named resource not found.
     */
    Resource *load(const string& name);

private:
    Resource *loadProgram(const string& name);
    Resource *loadShader(const string& name);

    DataFile m_datafile;
    boost::property_tree::ptree m_resources;
};

#endif

