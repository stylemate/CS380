#ifndef AFFINE_H
#define AFFINE_H

// Include GLM
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>

glm::mat4 linearFact(glm::mat4 A)
{
	glm::mat4 L = glm::mat4(1.0f);
	return L;
}

//since glm is column-major ordered, 4th column's first three elements are composing transformation matrix
//for accessing matrix rows and columns: http://glm.g-truc.net/0.9.3/api/a00144.html
glm::mat4 transFact(glm::mat4 A)
{
	glm::mat4 T = glm::mat4(1.0f);

	//glm::column with 2 arguments "gets", and 3 arguments "sets"
	glm::column(T, 3, glm::column(A, 3));
	return T;
}

#endif