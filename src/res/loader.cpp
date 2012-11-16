#include "resources.h"
#include "loader.h"
#include "shader.h"
#include "texture.h"
#include "mesh.h"
#include "model.h"
#include "font.h"

namespace {
/**
 * Convenience function: Convert a conftree node to a string vector
 *
 * The node must be either a Scalar or a List.
 * If the node is a scalar, a vector with just one
 * element will be returned.
 */
std::vector<string> node2vec(const conftree::Node &node)
{
    std::vector<string> vec;
    if(node.type() == conftree::Node::SCALAR) {
        vec.push_back(node.value());

    } else {
        for(unsigned int i=0;i<node.items();++i)
            vec.push_back(node.at(i).value());
    }
    return vec;
}

/**
 * Convenience function: Convert a conftree node to a three component
 * float vector.
 *
 * The node must be either a Scalar or a list.
 * If the node is a scalar, the same value is assigned to every
 * component. If the node is a List, its length must be
 * either 1 or 3.
 */
glm::vec3 node2vec3(const conftree::Node &node, const glm::vec3 &def=glm::vec3(0.0f))
{
    if(node.type() == conftree::Node::BLANK) {
        return def;

    } else if(node.type() == conftree::Node::SCALAR) {
        return glm::vec3(node.floatValue());

    } else {
        if(node.items() == 1)
            return glm::vec3(node.at(0).floatValue());

        else if(node.items() == 3)
            return glm::vec3(
                node.at(0).floatValue(),
                node.at(1).floatValue(),
                node.at(2).floatValue()
                );
        else
            throw ResourceException("", "", "List must have 1 or 3 elements!");
    }
}
}

void ResourceLoader::parseHeader(DataFile &datafile, const conftree::Node &header)
{
    // Load subresources, if defined.
    conftree::Node includes = header.opt("include");
    if(includes.type() != conftree::Node::BLANK) {
        std::vector<string> subresources = node2vec(includes);
        for(const string &sr : subresources) {
            ResourceLoader subloader(datafile, sr);
            for(const string &key : subloader.m_node.itemSet())
                m_node.insert(key, subloader.m_node.at(key));

        }
    }

    // Autoload specified resources
    conftree::Node autoloads = header.opt("autoload");
    if(autoloads.type() != conftree::Node::BLANK) {
        std::vector<string> loads = node2vec(autoloads);
        for(const string &al : loads)
            load(al);
    }
}

ResourceLoader::ResourceLoader(DataFile& datafile, const string& filename)
    : m_datafile(datafile)
{
    // Load configuration file
    std::vector<conftree::Node> nodes = conftree::parseMultiDocYAML(datafile, filename);

    // Sanity checks
    if(nodes.empty())
        throw ResourceException(datafile.name(), filename, "Not a YAML file!");

    if(nodes.size() > 2)
        throw ResourceException(datafile.name(), filename, "Resource file should contain at most two documents!");

    if(nodes[0].type() != conftree::Node::MAP)
        throw ResourceException(datafile.name(), filename, "Resource file should contain a Map!");

    if(nodes.size() > 1) {
        // Header present
        m_node = nodes[1];
        parseHeader(datafile, nodes[0]);
    } else {
        // No header
        m_node = nodes[0];
    }
}

Resource *ResourceLoader::load(const string& name)
{
    // First step: see if the resource has already been loaded
    try {
        return Resources::getInstance().getResource(name);
    } catch(const ResourceNotFound &ex) {
        // not loaded yet
    }

    conftree::Node resnode;
    try {
        resnode = m_node.at(name);
    } catch(const conftree::BadNode &ex) {
        throw ResourceNotFound(m_datafile.name(), name);
    }

    // Check resource type and dispatch to relevant handler
    string type;
    try {
        type = resnode.at("type").value();
    } catch(const conftree::BadNode &ex) {
        throw ResourceException(m_datafile.name(), name, "missing resource type!");
    }

    if(type=="program")
        return loadProgram(resnode, name);
    else if(type=="shader")
        return loadShader(resnode, name);
    else if(type=="texture")
        return loadTexture(resnode, name);
    else if(type=="mesh")
        return loadMesh(resnode, name);
    else if(type=="model")
        return loadModel(resnode, name);
    else if(type=="font")
        return loadFont(resnode, name);
    else
        throw ResourceException(m_datafile.name(), name, "Unknown resource type: " + type);
}

