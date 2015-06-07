#include <iostream>
#include <fstream>

#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "Enums.h"
#include "Color.h"

using namespace std;



int RES_X = 600;
int RES_Y = 600;

float square_size = 0.25;

TTF_Font *font = NULL;

RubikColor current_color = WHITE;

Color squares[NB_COLORS][9];




void setNextColor(RubikColor* color)
{
	int tmp = *color;
	if(tmp < NB_COLORS-1)
		tmp++;
	*color = RubikColor(tmp);
}



void setPrevColor(RubikColor* color)
{
	int tmp = *color;
	if(tmp > WHITE)
		tmp--;
	*color = RubikColor(tmp);
}


void setNextColorLoop(Color* color, bool sens=true)
{
	int tmp = color->getRubikColor();
	if(sens)
	{
		tmp++;
		if(tmp >= NB_COLORS)
			tmp = WHITE;
	}
	else
	{
		tmp--;
		if(tmp <= 0)
			tmp = NB_COLORS-1;
	}
	*color = Color(RubikColor(tmp));
}



void initSquares(Color s[NB_COLORS][9])
{
	for(int i=1; i<NB_COLORS; ++i)
		for(int j=0; j<9; ++j)
			s[i][j] = Color(RubikColor(i));
}



void drawFace(RubikColor color, Color squares[9], float size)
{
	// DÉCALAGE 
	float dy = 0.1;

	glBegin(GL_QUADS);

		/* LIGNE HAUT */
		squares[0].setGlColor();
		glVertex2f(-size/2*3 , size/2*3+dy);
		glVertex2f(-size/2*3 , size/2+dy);
		glVertex2f(-size/2   , size/2+dy);
		glVertex2f(-size/2   , size/2*3+dy);

		squares[1].setGlColor();
		glVertex2f(-size/2 , size/2*3+dy);
		glVertex2f(-size/2 , size/2+dy);
		glVertex2f(size/2  , size/2+dy);
		glVertex2f(size/2  , size/2*3+dy);

		squares[2].setGlColor();
		glVertex2f(size/2   , size/2*3+dy);
		glVertex2f(size/2   , size/2+dy);
		glVertex2f(size/2*3 , size/2+dy);
		glVertex2f(size/2*3 , size/2*3+dy);

		/* LIGNE MILIEU */
		squares[3].setGlColor();
		glVertex2f(-size/2*3 , -size/2+dy);
		glVertex2f(-size/2*3 ,  size/2+dy);
		glVertex2f(-size/2   ,  size/2+dy);
		glVertex2f(-size/2   , -size/2+dy);

		squares[4].setGlColor();
		glVertex2f(-size/2 , -size/2+dy);
		glVertex2f(-size/2 ,  size/2+dy);
		glVertex2f(size/2  ,  size/2+dy);
		glVertex2f(size/2  , -size/2+dy);

		squares[5].setGlColor();
		glVertex2f(size/2   , -size/2+dy);
		glVertex2f(size/2   ,  size/2+dy);
		glVertex2f(size/2*3 ,  size/2+dy);
		glVertex2f(size/2*3 , -size/2+dy);

		/* LIGNE BAS */
		squares[6].setGlColor();
		glVertex2f(-size/2*3 , -size/2*3+dy);
		glVertex2f(-size/2*3 , -size/2+dy);
		glVertex2f(-size/2   , -size/2+dy);
		glVertex2f(-size/2   , -size/2*3+dy);

		squares[7].setGlColor();
		glVertex2f(-size/2 , -size/2*3+dy);
		glVertex2f(-size/2 , -size/2+dy);
		glVertex2f(size/2  , -size/2+dy);
		glVertex2f(size/2  , -size/2*3+dy);

		squares[8].setGlColor();
		glVertex2f(size/2   , -size/2*3+dy);
		glVertex2f(size/2   , -size/2+dy);
		glVertex2f(size/2*3 , -size/2+dy);
		glVertex2f(size/2*3 , -size/2*3+dy);

		/* INDICES AUTRES FACES */
		Color indices[4];
		switch(color)
		{
			case WHITE  : indices[0]=BLUE;   indices[1]=ORANGE; indices[2]=GREEN;  indices[3]=RED;    break;
			case RED    : indices[0]=BLUE;   indices[1]=WHITE;  indices[2]=GREEN;  indices[3]=YELLOW; break;
			case BLUE   : indices[0]=YELLOW; indices[1]=ORANGE; indices[2]=WHITE;  indices[3]=RED;    break;
			case GREEN  : indices[0]=WHITE;  indices[1]=ORANGE; indices[2]=YELLOW; indices[3]=RED;    break;
			case ORANGE : indices[0]=BLUE;   indices[1]=YELLOW; indices[2]=GREEN;  indices[3]=WHITE;  break;
			case YELLOW : indices[0]=BLUE;   indices[1]=RED;    indices[2]=GREEN;  indices[3]=ORANGE; break;
		}

		indices[0].setGlColor();
		glVertex2f(-size/2 , size/2*6+dy);
		glVertex2f(-size/2 , size/2*4+dy);
		glVertex2f( size/2 , size/2*4+dy);
		glVertex2f( size/2 , size/2*6+dy);

		indices[1].setGlColor();
		glVertex2f(-size/2*6 , -size/2+dy);
		glVertex2f(-size/2*6 ,  size/2+dy);
		glVertex2f(-size/2*4 ,  size/2+dy);
		glVertex2f(-size/2*4 , -size/2+dy);

		indices[2].setGlColor();
		glVertex2f(-size/2 , -size/2*6+dy);
		glVertex2f(-size/2 , -size/2*4+dy);
		glVertex2f( size/2 , -size/2*4+dy);
		glVertex2f( size/2 , -size/2*6+dy);

		indices[3].setGlColor();
		glVertex2f(size/2*6 , -size/2+dy);
		glVertex2f(size/2*6 ,  size/2+dy);
		glVertex2f(size/2*4 ,  size/2+dy);
		glVertex2f(size/2*4 , -size/2+dy);

	glEnd();


	/* ARÊTES */
	glLineWidth(4);
	glColor3ub(0,0,0);
	
	glBegin(GL_LINES);

		glVertex2f(-size/2*3, size/2+dy);
		glVertex2f( size/2*3, size/2+dy);

		glVertex2f(-size/2*3, -size/2+dy);
		glVertex2f( size/2*3, -size/2+dy);

		glVertex2f(size/2 , -size/2*3+dy);
		glVertex2f(size/2 ,  size/2*3+dy);

		glVertex2f(-size/2 , -size/2*3+dy);
		glVertex2f(-size/2 ,  size/2*3+dy);

	glEnd();
}







