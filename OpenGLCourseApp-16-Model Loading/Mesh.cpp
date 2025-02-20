#include "GL/glew.h"
#include <string>
#include "Shader.h"
#include "Mesh.h"

Mesh::Mesh(std::vector<Vertex> _vertices, std::vector<unsigned int> _indices, std::vector<Texture> _textures)
{
	vertices = _vertices;
	indices = _indices;
	textures = _textures;

	SetupMesh();
}

void Mesh::Draw(Shader &shader)
{
	unsigned  int  diffuseNr = 1;
	unsigned  int  specularNr = 1;

	for (int i = 0; i < textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);  //  activate  texture  unit  first //  retrieve  texture  number  (the  N  in  diffuse_textureN)
		std::string  number;
		std::string  name = textures[i].type;
		
		if (name == "texture_diffuse")
			number = std::to_string(diffuseNr++);
		
		else  if (name == "texture_specular")
			number = std::to_string(specularNr++);
						//"material." + name + number

		shader.SetInt((name + number).c_str(), i);
		glBindTexture(GL_TEXTURE_2D, textures[i].id);
	}

	//glActiveTexture(GL_TEXTURE0);
	//  draw  mesh
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0); 
	glBindVertexArray(0);
}

void Mesh::SetupMesh()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO); 
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

	glBindVertexArray(0);
}
