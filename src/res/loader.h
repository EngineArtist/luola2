#ifndef LUOLA_RESOURCE_LOADER_H
#define LUOLA_RESOURCE_LOADER_H

#include <yaml-cpp/node.h>

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
    Resource *loadTexture(const string& name);
    Resource *loadMesh(const string& name);
    Resource *loadModel(const string& name);
    Resource *loadFont(const string& name);

    DataFile m_datafile;
    YAML::Node m_resources;
};

#endif

