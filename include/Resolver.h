#ifndef RESOLVER_HPP
#define RESOLVER_HPP

#include <iostream>
#include <SDL/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include "Enums.h"
#include "Master.h"
#include "Cube.h"
#include "Rubik3x3.h"
#include "Button.h"
#include "sdlglutils.h"

using namespace std;

class Master;
class Button;


class Resolver
{

	public :

		Master* master;

		int width;
		int height;

		float rubik_pos_x;
		float rubik_pos_y;
		float rubik_size;

		enum ResolverState {SHOW, RESOLVE};

		ResolverState state;

		bool move;
		bool  drag_player_cursor;

		Rubik3x3* rubik_view;
		Rubik3x3* rubik_test;

		static const unsigned int NB_STEPS;
		unsigned int current_step;

		vector<Button*> buttons;
		vector<Button*> player;

		bool show_player;
		float player_bar_w;
		float player_bar_h;
		float player_cursor_x;
		bool player_cursor_hover;

		float speed_rubik_normal;
		float speed_rubik_fast;

		TTF_Font* font;

		GLuint tex_play;
		GLuint tex_pause;
		GLuint tex_fast;
		GLuint tex_next;

		Resolver(Master* m, TTF_Font* f=NULL);
		~Resolver();

		void init();
		void animate(int dt);
		void display();
		void displayPlayer();

		void gestionClickButtons(SDL_Event*, bool click);
		void gestionSurvolButtons(SDL_Event*);
		void gestionMouseMotion(SDL_Event*);
		void gestionButton(SDL_Event*);

		void play();
		void playReverse();
		void pause();
		void playFast();
		void playFastReverse();
		void playOne();
		void playOneReverse();
		void quit();
		void shuffle();
		void setResolveMode();
		void resetRubikRotation();
		void resetRubik();
		void createRubik();
		void loadRubik();

		Cube* getCube(Cube** tab, RubikColor color, TypeCube type=UNKNOW_TYPE);
		vector<Cube*> getCubes(Cube** tab, RubikColor color, TypeCube type=UNKNOW_TYPE);
		Cube* getNextPosition(Rubik3x3*, Cube*, FacesCube, bool way=true, unsigned int nb_rot=0);

		void resolve();
		void resolveStep0(Rubik3x3* rubik);
		void resolveStep1(Rubik3x3* rubik);
		void resolveStep2(Rubik3x3* rubik);
		void resolveStep3(Rubik3x3* rubik);
		void resolveStep4(Rubik3x3* rubik);
		void resolveStep5(Rubik3x3* rubik);
		void resolveStep6(Rubik3x3* rubik);
		void resolveStep7(Rubik3x3* rubik);

		void setNextStep();

		bool whiteCross(Rubik3x3* rubik);
		bool whiteCorners(Rubik3x3* rubik);
		bool middleRing(Rubik3x3* rubik);
		bool yellowCross(Rubik3x3* rubik);
		bool yellowCorners(Rubik3x3* rubik);
		bool yellowCornersCorrect(Rubik3x3* rubik);
		bool yellowCrossCorrect(Rubik3x3* rubik);
		
		bool yellowBend(Rubik3x3* rubik, FacesCube*,FacesCube*);
		bool yellowLine(Rubik3x3* rubik, FacesCube*,FacesCube*);
		bool yellowCornersDouble(Rubik3x3* rubik, FacesCube*,FacesCube*,FacesCube*);
		bool getSensRotationFinal(Rubik3x3* rubik, FacesCube*);

};


#endif