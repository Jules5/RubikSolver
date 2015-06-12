#ifndef BROWSER_HPP
#define BROWSER_HPP

#include <iostream>
#include <vector>
#include <GL/gl.h>
#include <dirent.h>

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
		vector<GLuint> textures;
		vector<float> textures_width;

		float width;
		float height;
		float x;
		float y;

		unsigned int cursor;
		int nb_row;


		Browser(Master* m, TTF_Font* font, string path="");
		~Browser();

		void display();

		void updateFiles();
		void sortFiles();
		string getFile();

		void buildTextures();


};


#endif