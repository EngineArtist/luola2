#ifndef NDEBUG
#include <iostream>
using std::cerr;
using std::endl;
#endif

#include <yaml-cpp/yaml.h>

#include "../fs/datafile.h"
#include "resources.h"
#include "loader.h"
#include "shader.h"
#include "texture.h"
#include "mesh.h"
#include "model.h"
#include "font.h"

using std::unique_ptr;

class ResourceLoaderImpl {
public:
    ResourceLoaderImpl(DataFile &datafile, unique_ptr<YAML::Node> &node)
    : m_datafile(datafile), m_resources(std::move(node))
    { }

    void parseHeader(const YAML::Node &header);

    Resource *load(const string& name);

    Resource *loadProgram(const YAML::Node &node, const string &name);
    Resource *loadShader(const YAML::Node &node, const string &name);
    Resource *loadTexture(const YAML::Node &node, const string &name);
    Resource *loadMesh(const YAML::Node &node, const string &name);
    Resource *loadModel(const YAML::Node &node, const string &name);
    Resource *loadFont(const YAML::Node &node, const string &name);

    DataFile m_datafile;
    unique_ptr<YAML::Node> m_resources;
    std::vector<ResourceLoader> m_subresources;
};

ResourceLoader::ResourceLoader(DataFile& datafile, const string& filename)
{
    DataStream ds(datafile, filename);
    YAML::Parser parser(ds);

    // Try loading first document of the YAML file
    unique_ptr<YAML::Node> node1(new YAML::Node());
    if(!parser.GetNextDocument(*node1))
        throw ResourceException(datafile.name(), filename, "Not a YAML file!");

    // Both header and actual content are maps
    if(node1->Type() != YAML::NodeType::Map)
        throw ResourceException(datafile.name(), filename, "Resource file should contain a Map!");

    // Try loading second document of the YAML file.
    // If it is present, we know the first document was a header and the second
    // one is the actual content. If the second document is not present
    // the first document was the content.
    ResourceLoaderImpl *impl;
    unique_ptr<YAML::Node> node2(new YAML::Node());
    bool header;
    if(parser.GetNextDocument(*node2)) {
        if(node2->Type() != YAML::NodeType::Map && node2->Type() != YAML::NodeType::Null)
            throw ResourceException(datafile.name(), filename, "Resource content should be a Map!");
        impl = new ResourceLoaderImpl(datafile, node2);
        header = true;
    } else {
        impl = new ResourceLoaderImpl(datafile, node1);
        header = false;
    }
    ds.close();

    if(header)
        impl->parseHeader(*node1);

    m_impl = std::shared_ptr<ResourceLoaderImpl>(impl);
}

Resource *ResourceLoader::load(const string& name)
{
    if(!m_impl)
        throw ResourceException("", "", "no resource file loaded!");

    return m_impl->load(name);
}

std::vector<string> node2vec(const YAML::Node &node)
{
    std::vector<string> vec;
    if(node.Type() == YAML::NodeType::Scalar) {
        vec.push_back(node.to<string>());
    } else {
        for(const YAML::Node &n : node)
            vec.push_back(n.to<string>());
    }
    return vec;
}

void ResourceLoaderImpl::parseHeader(const YAML::Node &header)
{
    // Load subresources, if defined.
    const YAML::Node *includes = header.FindValue("include");
    if(includes) {
        std::vector<string> subresources = node2vec(*includes);
        for(const string &sr : subresources)
            m_subresources.push_back(ResourceLoader(m_datafile, sr));
    }

    // Autoload specified resources
    const YAML::Node *autoload = header.FindValue("autoload");
    if(autoload) {
        std::vector<string> loads = node2vec(*autoload);
        for(const string &al : loads)
            load(al);
    }
}

