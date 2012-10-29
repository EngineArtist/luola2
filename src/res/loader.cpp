#include <yaml-cpp/yaml.h>

#include "loader.h"
#include "shader.h"
#include "texture.h"
#include "mesh.h"
#include "model.h"
#include "font.h"

ResourceLoader::ResourceLoader(DataFile& datafile, const string& filename)
    : m_datafile(datafile)
{
    DataStream ds(datafile, filename);
    YAML::Parser parser(ds);

    if(!parser.GetNextDocument(m_resources))
        throw ResourceException(datafile.name(), filename, "Not a YAML file!");

    if(m_resources.Type() != YAML::NodeType::Map)
        throw ResourceException(datafile.name(), filename, "Resource file should contain a Map!");
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
        m_resources[name]["type"] >> type;
    } catch(const YAML::KeyNotFound &ex) {
        throw ResourceException(m_datafile.name(), name, "Resource \"" + name + "\" not found!");
    }

    if(type=="program")
        return loadProgram(name);
    else if(type=="shader")
        return loadShader(name);
    else if(type=="texture")
        return loadTexture(name);
    else if(type=="mesh")
        return loadMesh(name);
    else if(type=="model")
        return loadModel(name);
    else if(type=="font")
        return loadFont(name);
    else
        throw ResourceException(m_datafile.name(), name, "Unknown resource type: " + type);
}

Resource *ResourceLoader::loadProgram(const string& name)
{
    ProgramResource *pr = ProgramResource::make(name);

    for(const YAML::Node &n : m_resources[name]["shaders"]) {
        Resource *sr = load(n.to<string>());
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

Resource *ResourceLoader::loadModel(const string& name)
{
    const YAML::Node &m = m_resources[name];

    Resource *mesh = load(m["mesh"].to<string>());
    if(mesh->type() != Resource::MESH)
        throw ResourceException(m_datafile.name(), name, mesh->name() + " is not a mesh!");

    Resource *shader = load(m["shader"].to<string>());
    if(shader->type() != Resource::SHADER_PROGRAM)
        throw ResourceException(m_datafile.name(), name, shader->name() + " is not a shader program!");

    ModelResource::SamplerTextures textures;
    if(const YAML::Node *texnode = m.FindValue("textures")) {
        for(const YAML::Node &n : *texnode) {
            Resource *tr = load(n["texture"].to<string>());
            if(tr->type() != Resource::TEXTURE)
                throw ResourceException(m_datafile.name(), name, shader->name() + " is not a texture!");

            textures.push_back(ModelResource::SamplerTexture(
                n["sampler"].to<string>(),
                static_cast<TextureResource*>(tr)
                ));
        }
    }

    return ModelResource::make(
        name,
        static_cast<MeshResource*>(mesh),
        static_cast<ProgramResource*>(shader),
        textures);
}

Resource *ResourceLoader::loadFont(const string& name)
{
    const YAML::Node &m = m_resources[name];

    Resource *texture = load(m["texture"].to<string>());
    if(texture->type() != Resource::TEXTURE)
        throw ResourceException(m_datafile.name(), name, texture->name() + " is not a texture!");

    Resource *shader = load(m["shader"].to<string>());
    if(shader->type() != Resource::SHADER_PROGRAM)
        throw ResourceException(m_datafile.name(), name, shader->name() + " is not a shader program!");

    return FontResource::load(
        name,
        m_datafile,
        m["description"].to<string>(),
        static_cast<TextureResource*>(texture),
        static_cast<ProgramResource*>(shader));
}

Resource *ResourceLoader::loadShader(const string& name)
{
    string stype = m_resources[name]["subtype"].to<string>();
    Resource::Type type;
    if(stype == "vertex")
        type = Resource::VERTEX_SHADER;
    else if(stype == "fragment")
        type = Resource::FRAGMENT_SHADER;
    else if(stype == "geometry")
        type = Resource::GEOMETRY_SHADER;
    else
        throw ResourceException(m_datafile.name(), name, "Unrecognized shader type: " + stype);

    string src = m_resources[name]["src"].to<string>();

    return ShaderResource::load(name, m_datafile, src, type);
}

Resource *ResourceLoader::loadTexture(const string& name)
{
    string src = m_resources[name]["src"].to<string>();

    return TextureResource::load(name, m_datafile, src);
}

Resource *ResourceLoader::loadMesh(const string& name)
{
    string src = m_resources[name]["src"].to<string>();

    return MeshResource::load(name, m_datafile, src);
}
