#ifndef AFFINE_H
#define AFFINE_H
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>

/*
 * An affine matrix A can be factored as A = TL. You need to fill up two function named 'linearFact' and 'transFact'
 */

// TODO: Fill up linearFact function
// input: A (4 x 4 matrix)
// output: L (4 x 4 matrix)

//first three elements of first three columns comprises linear transformations.
glm::mat4 linearFact(glm::mat4 A)
{
	glm::mat4 L = glm::mat4(1.0f);

	for (int i = 0; i < 3; i++)
		glm::column(L, i, glm::column(A, i));
	//The reason why 4th row is copied is because affine transformation matrix's very last element would be "1"
	//Since column is occupied for translation... therefore, row
	glm::row(L, 3, glm::row(A, 3));

	return L;
}

// TODO: Fill up transFact function
// input: A (4 x 4 matrix)
// output: T (4 x 4 matrix)

//since glm is column-major ordered, we should approach as 4th column's first three elements are composing transformation matrix
//for accessing matrix rows and columns: http://glm.g-truc.net/0.9.3/api/a00144.html
glm::mat4 transFact(glm::mat4 A)
{
	glm::mat4 T = glm::mat4(1.0f);

	//glm::column with 2 arguments "gets", and 3 arguments "sets"
	glm::column(T, 3, glm::column(A, 3));

	return T;
}
#endif
