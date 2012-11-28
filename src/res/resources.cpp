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
#include <ostream>

#include "resources.h"

namespace resource {

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

}