void mouseClick(Color s[8], int x, int y, bool sens=true)
{
	float decal_y = 0.1/2;

	float click_x = float(x)/RES_X-0.5;
	float click_y = float(y)/RES_Y-0.5 + decal_y;

	float borne = square_size/4*3;

	if(click_x < borne && click_x > -borne
	&& click_y < borne && click_y > -borne)
	{
		if(click_x < -borne/3)
		{
			     if(click_y < -borne/3) setNextColorLoop(&s[0], sens);
			else if(click_y <  borne/3) setNextColorLoop(&s[3], sens);
			else                        setNextColorLoop(&s[6], sens);
		}

		else if(click_x < borne/3)
		{
			     if(click_y < -borne/3) setNextColorLoop(&s[1], sens);
			else if(click_y >  borne/3) setNextColorLoop(&s[7], sens);
		}

		if(click_x > borne/3)
		{
			     if(click_y < -borne/3) setNextColorLoop(&s[2], sens);
			else if(click_y <  borne/3) setNextColorLoop(&s[5], sens);
			else                        setNextColorLoop(&s[8], sens);
		}
	}
}





void drawInviteFilename(string filename)
{
	glClear(GL_COLOR_BUFFER_BIT);
	drawFace(current_color,squares[current_color],square_size);

	SDL_Color Color = {0, 0, 0};
	SDL_Surface *Message = TTF_RenderText_Blended(font, ("FILENAME : "+filename).c_str(), Color);
	unsigned Texture = 0;
 
	/*Generate an OpenGL 2D texture from the SDL_Surface*.*/
	glGenTextures(1, &Texture);
	glBindTexture(GL_TEXTURE_2D, Texture);
 
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Message->w, Message->h, 0, GL_BGRA,
	             GL_UNSIGNED_BYTE, Message->pixels);

	/* Conversion tailles */
	float real_dx = -0.9;
	float real_dy = -0.9;
	float real_w = float(Message->w)/RES_X;
	float real_h = float(Message->h)/RES_Y;

	/* Cadre de l'input */
	glBegin(GL_QUADS);
		glColor3ub(255,255,255);
		glVertex3d(real_dx , real_dy+real_h  ,   0);
		glVertex3d(real_dx+real_w  , real_dy+real_h  ,   0);
		glVertex3d(real_dx+real_w  , real_dy , 0);
		glVertex3d(real_dx , real_dy , 0);
 
	/* Affichage du texte */
		glTexCoord2d(0, 0); glVertex3d(real_dx , real_dy+real_h  ,   0);
		glTexCoord2d(1, 0); glVertex3d(real_dx+real_w  , real_dy+real_h  ,   0);
		glTexCoord2d(1, 1); glVertex3d(real_dx+real_w  , real_dy , 0);
		glTexCoord2d(0, 1); glVertex3d(real_dx , real_dy , 0);
	glEnd();
 
	/*Clean up.*/
	glDeleteTextures(1, &Texture);
	SDL_FreeSurface(Message);

	glFlush();
    SDL_GL_SwapBuffers();

}




