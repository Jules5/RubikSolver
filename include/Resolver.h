#ifndef MASTER_H
#define MASTER_H

#include <iostream>
#include <SDL/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "Enums.h"
#include "Cube.h"
#include "Rubik3x3.h"

using namespace std;


class Resolver
{

	public :

		bool is_test;
		int compteur;

		enum ResolverState {SHOW, RESOLVE};

		ResolverState state;

		float rotate_x, rotate_y;

		bool move;

		Rubik3x3* rubik;

		GLuint id_logo;

		static const unsigned int NB_STEPS;
		unsigned int current_step;

		vector<Cube*> save;

		Resolver();
		~Resolver();

		void run();
		void display(int dt);

		Cube* getCube(Cube** tab, RubikColor color, TypeCube type=UNKNOW_TYPE);
		vector<Cube*> getCubes(Cube** tab, RubikColor color, TypeCube type=UNKNOW_TYPE);
		Cube* getNextPosition(Cube*, FacesCube, bool way=true, unsigned int nb_rot=0);

		void resolve();
		void resolveStep0();
		void resolveStep1();
		void resolveStep2();
		void resolveStep3();
		void resolveStep4();
		void resolveStep5();
		void resolveStep6();

		void setNextStep();

		bool whiteCross();
		bool whiteCorners();
		bool middleRing();
		bool yellowCross();
		bool yellowCorners();
		bool yellowCornersCorrect();
		bool yellowCrossCorrect();
		
		bool yellowBend(FacesCube*,FacesCube*);
		bool yellowLine(FacesCube*,FacesCube*);
		bool yellowCornersDouble(FacesCube*,FacesCube*,FacesCube*);
		bool getSensRotationFinal(FacesCube*);

};


#endif