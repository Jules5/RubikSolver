#ifndef EDITOR_HPP
#define EDITOR_HPP

#include <iostream>
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "Enums.h"
#include "Master.h"
#include "Color.h"
#include "Button.h"

using namespace std;

class Master;
class Button;


class Editor
{

	public :

		Master* master;

		RubikColor squares[NB_COLORS][9];
		float square_size;

		RubikColor indices[4];
		int indice_hover;

		bool hover[9];

		float pos_x;
		float pos_y;

		float menu_pos_x;
		float menu_pos_y;
		float menu_item_size;
		int menu_hover;
		
		RubikColor current_color;

		vector<Button*> buttons;


		TTF_Font *font;




		Editor(Master* m, TTF_Font* f=NULL);
		~Editor();

		void display();
		void quit();

		void loadRubik();
		void saveRubik();
		void resetRubik();

		void switchToResolver();

		void updateIndices();
		
		RubikColor setNextColor(RubikColor);
		RubikColor setPrevColor(RubikColor);

		void gestionEvents(SDL_Event* event);
		void gestionClickButtons(SDL_Event*);
		void gestionMouseMotion(SDL_Event*);
		void gestionButton(SDL_Event*);
		void gestionWheel(SDL_Event*);


};


#endif