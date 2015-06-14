#ifndef BROWSER_HPP
#define BROWSER_HPP

#include <iostream>
#include <vector>
#include <GL/gl.h>
#include <dirent.h>

#include "Enums.h"
#include "Master.h"
#include "Button.h"

using namespace std;

class Master;
class Button;


class Browser
{

	public :

		Master* master;
		string path;
		TTF_Font* font;

		vector<string> files;
		vector<bool> files_hover;
		vector<GLuint> textures;
		vector<float> textures_width;

		bool run;

		float width;
		float height;
		float x;
		float y;

		string title;
		float title_height;
		float title_width;
		GLuint texture_title;

		unsigned int cursor;
		int nb_row;

		float file_width;
		float file_height;
		float file_margin_left;

		float elevator_width;

		bool create_file;
		float creator_height;

		GLuint saisie_texture;
		float saisie_width;


		Browser(Master* m, TTF_Font* font, string path="");
		~Browser();

		void setTitle(string);

		void display();

		void updateFiles();
		void sortFiles();
		string getFile(bool create=false);

		void detectChar(SDL_Event* event, string* filename);

		void buildTextures();
		void buildTextureSaisie(string);

		void gestionHover(SDL_Event* event);
		void gestionWheel(SDL_Event* event);
		string gestionClick(SDL_Event* event);


};


#endif