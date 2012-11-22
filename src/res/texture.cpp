#ifndef NDEBUG
#include <iostream>
using std::cerr;
using std::endl;
#endif

#include <GL/glew.h>
#include <png.h>

#include "texture.h"
#include "../fs/datafile.h"

namespace resource {

namespace {
    struct Image {
        unsigned char *data;
        unsigned int width;
        unsigned int height;
        bool alpha;
    };

    Image loadPng(fs::DataFile &df, const string& filename);
}

Texture *Texture::load(const string& name, fs::DataFile &datafile, const string& filename)
{
#ifndef NDEBUG
    cerr << "Loading texture " << filename << "..." << endl;
#endif

    Image img = loadPng(datafile, filename);

    GLuint id;
    glGenTextures(1, &id);

    glBindTexture(GL_TEXTURE_2D, id);
    GLint fmt = img.alpha ? GL_RGBA : GL_RGB;
    glTexImage2D(GL_TEXTURE_2D, 0, fmt, img.width, img.height, 0,
                 fmt, GL_UNSIGNED_BYTE,
                 img.data);

    delete[] img.data;

    // TODO adjustable parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    Texture *res = new Texture(name, id, img.width, img.height, GL_TEXTURE_2D);
    Resources::getInstance().registerResource(res);

    return res;
}

Texture::Texture(const string& name, GLuint id, int width, int height, GLenum target)
    : Resource(name, TEXTURE), m_id(id), m_width(width), m_height(height), m_target(target)
{

}

Texture::~Texture()
{
    glDeleteTextures(1, &m_id);
}

namespace {

void pngReadDsCallback(png_structp pngPtr, png_bytep data, png_size_t length)
{
    fs::DataStream *ds = static_cast<fs::DataStream*>(png_get_io_ptr(pngPtr));

    ds->read(reinterpret_cast<char*>(data), length);
}

Image loadPng(fs::DataFile &df, const string& filename)
{
    Image img;

    // Open image file
    fs::DataStream ds(df, filename);
    if(ds->isError())
        throw ResourceException(df.name(), filename, "PNG open error");

    // Main PNG reader
    png_structp pngPtr = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
    if(!pngPtr)
        throw ResourceException(df.name(), filename, "PNG init error");

    // PNG info
    png_infop pngInfoPtr = png_create_info_struct(pngPtr);
    if(!pngInfoPtr) {
        png_destroy_read_struct(&pngPtr, 0, 0);
        throw ResourceException(df.name(), filename, "PNG init error");
    }

    // Error handling
    if(setjmp(png_jmpbuf(pngPtr))) {
        png_destroy_read_struct(&pngPtr, &pngInfoPtr, 0);
        throw ResourceException(df.name(), filename, "PNG read error");
    }

    // Set up IO
    png_set_read_fn(pngPtr, &ds, pngReadDsCallback);

    // Read image header
    png_read_info(pngPtr, pngInfoPtr);

    img.width = png_get_image_width(pngPtr, pngInfoPtr);
    img.height = png_get_image_height(pngPtr, pngInfoPtr);
    int colorType = png_get_color_type(pngPtr, pngInfoPtr);
    int bitDepth = png_get_bit_depth(pngPtr, pngInfoPtr);

    // Make sure bith depth is 8 bits per channel
    if(bitDepth < 8)
        png_set_packing(pngPtr);
    else if(bitDepth == 16)
        png_set_strip_16(pngPtr);

    // We only support RGB/RGBA images
    switch(colorType) {
        case PNG_COLOR_TYPE_PALETTE:
            png_set_palette_to_rgb(pngPtr);
            break;
        case PNG_COLOR_TYPE_GRAY:
        case PNG_COLOR_TYPE_GRAY_ALPHA:
            png_set_gray_to_rgb(pngPtr);
            break;
        default: break;
    }

    img.alpha = (colorType == PNG_COLOR_TYPE_GRAY_ALPHA || colorType == PNG_COLOR_TYPE_RGBA);
    png_read_update_info(pngPtr, pngInfoPtr);

    // Read data
    int channels = png_get_channels(pngPtr, pngInfoPtr);
    img.data = new unsigned char[img.width * img.height * channels];
 
    png_bytep *rowPointers = new png_bytep[img.height];
 
    unsigned int row;
    for(row = 0; row < img.height; ++row) {
        rowPointers[row] = (png_bytep)img.data + (row * img.width * channels);
    }
 
    png_read_image(pngPtr, rowPointers);
    png_read_end(pngPtr, 0);
 
    png_destroy_read_struct(&pngPtr, &pngInfoPtr, 0);
    delete[] rowPointers;

    return img;
}

}

}