Resource *ResourceLoader::loadProgram(const conftree::Node &node, const string &name)
{
    ProgramResource *pr = ProgramResource::make(name);

    const conftree::Node &shaders = node.at("shaders");
    for(unsigned int i=0;i<shaders.items();++i) {
        const conftree:: Node &n = shaders.at(i);

        Resource *sr = load(n.value());
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

Resource *ResourceLoader::loadModel(const conftree::Node &node, const string &name)
{
    Resource *mesh = load(node.at("mesh").value());
    if(mesh->type() != Resource::MESH)
        throw ResourceException(m_datafile.name(), name, mesh->name() + " is not a mesh!");

    Resource *shader = load(node.at("shader").value());;
    if(shader->type() != Resource::SHADER_PROGRAM)
        throw ResourceException(m_datafile.name(), name, shader->name() + " is not a shader program!");

    ModelResource::SamplerTextures textures;
    conftree::Node texnodes = node.opt("textures");
    for(unsigned int i=0;i<texnodes.items();++i) {
        const conftree::Node &n = texnodes.at(i);

        Resource *tr = load(n.at("texture").value());
        if(tr->type() != Resource::TEXTURE)
            throw ResourceException(m_datafile.name(), name, shader->name() + " is not a texture!");

        textures.push_back(ModelResource::SamplerTexture(
            n.at("sampler").value(),
            static_cast<TextureResource*>(tr)
            ));
    }

    return ModelResource::make(
        name,
        static_cast<MeshResource*>(mesh),
        static_cast<ProgramResource*>(shader),
        textures);
}

Resource *ResourceLoader::loadFont(const conftree::Node &node, const string &name)
{
    Resource *texture = load(node.at("texture").value());
    if(texture->type() != Resource::TEXTURE)
        throw ResourceException(m_datafile.name(), name, texture->name() + " is not a texture!");

    Resource *shader = load(node.at("shader").value());
    if(shader->type() != Resource::SHADER_PROGRAM)
        throw ResourceException(m_datafile.name(), name, shader->name() + " is not a shader program!");

    return FontResource::load(
        name,
        m_datafile,
        node.at("description").value(),
        static_cast<TextureResource*>(texture),
        static_cast<ProgramResource*>(shader));
}

Resource *ResourceLoader::loadShader(const conftree::Node &node, const string &name)
{
    string stype = node.at("subtype").value();
    Resource::Type type;
    if(stype == "vertex")
        type = Resource::VERTEX_SHADER;
    else if(stype == "fragment")
        type = Resource::FRAGMENT_SHADER;
    else if(stype == "geometry")
        type = Resource::GEOMETRY_SHADER;
    else
        throw ResourceException(m_datafile.name(), name, "Unrecognized shader type: " + stype);

    return ShaderResource::load(
        name,
        m_datafile,
        node.at("src").value(),
        type
        );
}

Resource *ResourceLoader::loadTexture(const conftree::Node &node, const string &name)
{
    return TextureResource::load(
        name,
        m_datafile,
        node.at("src").value()
        );
}

Resource *ResourceLoader::loadMesh(const conftree::Node &node, const string &name)
{
    string src = node.at("src").value();
    glm::vec3 offset = node2vec3(node.opt("offset"));
    glm::vec3 scale = node2vec3(node.opt("scale"), glm::vec3(1.0f));

    return MeshResource::load(
        name,
        m_datafile,
        src,
        offset,
        scale
        );
}
