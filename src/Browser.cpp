#include <Browser.h>



Browser::Browser(Master* m, TTF_Font* font, string path)
:master(m), font(font), path(path)
{
	cursor = 0;
	width  = master->width*0.25;
	height = master->height*0.7;
	x = (master->width-width)/2;
	y = (master->height-height)/2;
	nb_row = 15;
}



Browser::~Browser()
{


}




void Browser::display()
{
	glPushMatrix();

	glTranslatef(x,y,0);

	/* CADRE */
	glBegin(GL_QUADS);

		glColor3ub(240,240,240);

		glVertex3f( 0     ,  0      , 1);
		glVertex3f( 0     , height  , 1);
		glVertex3f( width , height  , 1);
		glVertex3f( width ,  0      , 1);

	glEnd();

	/* NOMS DE FICHIERS ET DELIMITATIONS */
	glEnable(GL_TEXTURE_2D);

	for(unsigned int i=0; i<10 && i<files.size(); ++i)
	{
		glBindTexture(GL_TEXTURE_2D, textures[i]);

		glBegin(GL_QUADS);

			glTexCoord2d(0, 1); glVertex3f( 0                 , i*height/nb_row                , 0);
			glTexCoord2d(0, 0); glVertex3f( 0                 , i*height/nb_row+height/nb_row  , 0);
			glTexCoord2d(1, 0); glVertex3f( textures_width[i] , i*height/nb_row+height/nb_row  , 0);
			glTexCoord2d(1, 1); glVertex3f( textures_width[i] , i*height/nb_row                , 0);

		glEnd();
	}


	glPopMatrix();
}



void Browser::updateFiles()
{
	files.clear();
	cursor = 0;

	DIR *dir;
	struct dirent *ent;
	if(dir = opendir(path.c_str())) 
	{
		while(ent = readdir(dir)) 
		{
			string tmp = ent->d_name;
			if(tmp != ".")
				if(tmp != "..")
					files.push_back(ent->d_name);
		}
		closedir (dir);
	}

	sortFiles();
	buildTextures();
}



void Browser::sortFiles()
{
	for(unsigned int i=0; i<files.size(); ++i)
	{
		string tmp = files[i];
		unsigned int j = i;
		while(j > 0 && files[j-1] > tmp)
		{
			files[j] = files[j-1];
			j--;
		}
		files[j] = tmp;
	}
}



string Browser::getFile()
{
	glDisable(GL_DEPTH_TEST);

	string str = "";
	updateFiles();
	SDL_Event event;
	bool run = true;

	while(run)
    {
    	while(SDL_PollEvent(&event))
	    {
	        switch(event.type)
	        {
	        	case SDL_KEYUP :
		        	switch(event.key.keysym.sym)
					{
						case SDLK_ESCAPE : 
							run = false;
							break;

						case SDLK_KP_PLUS :
							nb_row++;
							break;

						case SDLK_KP_MINUS :
							nb_row--;
							break;
					}
					break;
	        }
	    }

    	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// animate(dt);
		display();

    	glFlush();
        SDL_GL_SwapBuffers();
    }

    glEnable(GL_DEPTH_TEST);
		
	return str;
}





void Browser::buildTextures()
{
	for(vector<GLuint>::iterator it=textures.begin(); it!=textures.end(); ++it)
		glDeleteTextures(1,&(*it));

	textures.clear();
	textures_width.clear();

	for(unsigned int i=0; i<files.size(); ++i)
	{
		SDL_Color Color = {0, 0, 0};
		SDL_Surface* surface = TTF_RenderText_Blended(font,files[i].c_str(),Color);
		textures.push_back(0);
		
		glGenTextures(1, &textures[i]);
		glBindTexture(GL_TEXTURE_2D, textures[i]);
	 
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_BGRA, GL_UNSIGNED_BYTE, surface->pixels);
	
		// if(surface->w > width) 
		// 	textures_width.push_back(width/surface->w);
		// else
		// 	textures_width.push_back(1);

		textures_width.push_back(height/nb_row*surface->w/surface->h);
	}
}