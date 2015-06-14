#include <Editor.h>



Editor::Editor(Master* m, TTF_Font* f)
:master(m), font(f)
{
	current_color = WHITE;

	float width  = master->width;
	float height = master->height;
	float ratio = float(width)/float(height);
	square_size = (ratio>1.6 ? height*0.12 : width*0.07);

	pos_x = width/2;
	pos_y = height/2;

	menu_item_size = square_size * 0.7;
	menu_pos_x = pos_x*1.5;
	menu_pos_y = pos_y+menu_item_size*4.75;
	menu_hover = 0;

	for(int i=1; i<NB_COLORS; ++i)
		for(int j=0; j<9; ++j)
			squares[i][j] = RubikColor(i);

	for(int i=0; i<9; ++i)
		hover[i] = false;

	updateIndices();
	indice_hover = -1;

	buttons.push_back(new Button(width-height*0.05,height,height*0.05,height*0.05,"X",font,this,&Editor::quit));

	buttons.push_back(new Button(width*0.065,height*0.70,width*0.19,height*0.05,"Go to resolver",font,this,&Editor::switchToResolver));
	buttons.push_back(new Button(width*0.10,height*0.55,width*0.12,height*0.05,"   Load   ",font,this,&Editor::loadRubik));
	buttons.push_back(new Button(width*0.10,height*0.40,width*0.12,height*0.05,"   Save   ",font,this,&Editor::saveRubik));
	//buttons.push_back(new Button(width*0.10,height*0.25,width*0.12,height*0.05,"    New    ",font,this,&Editor::resetRubik));
}



Editor::~Editor()
{
	for(vector<Button*>::iterator it=buttons.begin(); it!=buttons.end(); ++it)
		delete *it;
}




