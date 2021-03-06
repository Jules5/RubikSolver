#include "Cube.h"



Cube::Cube(float x, float y, float z, float s)
:posx(x), posy(y), posz(z), size(s)
{
	type = UNKNOW_TYPE;

	nb_colors = 0;

	for(int i=0; i<6; ++i)
	{
		colors[i] = BLACK;
		voisins[i] = NULL;
	}
}






Cube::~Cube()
{}




void Cube::copyColors(Cube* cube)
{
	for(int i=0; i<6; ++i)
		colors[i] = cube->colors[i];
}




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
	for(int j=0; j<6; ++j)
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
	for(int i=0; i<6; ++i)
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
	for(int i=0; i<6; ++i)
		colors[i] = c->colors[i];
	nb_colors = c->nb_colors;
	type = c->type;
}




void Cube::display(GLuint tex)
{
	glPushMatrix();

	glTranslatef(posx,posy,posz);


	/* TRACÉ DU CUBE */
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	float d = 0.41;

	glBegin(GL_QUADS);
		
		// AVANT
		colors[FRONT].setGlColor();
		glNormal3f( 0, 0, 1);
		glVertex3f(-size*d , -size*d , -size*0.501);
		glVertex3f( size*d , -size*d , -size*0.501);
		glVertex3f( size*d ,  size*d , -size*0.501);
		glVertex3f(-size*d ,  size*d , -size*0.501);

		// ARRIERE
		colors[BACK].setGlColor();
		glNormal3f( 0, 0, -1);
		glVertex3f(-size*d , -size*d , size*0.501);
		glVertex3f(-size*d ,  size*d , size*0.501);
		glVertex3f( size*d ,  size*d , size*0.501);
		glVertex3f( size*d , -size*d , size*0.501);

		// DESSUS
		colors[TOP].setGlColor();
		glNormal3f( 0, -1, 0);
		glVertex3f(-size*d , size*0.501 ,  size*d);
		glVertex3f(-size*d , size*0.501 , -size*d);
		glVertex3f( size*d , size*0.501 , -size*d);
		glVertex3f( size*d , size*0.501 ,  size*d);

		// DESSOUS
		colors[BOTTOM].setGlColor();
		glNormal3f( 0, 1, 0);
		glVertex3f(-size*d , -size*0.501 ,  size*d);
		glVertex3f( size*d , -size*0.501 ,  size*d);
		glVertex3f( size*d , -size*0.501 , -size*d);
		glVertex3f(-size*d , -size*0.501 , -size*d);

		// GAUCHE
		colors[LEFT].setGlColor();
		glNormal3f( 1, 0, 0);
		glVertex3f(-size*0.501 , -size*d ,  size*d);
		glVertex3f(-size*0.501 , -size*d , -size*d);
		glVertex3f(-size*0.501 ,  size*d , -size*d);
		glVertex3f(-size*0.501 ,  size*d ,  size*d);

		// DROITE
		colors[RIGHT].setGlColor();
		glNormal3f( -1, 0, 0);
		glVertex3f(size*0.501 , -size*d ,  size*d);
		glVertex3f(size*0.501 ,  size*d ,  size*d);
		glVertex3f(size*0.501 ,  size*d , -size*d);
		glVertex3f(size*0.501 , -size*d , -size*d);
	
	glEnd();


	/* TRACÉ DU NOIR */
	GLfloat ambient[4]  = {0, 0, 0, 1.f};  // COULEUR AVEC OMBRES
	GLfloat diffuse[4]  = {0, 0, 0, 1.f};  // COULEUR AVEC LUMIERE NORMALE
	GLfloat specular[4] = {0, 0, 0, 1.f};  // COULEUR REFLET
	GLfloat shininess[] = { .0F };

	glMaterialfv(GL_FRONT, GL_AMBIENT , ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE , diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, shininess);	

	d = 0.43;

	glBegin(GL_QUADS);

		// AVANT
		glNormal3f(0,0,1);

		glVertex3f(-size/2 , -size*d , -size/2);
		glVertex3f( size/2 , -size*d , -size/2);
		glVertex3f( size/2 ,  size*d , -size/2);
		glVertex3f(-size/2 ,  size*d , -size/2);

		glVertex3f(-size*d , -size/2 , -size/2);
		glVertex3f( size*d , -size/2 , -size/2);
		glVertex3f( size*d ,  size/2 , -size/2);
		glVertex3f(-size*d ,  size/2 , -size/2);


		// ARRIERE
		glNormal3f(0,0,-1);

		glVertex3f(-size/2 , -size*d , size/2);
		glVertex3f(-size/2 ,  size*d , size/2);
		glVertex3f( size/2 ,  size*d , size/2);
		glVertex3f( size/2 , -size*d , size/2);

		glVertex3f(-size*d , -size/2 , size/2);
		glVertex3f(-size*d ,  size/2 , size/2);
		glVertex3f( size*d ,  size/2 , size/2);
		glVertex3f( size*d , -size/2 , size/2);


		// DESSUS
		glNormal3f(0,-1,0);

		glVertex3f(-size*d , size/2 ,  size/2);
		glVertex3f(-size*d , size/2 , -size/2);
		glVertex3f( size*d , size/2 , -size/2);
		glVertex3f( size*d , size/2 ,  size/2);

		glVertex3f(-size/2 , size/2 ,  size*d);
		glVertex3f(-size/2 , size/2 , -size*d);
		glVertex3f( size/2 , size/2 , -size*d);
		glVertex3f( size/2 , size/2 ,  size*d);


		// DESSOUS
		glNormal3f(0,1,0);

		glVertex3f(-size*d , -size/2 ,  size/2);
		glVertex3f( size*d , -size/2 ,  size/2);
		glVertex3f( size*d , -size/2 , -size/2);
		glVertex3f(-size*d , -size/2 , -size/2);

		glVertex3f(-size/2 , -size/2 ,  size*d);
		glVertex3f( size/2 , -size/2 ,  size*d);
		glVertex3f( size/2 , -size/2 , -size*d);
		glVertex3f(-size/2 , -size/2 , -size*d);


		// GAUCHE
		glNormal3f(1,0,0);

		glVertex3f(-size/2 , -size*d ,  size/2);
		glVertex3f(-size/2 , -size*d , -size/2);
		glVertex3f(-size/2 ,  size*d , -size/2);
		glVertex3f(-size/2 ,  size*d ,  size/2);

		glVertex3f(-size/2 , -size/2 ,  size*d);
		glVertex3f(-size/2 , -size/2 , -size*d);
		glVertex3f(-size/2 ,  size/2 , -size*d);
		glVertex3f(-size/2 ,  size/2 ,  size*d);


		// DROITE
		glNormal3f(-1,0,0);

		glVertex3f(size/2 , -size*d ,  size/2);
		glVertex3f(size/2 ,  size*d ,  size/2);
		glVertex3f(size/2 ,  size*d , -size/2);
		glVertex3f(size/2 , -size*d , -size/2);

		glVertex3f(size/2 , -size/2 ,  size*d);
		glVertex3f(size/2 ,  size/2 ,  size*d);
		glVertex3f(size/2 ,  size/2 , -size*d);
		glVertex3f(size/2 , -size/2 , -size*d);
	
	glEnd();


	/* TRACÉ DES ANGLES ARRONDIS */
	d = 0.425;
	float s = size/10.97;

	glPushMatrix();
	glTranslatef(-size*d , -size*d , -size*d);
	glutSolidSphere(s,10,10);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(-size*d , -size*d ,  size*d);
	glutSolidSphere(s,10,10);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(-size*d ,  size*d , -size*d);
	glutSolidSphere(s,10,10);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(-size*d ,  size*d ,  size*d);
	glutSolidSphere(s,10,10);
	glPopMatrix();
	glPushMatrix();
	glTranslatef( size*d , -size*d , -size*d);
	glutSolidSphere(s,10,10);
	glPopMatrix();
	glPushMatrix();
	glTranslatef( size*d , -size*d , size*d);
	glutSolidSphere(s,10,10);
	glPopMatrix();
	glPushMatrix();
	glTranslatef( size*d ,  size*d , -size*d);
	glutSolidSphere(s,10,10);
	glPopMatrix();
	glPushMatrix();
	glTranslatef( size*d ,  size*d ,  size*d);
	glutSolidSphere(s,10,10);
	glPopMatrix();

	glDisable(GL_LIGHT0);
	glDisable(GL_LIGHTING);

	glPopMatrix();

}






ostream& operator << (ostream& os, const Cube& c)  
{  
    os << "Cube(" << c.posx << "," << c.posy << "," << int(c.size) << ")";
    return os;  
} 