void detectChar(SDL_Event* event, string* filename)
{
	int code = event->key.keysym.sym;
	
	if(code >= SDLK_KP0 && code <= SDLK_KP9) // CHIFFRES (PAVÉ NUM)
		*filename = *filename + char(event->key.keysym.sym-208);

	else if(code >= SDLK_0 && code <= SDLK_9) // CHIFFRES
		*filename = *filename + char(event->key.keysym.sym);

	else if(code >= SDLK_a && code <= SDLK_z) // LETTRES
	{
		char tmp = 0;
		if(event->key.keysym.mod & KMOD_CAPS || event->key.keysym.mod & KMOD_SHIFT)
			tmp = 32;
		*filename = *filename + char(char(event->key.keysym.sym)-tmp);
	}

	else if(code == SDLK_MINUS) // MOINS -
		*filename = *filename + '-';

	else if(code == SDLK_UNDERSCORE) // UNDERSCORE _
		*filename = *filename + '_';

	else if(code == SDLK_BACKSPACE) // EFFACER
		if(filename->length()>0)
			filename->erase(filename->length()-1);
}




void saveIntoFile(Color squares[NB_COLORS][9])
{
	string filename = "";
	bool continuer = true;
	bool ok = true;

	while(continuer)
	{
		drawInviteFilename(filename);

		SDL_Event event;
		SDL_WaitEvent(&event);

        switch(event.type)
        {
           	case SDL_KEYUP :
            	switch (event.key.keysym.sym)
       			{
       				case SDLK_ESCAPE : 
       					ok = false;
       					continuer = false;
       					break;

       				case SDLK_RETURN :
       					if(!filename.empty())
       					{
	       					ok = true;
	       					continuer = false;
	       				}
       					break;

       				default :
       					detectChar(&event,&filename);
       			}
            	break;
        }
	}

	if(ok)
	{
		ofstream file(("saves/"+filename+".rbk").c_str(), ios::out | ios::trunc);
        
        if(!file)  // si l'ouverture a réussi
       		cerr << "Échec dans l'ouverture du fichier." << endl;
       	else
       	{
       		for(int i=1; i<NB_COLORS; ++i)
       			for(int j=0; j<9; ++j)
       			{
       				file << squares[i][j].red;
       				file << squares[i][j].green;
       				file << squares[i][j].blue;
       			}

       		file.close();
       	}
	}

}






int main()
{
	initSquares(squares);

	SDL_Init(SDL_INIT_VIDEO);
	SDL_WM_SetCaption("Rubik Editer",NULL);
	SDL_SetVideoMode(RES_X, RES_Y, 32, SDL_OPENGL);

	TTF_Init();
	font = TTF_OpenFont("fonts/Roboto-Regular.ttf", 65);
	if(!font)
	{
		cout << "Police introuvable" << endl;
		exit(0);
	}

	glClearColor(.1,.1,.1,1.);

	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	 
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	while(current_color < NB_COLORS)
	{
		SDL_Event event;

		while(SDL_PollEvent(&event))
	    {
	        switch(event.type)
	        {
	            case SDL_QUIT :
	                current_color = NB_COLORS;
	                break;

	            case SDL_KEYUP :
	            	switch (event.key.keysym.sym)
	       			{
	       				case SDLK_ESCAPE : 
	       					current_color = NB_COLORS;
	       					break;

	       				case SDLK_LEFT :
	       					setPrevColor(&current_color);
	       					break;

	       				case SDLK_RIGHT :
	       					setNextColor(&current_color);
	       					break;

	       				case SDLK_RETURN :
	       					saveIntoFile(squares);
	       			}
	            	break;

	            case SDL_MOUSEBUTTONDOWN :
	            	switch(event.button.button)
	            	{
	            		case SDL_BUTTON_LEFT :
	            			mouseClick(squares[current_color],event.motion.x,event.motion.y,true);
	            			break;
	            		case SDL_BUTTON_RIGHT :
	            			mouseClick(squares[current_color],event.motion.x,event.motion.y,false);
	            			break;
	            	}
	            	break;
	            	

	        }
	    }

	    glClear(GL_COLOR_BUFFER_BIT);

		drawFace(current_color,squares[current_color],square_size);

		glFlush();
        SDL_GL_SwapBuffers();
	}

	TTF_CloseFont(font);
	TTF_Quit();
	SDL_Quit();
	return 0;
}