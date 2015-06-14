#include <Color.h>



Color::Color(unsigned char r, unsigned char g, unsigned char b)
:red(r), green(g), blue(b)
{}



Color::Color(RubikColor c)
{
	switch(c)
	{
		case BLACK :
			red   = 0;
			green = 0;
			blue  = 0;
			break;

		case WHITE :
			red   = 255;
			green = 255;
			blue  = 255;
			break;

		case RED :
			red   = 194;
			green = 1;
			blue  = 21;
			break;

		case BLUE :
			red   = 7;
			green = 24;
			blue  = 179;
			break;

		case GREEN :
			red   = 0;
			green = 155;
			blue  = 72;
			break;

		case ORANGE :
			red   = 255;
			green = 88;
			blue  = 0;
			break;

		case YELLOW :
			red   = 255;
			green = 213;
			blue  = 0;
			break;
	}
}



Color::~Color()
{}




void Color::setGlColor()
{
	glColor3ub(red,green,blue);

	// GLfloat ambient[4]  = {(float)red/255*0.6, (float)green/255*0.6, (float)blue/255*0.6, 1.f};  // COULEUR AVEC OMBRES
	// GLfloat diffuse[4]  = {(float)red/255*0.8, (float)green/255*0.8, (float)blue/255*0.8, 1.f};  // COULEUR AVEC LUMIERE NORMALE
	// GLfloat specular[4] = {0.06, 0.06, 0.06, 1.f};  // COULEUR REFLET
	// GLfloat shininess[] = { 5.0F };

	GLfloat ambient[4]  = {(float)red/255*0.6, (float)green/255*0.6, (float)blue/255*0.6, 1.f};  // COULEUR AVEC OMBRES
	GLfloat diffuse[4]  = {(float)red/255*0.8, (float)green/255*0.8, (float)blue/255*0.8, 1.f};  // COULEUR AVEC LUMIERE NORMALE
	GLfloat specular[4] = {0.6, 0.6, 0.6, 1.f};  // COULEUR REFLET
	GLfloat shininess[] = { 120.0F };

	glMaterialfv(GL_FRONT, GL_AMBIENT , ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE , diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, shininess);	
}




RubikColor Color::getRubikColor()
{
	     if(*this == WHITE)  return WHITE;
	else if(*this == BLUE)   return BLUE;
	else if(*this == ORANGE) return ORANGE;
	else if(*this == GREEN)  return GREEN;
	else if(*this == RED)    return RED;
	else if(*this == YELLOW) return YELLOW;
	else return BLACK;
}



bool Color::isIn(Color* array, unsigned int size)
{
	for(int i=0; i<size; ++i)
		if(*this == array[i])
			return true;
	return false;
}



ostream& operator << (ostream& os, const Color& c)  
{  
    os << "Color(" << int(c.red) << "," << int(c.green) << "," << int(c.blue) << ")";
    return os;  
} 



bool Color::operator==(const Color& c)
{
	if(red==c.red && green==c.green && blue==c.blue)
		return true;
	return false;
}





bool Color::operator==(const RubikColor& r)
{
	Color c(r);
	if(red==c.red && green==c.green && blue==c.blue)
		return true;
	return false;
}