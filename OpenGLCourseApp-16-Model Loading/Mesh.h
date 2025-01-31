#pragma once
#include <glm/glm.hpp>
#include <vector>

class Shader;

struct Vertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
};

struct Texture {
	unsigned int id;
	std::string type;
	std::string path;
};

class Mesh {

public:
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;
	
	Mesh(std::vector<Vertex> _vertices, std::vector<unsigned int> _indices, std::vector<Texture> _textures);
	
	//draw mesh
	void Draw(Shader &shader);

private:
	//buffer objects
	unsigned int VAO, VBO, EBO;
	
	//initialize buffers
	void SetupMesh();

protected:

};