#ifndef LUOLA_RESOURCES_H
#define LUOLA_RESOURCES_H

#include <vector>
#include <unordered_map>
#include <string>
#include <exception>

using std::string;

/**
 * Resource loading exception
 */
class ResourceException : public std::exception
{
public:
    ResourceException(const string& datafile, const string& resource, const string& error);
    ~ResourceException() throw();

    /**
     * Get the datafile related to the error
     * 
     * @return data file name
     */
    const string& datafile() const { return m_datafile; }

    /**
     * Get the name of the resource related to the error
     * 
     * @return resource name
     */
    const string& resource() const { return m_resource; }

    /**
     * Get the error string
     * 
     * @return error string
     */
    const string& error() const { return m_error; }

    const char *what() const throw();

    friend std::ostream& operator<<(std::ostream&, const ResourceException&);
 
private:
    string m_datafile;
    string m_resource;
    string m_error;
};

/**
 * Resource not found exception.
 */
class ResourceNotFound : public ResourceException
{
public:
    ResourceNotFound(const string& datafile, const string& resource)
    : ResourceException(datafile, resource, "resource \"" + resource + "\" not found!")
    {}
};

/**
 * Base class for resource types
 */
class Resource {
public:
    enum Type {
        VERTEX_SHADER,
        GEOMETRY_SHADER,
        FRAGMENT_SHADER,
        SHADER_PROGRAM,
        TEXTURE,
        MESH,
        MODEL,
        FONT
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
    static Resources &getInstance();

    /**
     * Get the named resource of the specific type
     *
     * @param name resource name
     * @return resource
     * @throw ResourceException if resource is not found or is the wrong type
     */
    template <class restype>
    static restype *get(const string& name)
    {
        Resource *res = getInstance().getResource(name);
        restype *r = dynamic_cast<restype*>(res);
        if(!r)
            throw ResourceException("", name, "wrong resource type!");
        return r;
    }

    /**
     * Register a new resource.
     *
     * @param resource the resource to registe
     * @throw ResourceException if resource with the same name is already registered
     */
    void registerResource(Resource *resource);


    /**
     * Get the named resource
     * 
     * @param name resource name
     * @return resource
     * @throw ResourceNotFound if not found
     */
    Resource *getResource(const string &name)
    {
        if(!m_resources.count(name))
            throw ResourceNotFound("", name);
        return m_resources[name];
    }

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
