#ifndef LUOLA_RESOURCES_H
#define LUOLA_RESOURCES_H

#include <vector>
#include <unordered_map>
#include <string>

using std::string;

/**
 * Base class for resource types
 */
class Resource {
public:
    enum Type {
        VERTEX_SHADER,
        FRAGMENT_SHADER,
        SHADER_PROGRAM
    };

    Resource(const string& name, Type type);
    virtual ~Resource();

    /**
     * Get the type of this resource
     *
     * @return resource type
     */
    Type type() const { return m_type; }

    /**
     * Is this a shader type resource
     *
     * @return true if resource type is one of the _SHADERs
     */
    bool isShader() const;

    /**
     * Get the name of the resource
     *
     * @return resource name
     */
    const string& name() const { return m_name; }

protected:
    /**
     * Add a resource to this resource's dependency list.
     *
     * This increments the reference count of the dependency.
     * When this resource is deleted, those resources whose
     * reference count is zero will also be deleted.
     *
     * @param dep dependency
     */
    void addDependency(Resource *dep);

private:
    Type m_type;

    string m_name;
    std::vector<Resource*> m_deps;
    int m_refcount;
};

/**
 * OpenGL resource manager.
 *
 * This class is in charge of loading and managing OpenGL resources such
 * as models, textures and shaders.
 */
class Resources {
    friend class Resource;
public:
    Resources(const Resources& r) = delete;

    /**
     * Get the resource manager singleton instance.
     * @return resource manager
     */
    static Resources &get();

    /**
     * Register a new resource.
     *
     * If a resource with the same name has already been registered,
     * the new resource will not be registered and false will be returned.
     * 
     * @param name resource name
     * @param resource the resource to registe
     * @return true if resource was registered
     */
    bool registerResource(Resource *resource);

    /**
     * Get the named resource
     *
     * @param name resource name
     * @return resource or nullptr if not found
     */
    Resource *getResource(const string& name);

    /**
     * Remove the named resource.
     *
     * @param name resource name
     */
    void unloadResource(const string& name);

private:
    Resources();

    std::unordered_map<string, Resource*> m_resources;
};

#endif