void Editor::display()
{
	glDisable(GL_DEPTH_TEST);

	glPushMatrix();

	glTranslatef(pos_x,pos_y,0);

	/* DESSIN DES 9 FACES */
	for(int j=0; j<3; j++)
		for(int i=0; i<3; i++)
		{
			int current = i+j*3;

			float dx = (i-1)*square_size;
			float dy = (2-j-1)*square_size;

			Color(squares[current_color][current]).setGlColor();

			glBegin(GL_QUADS);

			glVertex2f(-square_size/2+dx ,  square_size/2+dy);
			glVertex2f(-square_size/2+dx , -square_size/2+dy);
			glVertex2f( square_size/2+dx , -square_size/2+dy);
			glVertex2f( square_size/2+dx ,  square_size/2+dy);

			glEnd();

			if(hover[i+j*3])
			{
				glColor3ub(0,0,0);
				glLineWidth(4);
				glBegin(GL_LINE_STRIP);
					glVertex2f(-square_size/2+dx , -square_size/2+dy);
					glVertex2f(-square_size/2+dx ,  square_size/2+dy);
					glVertex2f( square_size/2+dx ,  square_size/2+dy);
					glVertex2f( square_size/2+dx , -square_size/2+dy);
					glVertex2f(-square_size/2+dx , -square_size/2+dy);
				glEnd();
			}
		}

		float size = square_size;

		
	/* DESSIN DES INDICES */
	glBegin(GL_QUADS);

		Color(indices[0]).setGlColor();
		glVertex2f(-size/2 , size*3);
		glVertex2f(-size/2 , size*2);
		glVertex2f( size/2 , size*2);
		glVertex2f( size/2 , size*3);

		Color(indices[1]).setGlColor();
		glVertex2f(-size*3 , -size/2);
		glVertex2f(-size*3 ,  size/2);
		glVertex2f(-size*2 ,  size/2);
		glVertex2f(-size*2 , -size/2);

		Color(indices[2]).setGlColor();
		glVertex2f(-size/2 , -size*3);
		glVertex2f(-size/2 , -size*2);
		glVertex2f( size/2 , -size*2);
		glVertex2f( size/2 , -size*3);

		Color(indices[3]).setGlColor();
		glVertex2f(size*3 , -size/2);
		glVertex2f(size*3 ,  size/2);
		glVertex2f(size*2 ,  size/2);
		glVertex2f(size*2 , -size/2);

	glEnd();


	glLineWidth(5);
	glColor3ub(0,0,0);

	if(indice_hover>=0 && indice_hover<=3)
	{
		glBegin(GL_LINE_STRIP);
		switch(indice_hover)
		{
			case 0 :
				glVertex2f(-size/2-2 , size*3+2);
				glVertex2f(-size/2-2 , size*2-2);
				glVertex2f( size/2+2 , size*2-2);
				glVertex2f( size/2+2 , size*3+2);
				glVertex2f(-size/2-2 , size*3+2);
				break;
			case 1 :
				glVertex2f(-size*3-2 , -size/2-2);
				glVertex2f(-size*3-2 ,  size/2+2);
				glVertex2f(-size*2+2 ,  size/2+2);
				glVertex2f(-size*2+2 , -size/2-2);
				glVertex2f(-size*3-2 , -size/2-2);
				break;
			case 2 :
				glVertex2f(-size/2-2 , -size*3-2);
				glVertex2f(-size/2-2 , -size*2+2);
				glVertex2f( size/2+2 , -size*2+2);
				glVertex2f( size/2+2 , -size*3-2);
				glVertex2f(-size/2-2 , -size*3-2);
				break;
			case 3 :
				glVertex2f(size*3+2 , -size/2-2);
				glVertex2f(size*3+2 ,  size/2+2);
				glVertex2f(size*2-2 ,  size/2+2);
				glVertex2f(size*2-2 , -size/2-2);
				glVertex2f(size*3+2 , -size/2-2);
				break;
		}
		glEnd();
	}


	/* ARÊTES */
	
	glBegin(GL_LINES);

		glVertex2f(-size/2*3 ,  size/2);
		glVertex2f( size/2*3 ,  size/2);

		glVertex2f(-size/2*3 , -size/2);
		glVertex2f( size/2*3 , -size/2);

		glVertex2f( size/2 , -size/2*3);
		glVertex2f( size/2 ,  size/2*3);

		glVertex2f(-size/2 , -size/2*3);
		glVertex2f(-size/2 ,  size/2*3);

	glEnd();

	glPopMatrix();


	/* MENU DROITE */
	glBegin(GL_QUADS);
		for(int i=WHITE; i<=YELLOW; ++i)
		{
			Color(RubikColor(i)).setGlColor();
			glVertex2f( menu_pos_x+menu_item_size   , menu_pos_y-(menu_item_size)*i*1.5               );
			glVertex2f( menu_pos_x+menu_item_size   , menu_pos_y-(menu_item_size)*i*1.5+menu_item_size);
			glVertex2f( menu_pos_x+menu_item_size*2 , menu_pos_y-(menu_item_size)*i*1.5+menu_item_size);
			glVertex2f( menu_pos_x+menu_item_size*2 , menu_pos_y-(menu_item_size)*i*1.5               );
		}
	glEnd();

	glColor3ub(180,180,180);
	glLineWidth(5);
	glBegin(GL_LINE_STRIP);
		glVertex2f( menu_pos_x+menu_item_size-2   , menu_pos_y-(menu_item_size)*current_color*1.5-2               );
		glVertex2f( menu_pos_x+menu_item_size-2   , menu_pos_y-(menu_item_size)*current_color*1.5+menu_item_size+2);
		glVertex2f( menu_pos_x+menu_item_size*2+2 , menu_pos_y-(menu_item_size)*current_color*1.5+menu_item_size+2);
		glVertex2f( menu_pos_x+menu_item_size*2+2 , menu_pos_y-(menu_item_size)*current_color*1.5-2               );
		glVertex2f( menu_pos_x+menu_item_size-2   , menu_pos_y-(menu_item_size)*current_color*1.5-2               );
	glEnd();

	if(menu_hover > 0)
	{
		glColor3ub(0,0,0);
		glLineWidth(5);
		glBegin(GL_LINE_STRIP);
			glVertex2f( menu_pos_x+menu_item_size-2   , menu_pos_y-(menu_item_size)*menu_hover*1.5-2               );
			glVertex2f( menu_pos_x+menu_item_size-2   , menu_pos_y-(menu_item_size)*menu_hover*1.5+menu_item_size+2);
			glVertex2f( menu_pos_x+menu_item_size*2+2 , menu_pos_y-(menu_item_size)*menu_hover*1.5+menu_item_size+2);
			glVertex2f( menu_pos_x+menu_item_size*2+2 , menu_pos_y-(menu_item_size)*menu_hover*1.5-2               );
			glVertex2f( menu_pos_x+menu_item_size-2   , menu_pos_y-(menu_item_size)*menu_hover*1.5-2               );
		glEnd();
	}


	/* BUTTONS */
	for(vector<Button*>::iterator it=buttons.begin(); it!=buttons.end(); ++it)
		(*it)->display();

	
	glEnable(GL_DEPTH_TEST);
}




