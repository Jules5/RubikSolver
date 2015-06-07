#ifndef CUBE_H
#define CUBE_H

#include <iostream>
#include <GL/glut.h>

#include "Enums.h"
#include "Color.h"
#include "Rubik3x3.h"

class Rubik3x3;

using namespace std;


class Cube 
{

	public :

		TypeCube type;
		
		float posx;
		float posy;
		float posz;

		float size;

		Color colors[NB_FACES];
		unsigned int nb_colors;

		Cube* voisins[NB_FACES];


		Cube(float x=0, float y=0, float z=0, float s=0.1);
		~Cube();

		void setColor(FacesCube,Color);
		Color* getColors();
		void updateType();
		FacesCube getFace(Color);
		void setVoisin(FacesCube face, Cube* cube);

		void rotate(RubikMoves);
		void rotate(FacesCube, bool way=true);
		void switchColors(Cube*);

		void display(GLuint* tex = NULL);


		friend ostream& operator << (ostream& os, const Cube& obj);

};


#endif