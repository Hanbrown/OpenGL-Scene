/*
 * Name: Pranav Rao
 * Class: CSCI 168 Graphics Programming in OpenGL
 * Description: This class allows users to use imported OBJ objects. It requires the model importer class
 */

#pragma once
#include "ModelImporter.h"

class ImportedModel {

private:
	int numVertices;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> texCoords;
	std::vector<glm::vec3> normalVecs;
	std::vector<glm::vec3> tangents;

public:

	// Create an instance of an imported model and allow it to be added to the vertex buffer object
	ImportedModel(const char * filePath) {
		ModelImporter modelImporter = ModelImporter();
		modelImporter.parseOBJ(filePath);

		numVertices = modelImporter.getNumVertices();
		std::vector<float> verts = modelImporter.getVertices();
		std::vector<float> tcs = modelImporter.getTextureCoords();
		std::vector<float> normals = modelImporter.getNormals();

		// Push the vertices for the faces into the 3 main vectors
		// 3 because the faces are all triangles
		for (int i = 0; i < numVertices; i++) {
			vertices.push_back(glm::vec3(verts[i * 3], verts[i * 3 + 1], verts[i * 3 + 2]));
			texCoords.push_back(glm::vec2(tcs[i * 2], tcs[i * 2 + 1]));
			normalVecs.push_back(glm::vec3(normals[i * 3], normals[i * 3 + 1], normals[i * 3 + 2]));
			tangents.push_back(glm::vec3(verts[i * 3], verts[i * 3 + 1], verts[i * 3 + 2]));
		}
	}

	// Accessor methods
	int getNumVertices() {
		return numVertices;
	}

	std::vector<glm::vec3> getVertices() {
		return vertices;
	}

	std::vector<glm::vec2> getTextureCoords() {
		return texCoords;
	}

	std::vector<glm::vec3> getNormals() {
		return normalVecs;
	}

	std::vector<glm::vec3> getTangents() {
		return tangents;
	}

};