void Editor::quit()
{
	master->is_running = false;
}




void Editor::loadRubik()
{
	master->browser->setTitle("LOAD");
	string filename = master->browser->getFile();

	if(!filename.empty())
	{
		ifstream file((filename).c_str(), ios::in);
        
        if(!file) 
       		cerr << "Échec dans l'ouverture du fichier." << endl;
       	else
       	{
       		unsigned char tmp;
       		for(int i=1; i<NB_COLORS; ++i)
       			for(int j=0; j<9; ++j)
       			{
       				file >> tmp;
       				squares[i][j] = RubikColor(tmp);
       			}

       		file.close();
       	}
	}
}





void Editor::saveRubik()
{
	master->browser->setTitle("SAVE");
	string filename = master->browser->getFile(true);
	
	if(!filename.empty())
	{
		ofstream file((filename).c_str(), ios::out | ios::trunc);
        
        if(!file)
       		cerr << "Échec dans l'ouverture du fichier." << endl;
       	else
       	{
       		for(int i=1; i<NB_COLORS; ++i)
       			for(int j=0; j<9; ++j)
       				file << (unsigned char)(squares[i][j]);

       		file.close();
       	}
	}
}




void Editor::resetRubik()
{
	current_color = WHITE;

	for(int i=1; i<NB_COLORS; ++i)
		for(int j=0; j<9; ++j)
			squares[i][j] = RubikColor(i);

	for(int i=0; i<9; ++i)
		hover[i] = false;

	updateIndices();
	indice_hover = -1;
}



void Editor::switchToResolver()
{
	master->switchMode();
}




void Editor::updateIndices()
{
	switch(current_color)
	{
		case WHITE  : indices[0]=BLUE;   indices[1]=ORANGE; indices[2]=GREEN;  indices[3]=RED;    break;
		case RED    : indices[0]=BLUE;   indices[1]=WHITE;  indices[2]=GREEN;  indices[3]=YELLOW; break;
		case BLUE   : indices[0]=YELLOW; indices[1]=ORANGE; indices[2]=WHITE;  indices[3]=RED;    break;
		case GREEN  : indices[0]=WHITE;  indices[1]=ORANGE; indices[2]=YELLOW; indices[3]=RED;    break;
		case ORANGE : indices[0]=BLUE;   indices[1]=YELLOW; indices[2]=GREEN;  indices[3]=WHITE;  break;
		case YELLOW : indices[0]=BLUE;   indices[1]=RED;    indices[2]=GREEN;  indices[3]=ORANGE; break;
	}
}




RubikColor Editor::setNextColor(RubikColor color)
{
	int tmp = int(color)+1;
	if(tmp >= NB_COLORS)
		return WHITE;
	else
		return RubikColor(tmp);
}


RubikColor Editor::setPrevColor(RubikColor color)
{
	int tmp = int(color)-1;
	if(tmp < WHITE)
		return RubikColor(NB_COLORS-1);
	else
		return RubikColor(tmp);
}




void Editor::gestionEvents(SDL_Event* event)
{
	switch(event->type)
	{
        case SDL_MOUSEBUTTONUP   : gestionClickButtons(event); break;
        case SDL_MOUSEBUTTONDOWN : gestionWheel(event); break;; break;
        case SDL_MOUSEMOTION     : gestionMouseMotion(event); break;
        case SDL_KEYUP           : gestionButton(event); break;
    }
}




