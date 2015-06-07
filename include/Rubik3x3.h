#ifndef RUBIK3X3_H
#define RUBIK3X3_H

#include <iostream>
#include <fstream>
#include <fstream>
#include <deque>
#include <vector>
#include <algorithm>
#include <SDL/SDL_image.h>

#include "Enums.h"
#include "Cube.h"
#include "sdlglutils.h"

using namespace std;

class Cube;

class Rubik3x3 
{

	public :

		GLuint id_logo;
		
		Cube* cubes[3][3][3];

		deque<RubikMoves> moves;
		Cube* moves_set[NB_FACES][9];

		float size;

		RubikStates state;
		RubikModes mode;
		float angle;


		Rubik3x3(float s=0.1, GLuint logo=0, string filename="");
		~Rubik3x3();

		void save(string filename);

		void initCubes();
		void defineGroupes();
		void defineVoisins();

		void loadRubikComplete();
		void loadRubikFromFile(string);

		void shuffle();

		void addMove(RubikMoves m);
		void addMove(FacesCube f, bool way=true);

		RubikMoves getContraryMove(RubikMoves m);

		void movesSynthese();

		void display(int dt);
		void updateCubesPositions(RubikMoves move);



		friend ostream& operator << (ostream& os, const Rubik3x3& obj);

};


#endif