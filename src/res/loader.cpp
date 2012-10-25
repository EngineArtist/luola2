#include <boost/property_tree/json_parser.hpp>

#include "loader.h"
#include "shader.h"
#include "texture.h"
#include "mesh.h"

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
        throw ResourceException(m_datafile.name(), name, "Resource not found");
    }

    if(type=="program")
        return loadProgram(name);
    else if(type=="shader")
        return loadShader(name);
    else if(type=="texture")
        return loadTexture(name);
    else if(type=="mesh")
        return loadMesh(name);
    else
        throw ResourceException(m_datafile.name(), name, "Unknown resource type: " + type);
}

Resource *ResourceLoader::loadProgram(const string& name)
{
    ProgramResource *pr = ProgramResource::make(name);

    for(const auto &pt : m_resources.get_child(name + ".shaders")) {
        Resource *sr = load(pt.second.get_value<string>());
        if(!sr) {
            Resources::get().unloadResource(pr->name());
            return nullptr;
        } else if(!dynamic_cast<ShaderResource*>(sr)) {
            throw ResourceException(m_datafile.name(), sr->name(), "resource is not a shader!");
        }
        pr->addShader(static_cast<ShaderResource*>(sr));
    }

    pr->link();

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
        throw ResourceException(m_datafile.name(), name, "Unrecognized shader type: " + stype);
    }

    string src = m_resources.get<string>(name + ".src");

    return ShaderResource::load(name, m_datafile, src, type);
}

Resource *ResourceLoader::loadTexture(const string& name)
{
    string src = m_resources.get<string>(name + ".src");

    return TextureResource::load(name, m_datafile, src);
}

Resource *ResourceLoader::loadMesh(const string& name)
{
    string src = m_resources.get<string>(name + ".src");

    return MeshResource::load(name, m_datafile, src);
}
