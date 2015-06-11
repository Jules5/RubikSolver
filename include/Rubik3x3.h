#ifndef RUBIK3X3_H
#define RUBIK3X3_H

#include <iostream>
#include <fstream>
#include <fstream>
#include <vector>
#include <deque>
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

		Cube* cubes[3][3][3];
		Cube* moves_set[NB_FACES][9];

		deque<RubikMoves> moves;
		deque<RubikMoves> moves_save;

		bool reinit_position;

		float posx;
		float posy;

		float size;

		float speed;

		float rotate_x;
		float rotate_y;

		RubikStates state;
		float angle;


		Rubik3x3(float x=0, float y=0, float s=0.1, string filename="");
		~Rubik3x3();

		void save(string filename);

		void initCubes();
		void defineGroupes();
		void defineVoisins();

		void loadRubikComplete();
		void loadRubikFromFile(string);
		void loadRubikFromOther(Rubik3x3*);

		void shuffle();

		void rotate(float x, float y);

		void addMove(RubikMoves m);
		void addMove(FacesCube f, bool way=true);

		void doMovesInstant();

		RubikMoves getContraryMove(RubikMoves m);

		void movesSynthese();

		void animate(int dt);
		void display();
		
		void updateCubesPositions(RubikMoves move);



		friend ostream& operator << (ostream& os, const Rubik3x3& obj);

};


#endif