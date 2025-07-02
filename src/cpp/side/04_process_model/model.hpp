#ifndef MODEL_H
#define MODEL_H

#define TINYOBJLOADER_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <lodepng.h>
#include "stb_image.h"

#include "tinyobjloader/tiny_obj_loader.h"

#include "mesh.hpp"
#include "shader.hpp"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
using namespace std;

unsigned int loadTexture(char const * path);
unsigned char* load_png(const char* filename, int* width, int* height, int* nrComponents);
unsigned int TextureFromFile(char const * path);

class Model 
{
public:
    // model data 
    vector<Texture> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
    vector<Mesh>    meshes;
    string directory;
    bool gammaCorrection;

    // constructor, expects a filepath to a 3D model.
    Model(string const &path, bool gamma = false) : gammaCorrection(gamma)
    {
        loadModel(path);
    }

    // draws the model, and thus all its meshes
    void Draw(Shader &shader)
    {
        for(unsigned int i = 0; i < meshes.size(); i++)
            meshes[i].Draw(shader);
    }
    
private:
    // loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
    void loadModel(const std::string &path) {
        // Use TinyObjLoader to load the model
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn, err;

        bool success = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str(), directory.c_str());
        if (!warn.empty()) {
            std::cout << "WARNING: " << warn << std::endl;
        }
        if (!err.empty()) {
            std::cerr << "ERROR: " << err << std::endl;
        }
        if (!success) {
            std::cerr << "Failed to load OBJ file: " << path << std::endl;
            return;
        }

        // Retrieve the directory path of the file
        directory = path.substr(0, path.find_last_of('/'));

