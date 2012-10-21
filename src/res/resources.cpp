#include <iostream>
#include <stdexcept>
#include "resources.h"

namespace {
    Resources *RESOURCES;
}

Resources::Resources()
{
}

Resources &Resources::get()
{
    if(!RESOURCES)
        RESOURCES = new Resources();

    return *RESOURCES;
}

bool Resources::registerResource(Resource *resource)
{
    if(m_resources.count(resource->name())>0) {
        std::cerr << "Warning: tried to register resource \"" <<
            resource->name() << "\" which is already registered.\n";
        return false;
    } else {
        m_resources[resource->name()] = resource;
        return true;
    }
}

Resource *Resources::getResource(const string& name)
{
    try {
        return m_resources.at(name);
    } catch(const std::out_of_range& ex) {
        return nullptr;
    }
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
#ifndef NDEBUG
    std::cout << "Unloading resource " << m_name << std::endl;
#endif
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

bool Resource::isShader() const
{
    switch(m_type) {
        case VERTEX_SHADER:
        case FRAGMENT_SHADER:
            return true;
        default: return false;
    }
}
