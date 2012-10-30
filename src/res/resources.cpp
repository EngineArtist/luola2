#include <ostream>

#include "resources.h"

namespace {
    Resources *RESOURCES;
}

Resources::Resources()
{
}

Resources &Resources::getInstance()
{
    if(!RESOURCES)
        RESOURCES = new Resources();

    return *RESOURCES;
}

void Resources::registerResource(Resource *resource)
{
    if(m_resources.count(resource->name())>0)
        throw ResourceException("", resource->name(), "named resource already registered");

    m_resources[resource->name()] = resource;
}

void Resources::unloadResource(const string& name)
{
    Resource *r = getResource(name);
    if(r) {
        m_resources.erase(name);
        delete r;
    }
}

Resource::Resource(const string& name, Type type)
    : m_type(type), m_name(name), m_refcount(0)
{
}

Resource::~Resource()
{
    for(Resource *res : m_deps) {
        if(--res->m_refcount==0) {
            RESOURCES->unloadResource(res->m_name);
        }
    }
}

void Resource::addDependency(Resource *dep)
{
    dep->m_refcount++;
    m_deps.push_back(dep);
}

ResourceException::ResourceException(const string& datafile, const string& resource, const string& error)
    : m_datafile(datafile), m_resource(resource), m_error(error)
{
}

ResourceException::~ResourceException() throw()
{
}

const char *ResourceException::what() const throw()
{
    return m_error.c_str();
}

std::ostream &operator<<(std::ostream &os, const ResourceException &ex)
{
    os << "Resource loading exception (" << ex.datafile() << "/" << ex.resource()
        << "): " << ex.error();
    return os;
}
