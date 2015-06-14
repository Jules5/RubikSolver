#ifndef MASTER_HPP
#define MASTER_HPP

#include <iostream>

#include "Browser.h"
#include "Resolver.h"
#include "Editor.h"

using namespace std;

class Browser;
class Resolver;
class Editor;


class Master
{

	public :

		string path;

		bool is_running;

		int width;
		int height;
		int ratio;

		enum MasterModes {RESOLVER, EDITOR};
		MasterModes mode;

		Resolver* resolver;
		Editor* editor;
		Browser* browser;

		TTF_Font* font;
		TTF_Font* font_small;


		Master(string path);
		~Master();

		void run();
		void animate(int dt);
		void display();
		
		void switchMode();


};


#endif