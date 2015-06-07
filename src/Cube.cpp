#include "Cube.h"



Cube::Cube(float x, float y, float z, float s)
:posx(x), posy(y), posz(z), size(s)
{
	type = UNKNOW_TYPE;

	nb_colors = 0;

	for(int i=0; i<NB_FACES; ++i)
	{
		colors[i] = BLACK;
		voisins[i] = NULL;
	}
}




Cube::~Cube()
{}





void Cube::setColor(FacesCube face, Color c)
{
	if(colors[face] == BLACK)
		nb_colors++;

	colors[face] = c;

	updateType();
}




Color* Cube::getColors()
{
	Color* r = new Color[nb_colors];

	int i=0;
	for(int j=0; j<NB_FACES; ++j)
	{
		if(!(colors[j]==BLACK))
		{
			r[i] = colors[j];
			i++;

			if(i >= nb_colors)
				break;
		}
	}

	return r;
}




void Cube::updateType()
{
	switch(nb_colors)
	{
		case 1 : type = CENTER; break;
		case 2 : type = EDGE;   break;
		case 3 : type = CORNER; break;

		default : type = UNKNOW_TYPE; break;
	}
}




FacesCube Cube::getFace(Color c)
{
	for(int i=0; i<NB_FACES; ++i)
		if(colors[i] == c)
			return FacesCube(i);
	return UNKNOW_FACE;
}




void Cube::setVoisin(FacesCube face, Cube* cube)
{
	voisins[face] = cube;
}



void Cube::rotate(RubikMoves move)
{
	Color tmp;

	switch(move)
	{
		case A  :
		case PI :
			tmp            = colors[TOP];
			colors[TOP]    = colors[LEFT];
			colors[LEFT]   = colors[BOTTOM];
			colors[BOTTOM] = colors[RIGHT];
			colors[RIGHT]  = tmp;
			break;

		case P  :
		case AI :
			tmp            = colors[TOP];
			colors[TOP]    = colors[RIGHT];
			colors[RIGHT]  = colors[BOTTOM];
			colors[BOTTOM] = colors[LEFT];
			colors[LEFT]   = tmp;
			break;

		case H  :
		case BI :
			tmp           = colors[FRONT];
			colors[FRONT] = colors[RIGHT];
			colors[RIGHT] = colors[BACK];
			colors[BACK]  = colors[LEFT];
			colors[LEFT]  = tmp;
			break;

		case B  :
		case HI :
			tmp           = colors[FRONT];
			colors[FRONT] = colors[LEFT];
			colors[LEFT]  = colors[BACK];
			colors[BACK]  = colors[RIGHT];
			colors[RIGHT] = tmp;
			break;

		case G  :
		case DI :
			tmp            = colors[FRONT];
			colors[FRONT]  = colors[TOP];
			colors[TOP]    = colors[BACK];
			colors[BACK]   = colors[BOTTOM];
			colors[BOTTOM] = tmp;
			break;

		case D  :
		case GI :
			tmp            = colors[FRONT];
			colors[FRONT]  = colors[BOTTOM];
			colors[BOTTOM] = colors[BACK];
			colors[BACK]   = colors[TOP];
			colors[TOP]    = tmp;
			break;
	}
}



void Cube::rotate(FacesCube face, bool way)
{
	switch(face)
	{
		case FRONT  : rotate((way ? A : AI)); break;
		case BACK   : rotate((way ? P : PI)); break;
		case TOP    : rotate((way ? H : HI)); break;
		case BOTTOM : rotate((way ? B : BI)); break;
		case LEFT   : rotate((way ? G : GI)); break;
		case RIGHT  : rotate((way ? D : DI)); break;
	}
}




void Cube::switchColors(Cube* c)
{
	for(int i=0; i<NB_FACES; ++i)
		colors[i] = c->colors[i];
	nb_colors = c->nb_colors;
	type = c->type;
}




