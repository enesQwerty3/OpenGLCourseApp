#pragma once
#include <vector>
#include "Mesh.h"

struct Texture;
struct Mesh;
class Shader;
class Mesh;
struct aiScene;
struct aiNode;
struct aiMaterial;
struct aiMesh;
enum aiTextureType;

class Model {

public:
	Model(const char* path , bool gamma = false);

	void Draw(Shader &shader);

private:
	std::vector<Mesh> meshes;
	std::string directory;
	std::vector<Texture> loaded_textures;
	bool gammaCorrection;

	void LoadModel(std::string path);
	void ProcessNode(aiNode* node, const aiScene* scene);
	Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> LoadMaterialTextures(aiMaterial* mat, aiTextureType  type, std::string  typeName);
	unsigned int TextureFromFile(const char* path, const std::string& directory, bool gamma);
protected:


};