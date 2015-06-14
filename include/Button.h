#ifndef BUTTON_H
#define BUTTON_H

#include <iostream>

#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL/SDL_ttf.h>

#include "Resolver.h"
#include "Editor.h"

using namespace std;

class Resolver;
class Editor;

class Button 
{

	public :

		enum ContentButton{TEXT, IMAGE};
		enum TypeButton{RESOLVER_BUTTON, EDITOR_BUTTON};

		ContentButton content;
		TypeButton type;

		Resolver* resolver;
		Editor* editor;

		float posx;
		float posy;

		float width;
		float height;

		string text;

		GLuint texture;
		bool invert_texture;

		bool hover;

		void (Resolver::*callback_resolver)();
		void (Editor::*callback_editor)();


		Button(float x, float y, float w, float h, string t, TTF_Font* font, Resolver* r=NULL, void (Resolver::*pf)()=NULL);
		Button(float x, float y, float w, float h, GLuint tex, bool invert=false, Resolver* r=NULL, void (Resolver::*pf)()=NULL);
		
		Button(float x, float y, float w, float h, string t, TTF_Font* font, Editor* r=NULL, void (Editor::*pf)()=NULL);
		Button(float x, float y, float w, float h, GLuint tex, bool invert=false, Editor* r=NULL, void (Editor::*pf)()=NULL);
		
		~Button();

		void display();
		void click();

};


#endif