/*
 * Name: Pranav Rao
 * Class: CSCI 168 Graphics Programming in OpenGL
 * Description: This class allows users to parse OBJ files
 */

#pragma once

#include <fstream>
#include <sstream>
#include <vector>
#include <glm/glm.hpp>

class ModelImporter {

private:
	std::vector<float> vertVals;
	std::vector<float> stVals;
	std::vector<float>nrmVals;

	std::vector<float> triangleVerts;
	std::vector<float> textureCoords;
	std::vector<float> normals;
	std::vector<float> tangent;

public:
	// Blank constructor
	ModelImporter() {}

	// Parse the OBJ file and extract the necessary information
	void parseOBJ(const char * filePath) {
		float x, y, z;
		std::string content;
		std::ifstream fileStream(filePath, std::ios::in);
		std::string line = "";
		while(!fileStream.eof()) {
			std::getline(fileStream, line);

			// If we get a vertex line, add the numerical components to the vertex array
			if (line.compare(0, 2, "v ") == 0) {
				std::stringstream ss(line.erase(0, 1));
				ss >> x;
				ss >> y;
				ss >> z;
				vertVals.push_back(x);
				vertVals.push_back(y);
				vertVals.push_back(z);
			}

			// If we get a texture coordinate line, do the same (remove the alphabets)
			if (line.compare(0, 2, "vt") == 0) {
				std::stringstream ss(line.erase(0, 2));
				ss >> x;
				ss >> y;
				stVals.push_back(x);
				stVals.push_back(y);
			}

			// Normal coordinate line
			if (line.compare(0, 2, "vn") == 0) {
				std::stringstream ss(line.erase(0, 2));
				ss >> x;
				ss >> y;
				ss >> z;
				nrmVals.push_back(x);
				nrmVals.push_back(y);
				nrmVals.push_back(z);
			}

			// Faces
			if (line.compare(0, 2, "f ") == 0) {
				std::string oneCorner, v, t, n;
				std::stringstream ss(line.erase(0, 2));
				for (int i = 0; i < 3; i++) {
					std::getline(ss, oneCorner, ' ');
					std::stringstream oneCornerSS(oneCorner);
					getline(oneCornerSS, v, '/');
					getline(oneCornerSS, t, '/');
					getline(oneCornerSS, n, '/');

					int vertRef = (stoi(v) - 1) * 3;
					int tcRef = (stoi(t) - 1) * 2;
					int nrmRef = (stoi(n) - 1) * 3;

					triangleVerts.push_back(vertVals[vertRef]);
					triangleVerts.push_back(vertVals[vertRef + 1]);
					triangleVerts.push_back(vertVals[vertRef + 2]);

					textureCoords.push_back(stVals[tcRef]);
					textureCoords.push_back(stVals[tcRef + 1]);

					normals.push_back(nrmVals[nrmRef]);
					normals.push_back(nrmVals[nrmRef + 1]);
					normals.push_back(nrmVals[nrmRef + 2]);
				}
			}
		}
	}

	// Accessor Methods
	int getNumVertices() {
		return (triangleVerts.size() / 3);
	}

	std::vector<float> getVertices() {
		return triangleVerts;
	}

	std::vector<float> getTextureCoords() {
		return textureCoords;
	}

	std::vector<float> getNormals() {
		return normals;
	}

};