void Editor::gestionClickButtons(SDL_Event* event)
{
	for(int i=0; i<9; ++i)
		if(i!=4 && hover[i])
		{
			switch(event->button.button)
        	{
        		case SDL_BUTTON_LEFT  : squares[current_color][i] = setNextColor(squares[current_color][i]); break;
        		case SDL_BUTTON_RIGHT : squares[current_color][i] = setPrevColor(squares[current_color][i]); break;
        	}
			break; 
		}

	for(vector<Button*>::iterator it=buttons.begin(); it!=buttons.end(); ++it)
	{
		if(event->motion.x>=(*it)->posx && event->motion.x<=(*it)->posx+(*it)->width)
			if(event->motion.y>=master->height-(*it)->posy && event->motion.y<=master->height-(*it)->posy+(*it)->height)
			{
				(*it)->click();
				break;
			}
	}

	if(menu_hover != 0)
	{
		current_color = RubikColor(menu_hover);
		updateIndices();
	}

	if(indice_hover>=0 && indice_hover<=3)
	{
		current_color = indices[indice_hover];
		updateIndices();
	}
}



void Editor::gestionMouseMotion(SDL_Event* event)
{
	int x = event->motion.x;
	int y = event->motion.y;

	/* SURVOL FACES */
	for(int j=0; j<3; ++j)
		for(int i=0; i<3; ++i)
		{
			if( ( i != 1 || j != 1 ) 
			&& x >= pos_x+(i-1)*square_size-square_size/2 && x <= pos_x+(i-1)*square_size+square_size/2
			&& y >= pos_y-(2-j-1)*square_size-square_size/2 && y <= pos_y-(2-j-1)*square_size+square_size/2)
				hover[i+j*3] = true;
			else
				hover[i+j*3] = false;
		}

	/* SURVOL BUTTONS */
	for(vector<Button*>::iterator it=buttons.begin(); it!=buttons.end(); ++it)
	{
		bool hover = false;
		if(x>=(*it)->posx && x<=(*it)->posx+(*it)->width)
			if(y>=master->height-(*it)->posy && y<=master->height-(*it)->posy+(*it)->height)
				hover = true;
		(*it)->hover = hover;
	}

	/* SURVOL MENU */
	menu_hover = 0;
	if(x >= menu_pos_x+menu_item_size && x <= menu_pos_x+menu_item_size*2)
		for(int i=WHITE; i<NB_COLORS; ++i)
		{
			if(i == current_color)
				continue;
			else if(y >= (menu_item_size)*i*1.5 && y <= (menu_item_size)*i*1.5+menu_item_size)
			{
				menu_hover = i;
				break;
			}
		}

	/* SURVOL INDICES */
	indice_hover = -1;
	if(x >= pos_x-square_size*3 && x <= pos_x+square_size*3
	&& y >= pos_y-square_size*3 && y <= pos_y+square_size*3)
	{
		if(x >= pos_x-square_size/2 && x <= pos_x+square_size/2
		&& y >= pos_y-square_size*3 && y <= pos_y-square_size*2)
			indice_hover = 0;
		
		else if(x >= pos_x-square_size*3 && x <= pos_x-square_size*2
		&& y >= pos_y-square_size/2 && y <= pos_y+square_size/2)
			indice_hover = 1;
		
		else if(x >= pos_x-square_size/2 && x <= pos_x+square_size/2
		&& y >= pos_y+square_size*2 && y <= pos_y+square_size*3)
			indice_hover = 2;
		
		else if(x >= pos_x+square_size*2 && x <= pos_x+square_size*3
		&& y >= pos_y-square_size/2 && y <= pos_y+square_size/2)
			indice_hover = 3;
	}
}



void Editor::gestionButton(SDL_Event* event)
{
	switch(event->key.keysym.sym)
	{
		case SDLK_ESCAPE : 
			master->is_running = false;
			break;

		case SDLK_UP   : current_color = setPrevColor(current_color); break;
		case SDLK_DOWN : current_color = setNextColor(current_color); break;
	}
}



void Editor::gestionWheel(SDL_Event* event)
{
	if(event->button.button == SDL_BUTTON_WHEELDOWN)
		current_color = setNextColor(current_color);
	else if(event->button.button == SDL_BUTTON_WHEELUP)
		current_color = setPrevColor(current_color);

	updateIndices();
	gestionMouseMotion(event);
}