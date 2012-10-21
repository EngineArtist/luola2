#include <iostream>

#include <boost/property_tree/json_parser.hpp>

#include "loader.h"
#include "shader.h"

ResourceLoader::ResourceLoader(DataFile& datafile, const string& filename)
    : m_datafile(datafile)
{
    DataStream ds(datafile, filename);
    boost::property_tree::read_json(ds, m_resources);
}

Resource *ResourceLoader::load(const string& name)
{
    // First step: see if the resource has already been loaded
    Resource *res = Resources::get().getResource(name);

    if(res)
        return res;

    // If not, load it from the datafile
    string type;
    try {
        type = m_resources.get<string>(name + ".type");
    } catch(const boost::property_tree::ptree_bad_data& ex) {
        std::cerr << "Warning: resource \"" << name << "\" not found!" << std::endl;
        return nullptr;
    }

    if(type=="program")
        return loadProgram(name);
    if(type=="shader")
        return loadShader(name);
    else {
        std::cerr << "Warning: unknown resource type \"" << type << "\" for resource \""
            << name << "\"!" << std::endl;
        return nullptr;
    }
}

Resource *ResourceLoader::loadProgram(const string& name)
{
    ProgramResource *pr = ProgramResource::make(name);

    for(const boost::property_tree::ptree::value_type &pt : m_resources.get_child(name + ".shaders")) {
        Resource *sr = load(pt.second.get_value<string>());
        if(!sr) {
            Resources::get().unloadResource(pr->name());
            return nullptr;
        } else if(!sr->isShader()) {
            std::cerr << "Resource \"" << sr->name() << "is not a shader!" << std::endl;
            Resources::get().unloadResource(pr->name());
            return nullptr;
        }
        pr->addShader(static_cast<ShaderResource*>(sr));
    }
    if(!pr->link()) {
        Resources::get().unloadResource(pr->name());
        return nullptr;
    }

    return pr;
}

Resource *ResourceLoader::loadShader(const string& name)
{
    string stype = m_resources.get<string>(name + ".subtype");
    Resource::Type type;
    if(stype == "vertex")
        type = Resource::VERTEX_SHADER;
    else if(stype == "fragment")
        type = Resource::FRAGMENT_SHADER;
    else {
        std::cerr << "unrecognized shader type: " << stype << std::endl;
        return nullptr;
    }

    string src = m_resources.get<string>(name + ".src");

    return ShaderResource::load(name, m_datafile, src, type);
}
