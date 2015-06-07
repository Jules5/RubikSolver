#ifndef COLOR_H
#define COLOR_H

#include <iostream>
#include <GL/glut.h>

#include "Enums.h"

using namespace std;


class Color 
{

	public :

		unsigned char red;
		unsigned char green;
		unsigned char blue;


		Color(unsigned char r=0,unsigned char g=0,unsigned char b=0);
		Color(RubikColor);

		~Color();


		void setGlColor();
		RubikColor getRubikColor();

		bool isIn(Color*, unsigned int);

		friend ostream& operator << (ostream& os, const Color& obj);
		bool operator==(const Color& c);
		bool operator==(const RubikColor& c);

};


#endif