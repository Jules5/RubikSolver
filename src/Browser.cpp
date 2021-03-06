#include <Browser.h>



Browser::Browser(Master* m, TTF_Font* font, string path)
:master(m), font(font), path(path)
{
	run = false;
	cursor = 0;
	width  = master->width*0.5;
	height = master->height*0.7;
	x = (master->width-width)/2;
	y = (master->height-height)/2;
	nb_row = 12;
	title_height = height*0.1;
	file_width  = width*0.9;
	file_height = (height-title_height)/nb_row;
	file_margin_left = width*0.05;
	elevator_width = width*0.05;
	create_file = false;
	creator_height = file_height;
	saisie_texture = 0;
}



Browser::~Browser()
{


}


void Browser::setTitle(string t)
{
	if(t.empty())
		return;

	title = t;

	/* TEXTURE TITLE */
	SDL_Color Color = {222, 222, 222};
	SDL_Surface* surface = TTF_RenderText_Blended(font,title.c_str(),Color);
	texture_title = 0;
	
	glGenTextures(1, &texture_title);
	glBindTexture(GL_TEXTURE_2D, texture_title);
 
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_BGRA,GL_UNSIGNED_BYTE, surface->pixels);

	title_width = height/nb_row*surface->w/surface->h*2;
}



void Browser::display()
{
	glPushMatrix();

	glTranslatef(x,y,0);

	/* CADRE */
	glColor3ub(240,240,240);
	glBegin(GL_QUADS);
		glVertex3f( 0     ,  0      , 1);
		glVertex3f( 0     , height  , 1);
		glVertex3f( width , height  , 1);
		glVertex3f( width ,  0      , 1);
	glEnd();


	/* TITLE */
	glColor3ub(0,0,0);
	glBegin(GL_QUADS);
		glVertex3f( 0     , height               , 0);
		glVertex3f( 0     , height-title_height  , 0);
		glVertex3f( width , height-title_height  , 0);
		glVertex3f( width , height               , 0);
	glEnd();

	glEnable(GL_TEXTURE_2D);
	glColor4ub(255,255,255,255);
	glBindTexture(GL_TEXTURE_2D, texture_title);
	glBegin(GL_QUADS);
		glTexCoord2d(0, 0); glVertex3f( (width-title_width)/2       , height               , 1);
		glTexCoord2d(0, 1); glVertex3f( (width-title_width)/2       , height-title_height  , 1);
		glTexCoord2d(1, 1); glVertex3f( width-(width-title_width)/2 , height-title_height  , 1);
		glTexCoord2d(1, 0); glVertex3f( width-(width-title_width)/2 , height               , 1);
	glEnd();
	glDisable(GL_TEXTURE_2D);


	/* NOMS DE FICHIERS */
	for(unsigned int i=0; i<nb_row && i<files.size(); ++i)
	{
		float d = 0.1;

		float tex_w = 1;
		float bar_w = textures_width[i+cursor];
		bar_w = bar_w-bar_w*d*2;

		if(bar_w > file_width*0.95)
		{
			tex_w = file_width*0.95/bar_w;
			bar_w = file_width*0.95;
		}
		

		if(files_hover[i+cursor])
		{
			float hover_width;
			if(files.size() > nb_row)
				hover_width = file_width+file_margin_left;
			else
				hover_width = width;

			glColor3ub(200,200,200);
			glBegin(GL_QUADS);
			glVertex3f( 0           , height-title_height-i*file_height-file_height       , 0);
			glVertex3f( 0           , height-title_height-(i+1)*file_height+file_height , 0);
			glVertex3f( hover_width , height-title_height-(i+1)*file_height+file_height , 0);
			glVertex3f( hover_width , height-title_height-i*file_height-file_height       , 0);
			glEnd();
		}

		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, textures[i+cursor]);

		glBegin(GL_QUADS);

			glTexCoord2d(0    , 0); glVertex3f( file_margin_left                 , height-title_height-i*file_height-file_height*d      , 0);
			glTexCoord2d(0    , 1); glVertex3f( file_margin_left                 , height-title_height-(i+1)*file_height+file_height*d  , 0);
			glTexCoord2d(tex_w, 1); glVertex3f( file_margin_left+bar_w , height-title_height-(i+1)*file_height+file_height*d  , 0);
			glTexCoord2d(tex_w, 0); glVertex3f( file_margin_left+bar_w , height-title_height-i*file_height-file_height*d      , 0);

		glEnd();

		glDisable(GL_TEXTURE_2D);
	}

	/* ASCENCEUR */
	if(files.size() > nb_row)
	{
		float elevator_height = (float(nb_row)/files.size())*(height-title_height);
		float elevator_pos    = float(cursor)*elevator_height/float(nb_row);

		glBegin(GL_QUADS);
			glColor3ub(200,200,200);
			glVertex3f(width-elevator_width , height-title_height , 2);
			glVertex3f(width-elevator_width , 0                   , 2);
			glVertex3f(width                , 0                   , 2);
			glVertex3f(width                , height-title_height , 2);

			glColor3ub(100,100,100);
			glVertex3f(width-elevator_width , height-title_height-elevator_pos-elevator_height , 3);
			glVertex3f(width-elevator_width , height-title_height-elevator_pos                   , 3);
			glVertex3f(width                , height-title_height-elevator_pos                   , 3);
			glVertex3f(width                , height-title_height-elevator_pos-elevator_height , 3);
		glEnd();
	}

	/* ZONE DE SAISIE */
	if(create_file)
	{
		glBegin(GL_QUADS);
			glColor3ub(255,255,255);
			glVertex3f(0     , -creator_height , 2);
			glVertex3f(0     , 0                   , 2);
			glVertex3f(width , 0                   , 2);
			glVertex3f(width , -creator_height , 2);
		glEnd();

		glLineWidth(3);
		glBegin(GL_LINES);
			glColor3ub(0,0,0);
			glVertex3f(0     , 0 , 20);
			glVertex3f(width , 0 , 20);
		glEnd();

		if(saisie_texture != 0)
		{
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, saisie_texture);

			float tex_w = 1;
			float bar_w = saisie_width;
			bar_w = bar_w;

			if(bar_w > width*0.96)
			{
				tex_w = width*0.96/bar_w;
				bar_w = width*0.96;
			}

			glBegin(GL_QUADS);
				glColor3ub(255,255,255);
				glTexCoord2d(0    , 0); glVertex3f( width*0.02       , 0               , 0);
				glTexCoord2d(0    , 1); glVertex3f( width*0.02       , -creator_height , 0);
				glTexCoord2d(tex_w, 1); glVertex3f( width*0.02+bar_w , -creator_height , 0);
				glTexCoord2d(tex_w, 0); glVertex3f( width*0.02+bar_w ,  0              , 0);
			glEnd();

			glDisable(GL_TEXTURE_2D);
		}
	}


	glPopMatrix();
}



