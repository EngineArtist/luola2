#ifndef LUOLA_RESOURCE_LOADER_H
#define LUOLA_RESOURCE_LOADER_H

#include <memory>

using std::string;

class ResourceLoaderImpl;
class DataFile;
class Resource;

/**
 * Load resources from data files.
 *
 * The resource loader takes a YAML resource description file as input.
 * Resources can then be loaded with the `load(const string&)` function. The
 * loaded resources will automatically be registered with the resource manager.
 *
 * <h1>Description file format</h1>
 * \verbatim
include: *list or scalar*  # list of extra resource description files to include
autoload: *list or scalar* # list of resources to automatically load
---
*resouce name*:
    type: *resource type*
    *options*
\endverbatim
 *
 * The first document of the description file is a metadata header. It can be
 * omitted. If used, however, the document separator --- must be used to indicate
 * the presence of the header!
 *
 * <h1>Resource types</h1>
 * <h2>Program</h2>
 * A shader program. This generates a ProgramResource.
 * The attribute "shaders" is a list of shaders to link to the program.
 *
 * Example:
 * \verbatim
myProgram:
    type: program
    shaders:
        - myVertexShader
        - myFragmentShader
\endverbatim
 *
 * <h2>Shader</h2>
 * A shader. This generates a ShaderResource.
 * The attribute "subtype" must be one of "vertex", "fragment" or "geometry".
 * The attribute "src" is the name of the file (inside the same resource
 * data file) from which the shader source code is read.
 *
 * Example:
 * \verbatim
myVertexShader:
    type: shader
    subtype: vertex
    src: vertex.glsl
\endverbatim
 *
 * <h2>Texture</h2>
 * This generates a TextureResource.
 * Currently only 2D textures loaded from PNG files are supported.
 * The attribute "src" is the name of the texture file.
 *
 * Example:
 * \verbatim
myTexture:
    type: texture
    src: image.png
\endverbatim
 *
 * <h2>Mesh</h2>
 * 3D vertex data. This generates a MeshResource.
 * The file format is described in MeshResource's documentation.
 * The attribute "src" is the name of the mesh data file. The
 * optional attributes "offset" and "scale" can be used to modify the mesh.
 * They take a vector of 1 or 3 elements which will be applied to the
 * vertex data when loaded.
 *
 * Example:
 * \verbatim
myMesh:
    type: mesh
    src: myMeshData.mesh
    offset: [1, 0, 0]
    scale: 0.5
\endverbatim
 *
 * <h2>Model</h2>
 * A 3D model. This wraps together a mesh, a shader program and zero or more
 * textures. A ModelResource is generated.
 * The attributes "mesh" and "shader" indicate the mesh and shader program to use.
 * The attribute "textures" is a list of texture references and sampler names.
 * The texture list may be omitted if no textures are used.
 *
 * Example:
 * \verbatim
myModel:
    type: model
    mesh: myMesh
    shader: myProgram
    textures:
        -texture: myTexture
         sampler: tex0

        -texture: mySecondTexture
         sampler: tex1
\endverbatim
 * The "sampler" attribute is the name of the sampler uniform used in the
 * fragment shader program to which the given texture will be bound.
 *
 * <h2>Font</h2>
 * A bitmap texture font. This generates a FontResource.
 * The attributes "texture" and "shader" reference the font texture and shader to
 * use. The attribute "description" names the font description XML file.
 *
 * Example:
 * \verbatim
myFont:
    type: font
    texture: myFontTexture
    description: font.xml
    shader: myFontShader
\endverbatim
 *
 */
class ResourceLoader {
public:
    ResourceLoader();

    /**
     * Construct a resource loader instance.
     *
     * @param datafile the data file from which the resources are loaded
     * @param filename the resource descriptor file
     */
    ResourceLoader(DataFile &datafile, const string& filename);

    /**
     * Load the named resource and all its dependancies from the datafile.
     *
     * The loaded resources will automatically be registered. If the resource
     * has already been registered, it will not be reloaded.
     * 
     * @param name name of the resource to load
     * @return resource instance or null if named resource not found.
     */
    Resource *load(const string& name);

private:
    std::shared_ptr<ResourceLoaderImpl> m_impl;
};

#endif

