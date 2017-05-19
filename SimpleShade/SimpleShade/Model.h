#pragma once
#ifndef _MODEL_H
#define _MODEL_H
#include <GLobjects.h>
#include <tiny_obj_loader.h>
#include <map>
class Model {
public:
	std::vector<GLmem> components;
	std::map<std::string, GLuint> textures;
	std::vector<tinyobj::material_t> materials;					// materials
	std::string name;
public:
	Model(){}

	void Draw();
};

void Model::Draw() {
	glPolygonMode(GL_FRONT, GL_FILL);
	glPolygonMode(GL_BACK, GL_FILL);

	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1.0, 1.0);
	GLsizei stride = (3 + 3 + 3 + 2) * sizeof(float);
	for (size_t i = 0; i < components.size(); i++) {
		GLmem o = components[i];
		if (o.vbo < 1) {
			continue;
		}

		glBindBuffer(GL_ARRAY_BUFFER, o.vbo);
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_NORMAL_ARRAY);
		glEnableClientState(GL_COLOR_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		if ((o.m_materialID < materials.size())) {
			std::string diffuse_texname = materials[o.m_materialID].diffuse_texname;
			if (textures.find(diffuse_texname) != textures.end()) {
				glBindTexture(GL_TEXTURE_2D, textures[diffuse_texname]);
			}
		}
		glVertexPointer(3, GL_FLOAT, stride, (const void*)0);
		glNormalPointer(GL_FLOAT, stride, (const void*)(sizeof(float) * 3));
		glColorPointer(3, GL_FLOAT, stride, (const void*)(sizeof(float) * 6));
		glTexCoordPointer(2, GL_FLOAT, stride, (const void*)(sizeof(float) * 9));

		glDrawArrays(GL_TRIANGLES, 0, 3 * o.m_numTriangles);
		//CheckErrors("drawarrays");
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	// draw wireframe
	glDisable(GL_POLYGON_OFFSET_FILL);
	glPolygonMode(GL_FRONT, GL_LINE);
	glPolygonMode(GL_BACK, GL_LINE);

	glColor3f(0.0f, 0.0f, 0.4f);
	for (size_t i = 0; i < components.size(); i++) {
		GLmem o = components[i];
		if (o.vbo < 1) {
			continue;
		}

		glBindBuffer(GL_ARRAY_BUFFER, o.vbo);
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_NORMAL_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glVertexPointer(3, GL_FLOAT, stride, (const void*)0);
		glNormalPointer(GL_FLOAT, stride, (const void*)(sizeof(float) * 3));
		glColorPointer(3, GL_FLOAT, stride, (const void*)(sizeof(float) * 6));
		glTexCoordPointer(2, GL_FLOAT, stride, (const void*)(sizeof(float) * 9));

		glDrawArrays(GL_TRIANGLES, 0, 3 * o.m_numTriangles);
		//CheckErrors("drawarrays");
	}
}
#endif // !_MODEL_H