        // Process each shape (equivalent to nodes)
        for (size_t i = 0; i < shapes.size(); i++) {
            processShape(shapes[i], attrib, materials);
        }
    }

    void processShape(const tinyobj::shape_t &shape, const tinyobj::attrib_t &attrib, const std::vector<tinyobj::material_t> &materials) {
        // Data to fill
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<Texture> textures;

        // Reserve space for vertices and indices to avoid repeated reallocations
        size_t numIndices = shape.mesh.indices.size();
        vertices.reserve(numIndices); // Reserve space for vertices
        indices.reserve(numIndices);  // Reserve space for indices

        // Process vertices and indices
        for (size_t i = 0; i < numIndices; ++i) {
            const tinyobj::index_t &idx = shape.mesh.indices[i];

            Vertex vertex;
            glm::vec3 vector;

            // Positions
            vector.x = attrib.vertices[3 * idx.vertex_index + 0];
            vector.y = attrib.vertices[3 * idx.vertex_index + 1];
            vector.z = attrib.vertices[3 * idx.vertex_index + 2];
            vertex.Position = vector;

            // Normals
            if (idx.normal_index >= 0) {
                vector.x = attrib.normals[3 * idx.normal_index + 0];
                vector.y = attrib.normals[3 * idx.normal_index + 1];
                vector.z = attrib.normals[3 * idx.normal_index + 2];
                vertex.Normal = vector;
            } else {
                vertex.Normal = glm::vec3(0.0f, 0.0f, 0.0f); // Default normal
            }

            // Texture coordinates
            if (idx.texcoord_index >= 0) {
                vertex.TexCoords = glm::vec2(
                    attrib.texcoords[2 * idx.texcoord_index + 0],
                    attrib.texcoords[2 * idx.texcoord_index + 1]);
            } else {
                vertex.TexCoords = glm::vec2(0.0f, 0.0f);
            }

            vertices.push_back(vertex);
            indices.push_back(static_cast<unsigned int>(i));
        }

        // Process materials and textures once per shape (not inside the loop)
        if (!materials.empty() && shape.mesh.material_ids[0] >= 0) {
            const tinyobj::material_t &material = materials[shape.mesh.material_ids[0]];

            // Diffuse textures
            std::vector<Texture> diffuseMaps = loadMaterialTextures(material, "diffuse_texname", "texture_diffuse");
            textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

            // Specular textures
            std::vector<Texture> specularMaps = loadMaterialTextures(material, "specular_texname", "texture_specular");
            textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

            // Normal textures
            std::vector<Texture> normalMaps = loadMaterialTextures(material, "bump_texname", "texture_normal");
            textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

            // Height textures
            std::vector<Texture> heightMaps = loadMaterialTextures(material, "displacement_texname", "texture_height");
            textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
        }

        // Create and store the mesh object
        meshes.push_back(Mesh(vertices, indices, textures));
    }

    void processShape_slow(const tinyobj::shape_t &shape, const tinyobj::attrib_t &attrib, const std::vector<tinyobj::material_t> &materials) {
        // Data to fill
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<Texture> textures;

        // Process vertices and indices
        for (size_t i = 0; i < shape.mesh.indices.size(); i++) {
            tinyobj::index_t idx = shape.mesh.indices[i];

            Vertex vertex;
            glm::vec3 vector;

            // Positions
            vector.x = attrib.vertices[3 * idx.vertex_index + 0];
            vector.y = attrib.vertices[3 * idx.vertex_index + 1];
            vector.z = attrib.vertices[3 * idx.vertex_index + 2];
            vertex.Position = vector;

            // Normals
            if (idx.normal_index >= 0) {
                vector.x = attrib.normals[3 * idx.normal_index + 0];
                vector.y = attrib.normals[3 * idx.normal_index + 1];
                vector.z = attrib.normals[3 * idx.normal_index + 2];
                vertex.Normal = vector;
            } else {
                vertex.Normal = glm::vec3(0.0f, 0.0f, 0.0f); // Default normal
            }

            // Texture coordinates
            if (idx.texcoord_index >= 0) {
                vertex.TexCoords = glm::vec2(
                    attrib.texcoords[2 * idx.texcoord_index + 0],
                    attrib.texcoords[2 * idx.texcoord_index + 1]);
            } else {
                vertex.TexCoords = glm::vec2(0.0f, 0.0f);
            }

            vertices.push_back(vertex);
            indices.push_back(static_cast<unsigned int>(i));
        }

        // Process materials and textures
        if (!materials.empty() && shape.mesh.material_ids[0] >= 0) {
            const tinyobj::material_t &material = materials[shape.mesh.material_ids[0]];

            // Diffuse textures
            std::vector<Texture> diffuseMaps = loadMaterialTextures(material, "diffuse_texname", "texture_diffuse");
            textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

            // Specular textures
            std::vector<Texture> specularMaps = loadMaterialTextures(material, "specular_texname", "texture_specular");
            textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

            // Normal textures
            std::vector<Texture> normalMaps = loadMaterialTextures(material, "bump_texname", "texture_normal");
            textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

            // Height textures
            std::vector<Texture> heightMaps = loadMaterialTextures(material, "displacement_texname", "texture_height");
            textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
        }

        // Create and store the mesh object
        meshes.push_back(Mesh(vertices, indices, textures));
    }

    // checks all material textures of a given type and loads the textures if they're not loaded yet.
    // the required info is returned as a Texture struct.
    std::vector<Texture> loadMaterialTextures(const tinyobj::material_t &material, const std::string &textureField, const std::string &typeName) {
        std::vector<Texture> textures;

        // Map textureField to the corresponding material field
        std::string texturePath;
        if (textureField == "diffuse_texname") {
            texturePath = material.diffuse_texname;
        } else if (textureField == "specular_texname") {
            texturePath = material.specular_texname;
        } else if (textureField == "bump_texname") {
            texturePath = material.bump_texname;
        } else if (textureField == "displacement_texname") {
            texturePath = material.displacement_texname;
        }
        bool skip = false;
        for(unsigned int j = 0; j < textures_loaded.size(); j++)
        {
            if(textures_loaded[j].path == texturePath)
            {
                textures.push_back(textures_loaded[j]);
                skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
                break;
            }
        }

        // Add the texture if the path is not empty
        if (!texturePath.empty() && !skip) {
            Texture texture;
            texture.id = TextureFromFile(texturePath.c_str());
            // texture.path = directory + "/" + texturePath; // Full path to the texture
            texture.path = texturePath; // Full path to the texture
            texture.type = typeName;
            textures.push_back(texture);
            textures_loaded.push_back(texture);
        }

        return textures;
    }
};


// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int TextureFromFile(char const * path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    // unsigned char *data = load_png(path, &width, &height, &nrComponents);
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        delete[] data;
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        delete[] data;
    }

    return textureID;
}

unsigned char* load_png(const char* filename, int* width, int* height, int* nrComponents) {
    std::vector<unsigned char> image;  // Pixel data storage
    unsigned int w, h;                 // Image dimensions

    // Decode the PNG
    unsigned int error = lodepng::decode(image, w, h, filename);
    if (error) {
        std::cerr << "Error loading PNG: " << lodepng_error_text(error) << std::endl;
        return nullptr;
    }

    // Return image dimensions and number of components
    *width = w;
    *height = h;
    *nrComponents = (unsigned int)(image.size() / (w * h));

    // Allocate and return data
    unsigned char* data = new unsigned char[image.size()];
    std::copy(image.begin(), image.end(), data);
    return data;
}

#endif