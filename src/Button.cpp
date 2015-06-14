#include "Button.h"



Button::Button(float x, float y, float w, float h, string t, TTF_Font* font, Resolver* r, void (Resolver::*pf)())
:posx(x), posy(y), width(w), height(h), text(t), resolver(r), callback_resolver(pf)
{
	hover = false;
	invert_texture = false;
	content = TEXT;
	type = RESOLVER_BUTTON;

	/* CALCUL DE LA TEXTURE DU BUTTON */
	SDL_Color Color = {0, 0, 0};
	SDL_Surface* surface = TTF_RenderText_Blended(font,(text).c_str(),Color);
	texture = 0;
	
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
 
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_BGRA,GL_UNSIGNED_BYTE, surface->pixels);
}




Button::Button(float x, float y, float w, float h, GLuint tex, bool invert, Resolver* r, void (Resolver::*pf)())
:posx(x), posy(y), width(w), height(h), texture(tex), invert_texture(invert), resolver(r), callback_resolver(pf)
{
	hover = false;
	content = IMAGE;
	type = RESOLVER_BUTTON;
}



Button::Button(float x, float y, float w, float h, string t, TTF_Font* font, Editor* r, void (Editor::*pf)())
:posx(x), posy(y), width(w), height(h), text(t), editor(r), callback_editor(pf)
{
	hover = false;
	invert_texture = false;
	content = TEXT;
	type = EDITOR_BUTTON;

	/* CALCUL DE LA TEXTURE DU BUTTON */
	SDL_Color Color = {0, 0, 0};
	SDL_Surface* surface = TTF_RenderText_Blended(font,(text).c_str(),Color);
	texture = 0;
	
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
 
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_BGRA,GL_UNSIGNED_BYTE, surface->pixels);
}




Button::Button(float x, float y, float w, float h, GLuint tex, bool invert, Editor* r, void (Editor::*pf)())
:posx(x), posy(y), width(w), height(h), texture(tex), invert_texture(invert), editor(r), callback_editor(pf)
{
	hover = false;
	content = IMAGE;
	type = EDITOR_BUTTON;
}



Button::~Button()
{
	
}




void Button::display()
{
	glDisable(GL_DEPTH_TEST);

	glPushMatrix();

	glTranslatef(posx,posy,0);
	
	/* TRACÉ DU BUTTON */
	glBegin(GL_QUADS);

		if(hover)
			glColor3ub(180,180,180);
		else
			glColor3ub(240,240,240);

		glVertex2f( 0     ,  0     );
		glVertex2f( 0     , -height);
		glVertex2f( width , -height);
		glVertex2f( width ,  0     );

	glEnd();

	/* TRACÉ DU CONTENU */
	float dx = (content==TEXT ? 0.1  : 0.1);
	float dy = (content==TEXT ? 0.02 : 0.1);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture);
	glBegin(GL_QUADS);

		if(!invert_texture)
		{
			glTexCoord2d(0, 1); glVertex2f( width*dx     , -height*(1-dy));
			glTexCoord2d(0, 0); glVertex2f( width*dx     , -height*dy    );
			glTexCoord2d(1, 0); glVertex2f( width*(1-dx) , -height*dy    );
			glTexCoord2d(1, 1); glVertex2f( width*(1-dx) , -height*(1-dy));
		}
		else
		{
			glTexCoord2d(1, 1); glVertex2f( width*dx     , -height*(1-dy));
			glTexCoord2d(1, 0); glVertex2f( width*dx     , -height*dy    );
			glTexCoord2d(0, 0); glVertex2f( width*(1-dx) , -height*dy    );
			glTexCoord2d(0, 1); glVertex2f( width*(1-dx) , -height*(1-dy));
		}

	glEnd();
	glDisable(GL_TEXTURE_2D);

	// CONTOUR 
	glLineWidth(4);
	glBegin(GL_LINE_STRIP);
		
		glColor3ub(0,0,0);
		
		glVertex2f(0 ,      0     );
		glVertex2f(0 ,     -height);
		glVertex2f(width , -height);
		glVertex2f(width ,  0     );
		glVertex2f(0 ,      0     );

	glEnd();


	
	glPopMatrix();

	glEnable(GL_DEPTH_TEST);
}






void Button::click()
{
	switch(type)
	{
		case RESOLVER_BUTTON : (resolver->*callback_resolver)(); break;
		case EDITOR_BUTTON   : (editor->*callback_editor)();   break;
	}
}