void Browser::updateFiles()
{
	files.clear();
	files_hover.clear();
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
					if(tmp.substr(tmp.size()-4,4) == ".rbk")
					{
						files.push_back(ent->d_name);
						files_hover.push_back(false);
					}
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



string Browser::getFile(bool create)
{
	glDisable(GL_DEPTH_TEST);

	string str = "";
	string saisie = "";
	updateFiles();

	create_file = create;

	if(create_file) 
		buildTextureSaisie(saisie);

	int unicode;
	SDL_Event event;
	run = true;


	SDL_EnableUNICODE(1);


	while(run && str=="")
    {
    	while(SDL_PollEvent(&event))
	    {
	        switch(event.type)
	        {
	        	case SDL_KEYUP :
		        	switch(event.key.keysym.sym)
					{
						case SDLK_ESCAPE : run = false; break;

						case SDLK_RETURN : 
							str = path+saisie; 
							if(str.substr(str.size()-4,4) != ".rbk")
								str += ".rbk";
							break;
					}
					break;

				case SDL_KEYDOWN :
					unicode = event.key.keysym.unicode;
					if(unicode == 8 && saisie.size()>0)
					{
						saisie.erase(saisie.size()-1);
						buildTextureSaisie(saisie);
					}
					else if(unicode >= 32 && unicode <= 127)
					{
						saisie += (char)unicode;
						buildTextureSaisie(saisie);
					}
					break;

				case SDL_MOUSEMOTION     : gestionHover(&event); break;
				case SDL_MOUSEBUTTONDOWN : if(files.size()>nb_row) gestionWheel(&event); break;
				case SDL_MOUSEBUTTONUP   : str = gestionClick(&event); break;
	        }
	    }

    	display();

    	glFlush();
        SDL_GL_SwapBuffers();
    }

    glEnable(GL_DEPTH_TEST);


    SDL_EnableUNICODE(0);


	return str;
}



void Browser::detectChar(SDL_Event* event, string* filename)
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
	
		textures_width.push_back(file_height*surface->w/surface->h);
	}
}



void Browser::buildTextureSaisie(string saisie)
{
	glDeleteTextures(1,&saisie_texture);

	if(saisie.empty())
	{
		saisie_texture = 0;
		return;
	}

	SDL_Color Color = {0, 0, 0};
	SDL_Surface* surface = TTF_RenderText_Blended(font,saisie.c_str(),Color);
	
	glGenTextures(1, &saisie_texture);
	glBindTexture(GL_TEXTURE_2D, saisie_texture);
 
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_BGRA, GL_UNSIGNED_BYTE, surface->pixels);

	saisie_width = file_height*surface->w/surface->h;
}






void Browser::gestionHover(SDL_Event* event)
{
	bool is_in = true;

	if(event->motion.x < x || event->motion.x > x+width-elevator_width
	|| event->motion.y < y+title_height || event->motion.y > y+height)
		is_in = false;
	
	if(is_in)
	{
		float real_x = event->motion.y-(y+title_height);
		unsigned int target = cursor+(int(real_x/file_height));

		if(target < files_hover.size())
			files_hover[target] = true;

		for(unsigned int i=0; i<files_hover.size(); ++i)
			if(i != target)
				files_hover[i] = false;
	}
	else
	{
		for(unsigned int i=0; i<files_hover.size(); ++i)
			files_hover[i] = false;
	}

}



void Browser::gestionWheel(SDL_Event* event)
{
	if(event->button.button == SDL_BUTTON_WHEELDOWN)
	{
		if(cursor < files.size()-nb_row)
			cursor++;
	}
	else if(event->button.button == SDL_BUTTON_WHEELUP)
	{
		if(cursor > 0)
			cursor--;
	}
		
	gestionHover(event);
}



string Browser::gestionClick(SDL_Event* event)
{
	if(!create_file
	&& (event->motion.x < x || event->motion.x > x+width
	|| event->motion.y < y || event->motion.y > y+height))
	{
		run = false;
		return "";
	}

	else if(create_file
	&& (event->motion.x < x || event->motion.x > x+width
	|| event->motion.y < y || event->motion.y > y+height+creator_height))
	{
		run = false;
		return "";
	}

	if(event->button.button == SDL_BUTTON_LEFT)
		for(unsigned int i=0; i<files_hover.size(); ++i)
			if(files_hover[i])
				return path+files[i];

	return "";
}