void Cube::display(GLuint* tex)
{
	glPushMatrix();

	glTranslatef(posx,posy,posz);

	/* TRACÉ DU CUBE */
	glBegin(GL_QUADS);
		
		
		// AVANT
		/* TRACÉ DU LOGO SI ON EST SUR LE CUBE CENTRAL BLANC */
		// if(type==CENTER && getColors()[0]==WHITE)
		// {
		// 	colors[FRONT].setGlColor();

		// 	glBindTexture(GL_TEXTURE_2D, *tex);

		// 	glBegin(GL_QUADS);
		// 		glTexCoord2d(0,0); glVertex3f(-size/2 , -size/2 , -size/2);
		// 		glTexCoord2d(0,1); glVertex3f(-size/2 ,  size/2 , -size/2);
		// 		glTexCoord2d(1,1); glVertex3f( size/2 ,  size/2 , -size/2);
		// 		glTexCoord2d(1,0); glVertex3f( size/2 , -size/2 , -size/2);
		// 	glEnd();
		// }
		// else
		{
			colors[FRONT].setGlColor();
			glVertex3f(-size/2 , -size/2 , -size/2);
			glVertex3f(-size/2 ,  size/2 , -size/2);
			glVertex3f( size/2 ,  size/2 , -size/2);
			glVertex3f( size/2 , -size/2 , -size/2);
		}

		// ARRIERE
		colors[BACK].setGlColor();
		glVertex3f(-size/2 , -size/2 , size/2);
		glVertex3f(-size/2 ,  size/2 , size/2);
		glVertex3f( size/2 ,  size/2 , size/2);
		glVertex3f( size/2 , -size/2 , size/2);

		// DESSUS
		colors[TOP].setGlColor();
		glVertex3f(-size/2 , size/2 ,  size/2);
		glVertex3f(-size/2 , size/2 , -size/2);
		glVertex3f( size/2 , size/2 , -size/2);
		glVertex3f( size/2 , size/2 ,  size/2);

		// DESSOUS
		colors[BOTTOM].setGlColor();
		glVertex3f(-size/2 , -size/2 ,  size/2);
		glVertex3f(-size/2 , -size/2 , -size/2);
		glVertex3f( size/2 , -size/2 , -size/2);
		glVertex3f( size/2 , -size/2 ,  size/2);

		// GAUCHE
		colors[LEFT].setGlColor();
		glVertex3f(-size/2 , -size/2 ,  size/2);
		glVertex3f(-size/2 , -size/2 , -size/2);
		glVertex3f(-size/2 ,  size/2 , -size/2);
		glVertex3f(-size/2 ,  size/2 ,  size/2);

		// DROITE
		colors[RIGHT].setGlColor();
		glVertex3f(size/2 , -size/2 ,  size/2);
		glVertex3f(size/2 , -size/2 , -size/2);
		glVertex3f(size/2 ,  size/2 , -size/2);
		glVertex3f(size/2 ,  size/2 ,  size/2);
	
	glEnd();

	/* TRACÉ DES ARÊTES */
	glLineWidth(5);
	glColor3f(0.f,0.f,0.f);

	// AVANT 
	glBegin(GL_LINE_STRIP);
		glVertex3f(-size/2 , -size/2 , size/2);
		glVertex3f(-size/2 ,  size/2 , size/2);
		glVertex3f( size/2 ,  size/2 , size/2);
		glVertex3f( size/2 , -size/2 , size/2);
		glVertex3f(-size/2 , -size/2 , size/2);
	glEnd();

	// ARRIERE
	glBegin(GL_LINE_STRIP);
		glVertex3f(-size/2 , -size/2 , -size/2);
		glVertex3f(-size/2 ,  size/2 , -size/2);
		glVertex3f( size/2 ,  size/2 , -size/2);
		glVertex3f( size/2 , -size/2 , -size/2);
		glVertex3f(-size/2 , -size/2 , -size/2);
	glEnd();

	// AUTRES ARÊTES
	glBegin(GL_LINES);
		glVertex3f( size/2 ,  size/2 , -size/2);
		glVertex3f( size/2 ,  size/2 ,  size/2);

		glVertex3f(-size/2 ,  size/2 , -size/2);
		glVertex3f(-size/2 ,  size/2 ,  size/2);

		glVertex3f( size/2 , -size/2 , -size/2);
		glVertex3f( size/2 , -size/2 ,  size/2);

		glVertex3f(-size/2 , -size/2 , -size/2);
		glVertex3f(-size/2 , -size/2 ,  size/2);
	glEnd();


	glPopMatrix();
}






ostream& operator << (ostream& os, const Cube& c)  
{  
    os << "Cube(" << c.posx << "," << c.posy << "," << int(c.size) << ")";
    return os;  
} 