#ifndef MODEL_H
#define MODEL_H

#include <glad/glad.h> 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include"stb_image.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <Windows.h>

#include "mesh.h"
#include "shader.h"
#include"maze.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
using namespace std;

enum ModelMovement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

float MODELYAW = 90.0f;
float MODELSCALE = 1.0f;
float MODELMOVESPEED = 0.5f;
float MODELROTATESPEED = 0.7f;

unsigned int TextureFromFile(const char* path, const string& directory, bool gamma = false);

// 处理中文路径
string UTF8ToGB(const char* str)
{
    string result;
    WCHAR* strSrc;
    LPSTR szRes;

    //获得临时变量的大小
    int i = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
    strSrc = new WCHAR[i + 1];
    MultiByteToWideChar(CP_UTF8, 0, str, -1, strSrc, i);

    //获得临时变量的大小
    i = WideCharToMultiByte(CP_ACP, 0, strSrc, -1, NULL, 0, NULL, NULL);
    szRes = new CHAR[i + 1];
    WideCharToMultiByte(CP_ACP, 0, strSrc, -1, szRes, i, NULL, NULL);

    result = szRes;
    delete[]strSrc;
    delete[]szRes;

    return result;
}

class Model
{
public:
    // model data 
    vector<Texture> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
    vector<Mesh>    meshes;
    string directory;
    bool gammaCorrection;

    // 模型参数
    glm::vec3 Position;
    glm::vec3 Front;
    float Yaw;
    float Scale;
    glm::mat4 ModelMat;
    
    // 变化速度
    float MoveSpeed;
    float RotateSpeed;

    Model() {}

    // constructor, expects a filepath to a 3D model.
    Model(string const& path, bool gamma = false) : gammaCorrection(gamma), MoveSpeed(MODELMOVESPEED), RotateSpeed(MODELROTATESPEED)
    {
        loadModel(path);
        countModelBoundingBox();
        Yaw = MODELYAW;
        Scale = MODELSCALE;
        Position = glm::vec3(0.0f, 0.0f, 0.0f);
        Center = glm::vec3(boxPoints[1].x + 0.5 * Width, boxPoints[1].y + 0.5 * Height, boxPoints[1].z + 0.5 * Thickness);
        Front = glm::vec3(0.0f, 0.0f, 1.0f);
        SetModelTransformation(Position, Yaw, Scale);
    }

    // draws the model, and thus all its meshes
    void Draw(Shader& shader)
    {
        for (unsigned int i = 0; i < meshes.size(); i++)
            meshes[i].Draw(shader);
    }

    glm::vec3 getOriginalCenter()
    {
        return Center;
    }

    glm::vec3 getChangedCenter()
    {
        glm::vec4 changedCenter = glm::vec4(Center, 1.0f);
        changedCenter = ModelMat * changedCenter;
        return changedCenter;
    }

    std::vector<glm::vec3> getOriginalBoxPoints()
    {
        return boxPoints;
    }

    std::vector<glm::vec3> getChangedBoxPoints()
    {
        std::vector<glm::vec3> changedPoints;
        for (int i = 0; i < 4; ++i) {
            glm::vec4 changedPoint = glm::vec4(boxPoints[i], 1.0f);
            changedPoint = ModelMat * changedPoint;
            changedPoints.push_back(glm::vec3(changedPoint.x, changedPoint.y, changedPoint.z));
        }
        return changedPoints;
    }

    float getOriginalWidth()
    {
        return Width;
    }

    float getOriginalHeight()
    {
        return Height;
    }

    float getOriginalThickness()
    {
        return Thickness;
    }

    float getChangedWidth()
    {
        return Width * Scale;
    }

    float getChangedHeight()
    {
        return Height * Scale;
    }

    float getChangedThickness()
    {
        return Thickness * Scale;
    }

