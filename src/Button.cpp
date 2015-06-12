#include "Button.h"



Button::Button(float x, float y, float w, float h, string t, TTF_Font* font, Resolver* r, void (Resolver::*pf)())
:posx(x), posy(y), width(w), height(h), text(t), resolver(r), callback(pf)
{
	hover = false;
	invert_texture = false;
	content = TEXT;

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
:posx(x), posy(y), width(w), height(h), texture(tex), invert_texture(invert), resolver(r), callback(pf)
{
	hover = false;
	content = IMAGE;
}


Button::~Button()
{
	
}




void Button::display()
{
	glPushMatrix();

	glTranslatef(posx,posy,0);
	
	/* TRACÉ DU BUTTON */
	glBegin(GL_QUADS);

		if(hover)
			glColor3ub(180,180,180);
		else
			glColor3ub(240,240,240);

		glVertex3f( 0     ,  0      ,0);
		glVertex3f( 0     , -height ,0);
		glVertex3f( width , -height ,0);
		glVertex3f( width ,  0      ,0);

	glEnd();

	/* TRACÉ DU CONTENU */
	float dx = (content==TEXT ? 0.1  : 0.1);
	float dy = (content==TEXT ? 0.02 : 0.1);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture);
	glBegin(GL_QUADS);

		if(!invert_texture)
		{
			glTexCoord2d(0, 1); glVertex3f( width*dx     , -height*(1-dy) ,-1);
			glTexCoord2d(0, 0); glVertex3f( width*dx     , -height*dy     ,-1);
			glTexCoord2d(1, 0); glVertex3f( width*(1-dx) , -height*dy     ,-1);
			glTexCoord2d(1, 1); glVertex3f( width*(1-dx) , -height*(1-dy) ,-1);
		}
		else
		{
			glTexCoord2d(1, 1); glVertex3f( width*dx     , -height*(1-dy) ,-1);
			glTexCoord2d(1, 0); glVertex3f( width*dx     , -height*dy     ,-1);
			glTexCoord2d(0, 0); glVertex3f( width*(1-dx) , -height*dy     ,-1);
			glTexCoord2d(0, 1); glVertex3f( width*(1-dx) , -height*(1-dy) ,-1);
		}

	glEnd();
	glDisable(GL_TEXTURE_2D);

	// CONTOUR 
	glBegin(GL_LINE_STRIP);
		
		glLineWidth(4);
		glColor3ub(0,0,0);
		
		glVertex3f(0 ,      0      , 1);
		glVertex3f(0 ,     -height , 1);
		glVertex3f(width , -height , 1);
		glVertex3f(width ,  0      , 1);
		glVertex3f(0 ,      0      , 1);

	glEnd();


	
	glPopMatrix();
}






void Button::click()
{
	(resolver->*callback)();
}