Resource *ResourceLoaderImpl::load(const string& name)
{
    // First step: see if the resource has already been loaded
    try {
        return Resources::getInstance().getResource(name);
    } catch(const ResourceNotFound &ex) {
        // not loaded yet
    }

    // See if the resource is defined here
    const YAML::Node *resnode;
    if(m_resources->Type() == YAML::NodeType::Map)
        resnode = m_resources->FindValue(name);
    else
        resnode = nullptr;

    if(!resnode) {
        // Try subresources if its not here
        for(ResourceLoader &sub : m_subresources) {
            try {
                return sub.load(name);
            } catch(const ResourceNotFound &ex2) {
                // We can get a NotFound exception for another
                // resource if a dependency is not found.
                if(ex2.resource() != name)
                    throw ex2;
            }
        }

        // Resource is not here or in any of the subresources
        throw ResourceNotFound(m_datafile.name(), name);
    }

    // Check resource type and dispatch to relevant handler
    string type;
    try {
        (*resnode)["type"] >> type;
    } catch(const YAML::KeyNotFound &ex) {
        throw ResourceException(m_datafile.name(), name, "missing resource type!");
    }

    if(type=="program")
        return loadProgram(*resnode, name);
    else if(type=="shader")
        return loadShader(*resnode, name);
    else if(type=="texture")
        return loadTexture(*resnode, name);
    else if(type=="mesh")
        return loadMesh(*resnode, name);
    else if(type=="model")
        return loadModel(*resnode, name);
    else if(type=="font")
        return loadFont(*resnode, name);
    else
        throw ResourceException(m_datafile.name(), name, "Unknown resource type: " + type);
}

Resource *ResourceLoaderImpl::loadProgram(const YAML::Node &node, const string &name)
{
    ProgramResource *pr = ProgramResource::make(name);

    for(const YAML::Node &n : node["shaders"]) {
        Resource *sr = load(n.to<string>());
        if(!sr) {
            Resources::getInstance().unloadResource(pr->name());
            return nullptr;
        } else if(!dynamic_cast<ShaderResource*>(sr)) {
            throw ResourceException(m_datafile.name(), sr->name(), "resource is not a shader!");
        }
        pr->addShader(static_cast<ShaderResource*>(sr));
    }

    pr->link();

    return pr;
}

Resource *ResourceLoaderImpl::loadModel(const YAML::Node &node, const string &name)
{
    Resource *mesh = load(node["mesh"].to<string>());
    if(mesh->type() != Resource::MESH)
        throw ResourceException(m_datafile.name(), name, mesh->name() + " is not a mesh!");

    Resource *shader = load(node["shader"].to<string>());
    if(shader->type() != Resource::SHADER_PROGRAM)
        throw ResourceException(m_datafile.name(), name, shader->name() + " is not a shader program!");

    ModelResource::SamplerTextures textures;
    if(const YAML::Node *texnode = node.FindValue("textures")) {
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

Resource *ResourceLoaderImpl::loadFont(const YAML::Node &node, const string &name)
{
    Resource *texture = load(node["texture"].to<string>());
    if(texture->type() != Resource::TEXTURE)
        throw ResourceException(m_datafile.name(), name, texture->name() + " is not a texture!");

    Resource *shader = load(node["shader"].to<string>());
    if(shader->type() != Resource::SHADER_PROGRAM)
        throw ResourceException(m_datafile.name(), name, shader->name() + " is not a shader program!");

    return FontResource::load(
        name,
        m_datafile,
        node["description"].to<string>(),
        static_cast<TextureResource*>(texture),
        static_cast<ProgramResource*>(shader));
}

Resource *ResourceLoaderImpl::loadShader(const YAML::Node &node, const string &name)
{
    string stype = node["subtype"].to<string>();
    Resource::Type type;
    if(stype == "vertex")
        type = Resource::VERTEX_SHADER;
    else if(stype == "fragment")
        type = Resource::FRAGMENT_SHADER;
    else if(stype == "geometry")
        type = Resource::GEOMETRY_SHADER;
    else
        throw ResourceException(m_datafile.name(), name, "Unrecognized shader type: " + stype);

    string src = node["src"].to<string>();

    return ShaderResource::load(name, m_datafile, src, type);
}

Resource *ResourceLoaderImpl::loadTexture(const YAML::Node &node, const string &name)
{
    string src = node["src"].to<string>();

    return TextureResource::load(name, m_datafile, src);
}

Resource *ResourceLoaderImpl::loadMesh(const YAML::Node &node, const string &name)
{
    string src = node["src"].to<string>();

    return MeshResource::load(name, m_datafile, src);
}