    void SetModelTransformation(glm::vec3 position, float yaw, float scale=MODELSCALE)
    {
        // 更新变化参数
        Position = position;
        Yaw = yaw;
        Scale = scale;
        // 更新模型朝向
        glm::vec4 changedFront = glm::vec4(Front, 0.0f);
        glm::mat4 FrontRotateMat = glm::rotate(glm::mat4(1.0f), yaw, glm::vec3(0.0f, 1.0f, 0.0f));
        changedFront = FrontRotateMat * glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
        Front = glm::vec3(changedFront);
        // 更新模型矩阵
        ModelMat = glm::mat4(1.0f);
        ModelMat = glm::translate(ModelMat, Position);
        ModelMat = glm::rotate(ModelMat, Yaw, glm::vec3(0.0f, 1.0f, 0.0f));
        ModelMat = glm::scale(ModelMat, glm::vec3(Scale, Scale, Scale));
    }

    void ProcessKeyboard(ModelMovement direction, float deltaTime, const Camera& camera, maze& mazee) {
        glm::vec3 originPos = Position;
        float originYaw = Yaw;
        glm::vec3 modelFront = glm::vec3(0.0f);
        glm::vec3 changedPos = glm::vec3(0.0f);
        float changedYaw = 0.0f;

        if (direction == FORWARD) {
            modelFront = camera.Front;
            if (modelFront.x >= 0) {
                changedYaw = acos(modelFront.z);
            }
            else {
                changedYaw = -acos(modelFront.z);
            }
        }

        if (direction == BACKWARD) {
            modelFront = -camera.Front;
            if (modelFront.x >= 0) {
                changedYaw = acos(modelFront.z);
            }
            else {
                changedYaw = -acos(modelFront.z);
            }
        }

        if (direction == RIGHT) {
            modelFront = camera.Right;
            if (glm::dot(Front, camera.Front) >= 0) {
                changedYaw = Yaw - RotateSpeed * deltaTime * acos(0);
            }
            else {
                changedYaw = Yaw + RotateSpeed * deltaTime * acos(0);
            }
        }

        if (direction == LEFT) {
            modelFront = -camera.Right;
            if (glm::dot(Front, camera.Front) >= 0) {
                changedYaw = Yaw + RotateSpeed * deltaTime * acos(0);
            }
            else {
                changedYaw = Yaw - RotateSpeed * deltaTime * acos(0);
            }
        }

        changedPos = Position + MoveSpeed * deltaTime * modelFront;

        SetModelTransformation(changedPos, changedYaw);

        // 做碰撞检测
        if (mazee.mazepeng(getChangedBoxPoints()[2], getChangedBoxPoints()[1], getChangedBoxPoints()[0])) {
            SetModelTransformation(originPos, originYaw);
        }
    }

private:

    // 包围盒参数，存的是初始的数据，且不会改变
    float Width, Height, Thickness;     // 模型的宽度，高度和厚度
    std::vector<glm::vec3> boxPoints;    // 存包围盒长方体的四个点，详见countModelBoundingBox()函数
    glm::vec3 Center;                   // 模型中心

    // 计算包围盒，存四个顶点
    void countModelBoundingBox() {
        glm::vec3 tmin, tmax;
        glm::vec3 point0, point1, point2, point3;
        tmin = meshes[0].vertices[0].Position;
        tmax = meshes[0].vertices[0].Position;
        for (unsigned int i = 0; i < meshes.size(); ++i) {
            for (unsigned int j = 0; j < meshes[i].vertices.size(); ++j) {
                Vertex vertex = meshes[i].vertices[j];
                tmin.x = min(tmin.x, vertex.Position.x);
                tmin.y = min(tmin.y, vertex.Position.y);
                tmin.z = min(tmin.z, vertex.Position.z);
                tmax.x = max(tmax.x, vertex.Position.x);
                tmax.y = max(tmax.y, vertex.Position.y);
                tmax.z = max(tmax.z, vertex.Position.z);
            }
        }
        Width = tmax.x - tmin.x;        // 宽度，“左右”距离
        Height = tmax.y - tmin.y;       // 高度，“上下”距离
        Thickness = tmax.z - tmin.z;    // 厚度，“前后”距离，模型是面向z正轴
        point0 = glm::vec3(tmin.x + Width, tmin.y, tmin.z);
        point1 = glm::vec3(tmin.x, tmin.y, tmin.z);
        point2 = glm::vec3(tmin.x, tmin.y, tmin.z + Thickness);
        //point3 = glm::vec3(tmin.x, tmin.y + Height, tmin.z);
        //point3 = tmax;
        point3 = glm::vec3(tmin.x + Width, tmin.y, tmin.z + Thickness);
        boxPoints.push_back(point0);
        boxPoints.push_back(point1);
        boxPoints.push_back(point2);
        boxPoints.push_back(point3);
    }

    // loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
    void loadModel(string const& path)
    {
        // read file via ASSIMP
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
        // check for errors
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
        {
            cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
            return;
        }
        // retrieve the directory path of the filepath
        directory = path.substr(0, path.find_last_of('/'));

        // process ASSIMP's root node recursively
        processNode(scene->mRootNode, scene);
    }

    // processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
    void processNode(aiNode* node, const aiScene* scene)
    {
        // process each mesh located at the current node
        for (unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            // the node object only contains indices to index the actual objects in the scene. 
            // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(processMesh(mesh, scene));
        }
        // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
        for (unsigned int i = 0; i < node->mNumChildren; i++)
        {
            processNode(node->mChildren[i], scene);
        }

    }

    Mesh processMesh(aiMesh* mesh, const aiScene* scene)
    {
        // data to fill
        vector<Vertex> vertices;
        vector<unsigned int> indices;
        vector<Texture> textures;

        // walk through each of the mesh's vertices
        for (unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex;
            glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
            // positions
            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            vertex.Position = vector;
            // normals
            if (mesh->HasNormals())
            {
                vector.x = mesh->mNormals[i].x;
                vector.y = mesh->mNormals[i].y;
                vector.z = mesh->mNormals[i].z;
                vertex.Normal = vector;
            }
            // texture coordinates
            if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
            {
                glm::vec2 vec;
                // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
                // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
                vec.x = mesh->mTextureCoords[0][i].x;
                vec.y = mesh->mTextureCoords[0][i].y;
                vertex.TexCoords = vec;
                // tangent
                vector.x = mesh->mTangents[i].x;
                vector.y = mesh->mTangents[i].y;
                vector.z = mesh->mTangents[i].z;
                vertex.Tangent = vector;
                // bitangent
                vector.x = mesh->mBitangents[i].x;
                vector.y = mesh->mBitangents[i].y;
                vector.z = mesh->mBitangents[i].z;
                vertex.Bitangent = vector;
            }
            else
                vertex.TexCoords = glm::vec2(0.0f, 0.0f);

            vertices.push_back(vertex);
        }
        // now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
        for (unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            // retrieve all indices of the face and store them in the indices vector
            for (unsigned int j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }
        // process materials
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        // we assume a convention for sampler names in the shaders. Each diffuse texture should be named
        // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
        // Same applies to other texture as the following list summarizes:
        // diffuse: texture_diffuseN
        // specular: texture_specularN
        // normal: texture_normalN

        // 1. diffuse maps
        vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        // 2. specular maps
        vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
        // 3. normal maps
        std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
        // 4. height maps
        std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
        textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

        // return a mesh object created from the extracted mesh data
        return Mesh(vertices, indices, textures);
    }

    // checks all material textures of a given type and loads the textures if they're not loaded yet.
    // the required info is returned as a Texture struct.
    vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName)
    {
        vector<Texture> textures;
        for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
        {
            aiString str;
            mat->GetTexture(type, i, &str);
            str = UTF8ToGB(str.C_Str());
            // check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
            bool skip = false;
            for (unsigned int j = 0; j < textures_loaded.size(); j++)
            {
                if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
                {
                    textures.push_back(textures_loaded[j]);
                    skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
                    break;
                }
            }
            if (!skip)
            {   // if texture hasn't been loaded already, load it
                Texture texture;
                texture.id = TextureFromFile(str.C_Str(), this->directory);
                texture.type = typeName;
                texture.path = str.C_Str();
                textures.push_back(texture);
                textures_loaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
            }
        }
        return textures;
    }
};


unsigned int TextureFromFile(const char* path, const string& directory, bool gamma)
{
    string filename = string(path);
    filename = directory + '/' + filename;

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
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

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

#endif