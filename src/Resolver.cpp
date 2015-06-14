#include <Resolver.h>



const unsigned int Resolver::NB_STEPS = 7;



Resolver::Resolver(Master* m, TTF_Font* f)
:master(m), font(f)
{
	/* INIT RESOLVER */
	init();

	width = master->width;
	height = master->height;

	float ratio = float(width)/float(height);

	rubik_pos_x = width*0.5;
	rubik_pos_y = height*0.5;
	rubik_size = (ratio>1.6 ? height*0.40 : width*0.25);

	/* INIT TEXTURES */
	tex_play  = loadTexture((master->path+"/res/tex/play.png").c_str());
	tex_pause = loadTexture((master->path+"/res/tex/pause.png").c_str());
	tex_fast  = loadTexture((master->path+"/res/tex/fast.png").c_str());
	tex_next  = loadTexture((master->path+"/res/tex/next.png").c_str());

	/* RUBIK */
	rubik_view = new Rubik3x3(rubik_pos_x,rubik_pos_y,rubik_size);
	rubik_test = new Rubik3x3();

	/* BUTTONS */
	buttons.push_back(new Button(width*0.78,height*0.70,width*0.12,height*0.05," Reset ",font,this,&Resolver::resetRubik));
	buttons.push_back(new Button(width*0.78,height*0.55,width*0.12,height*0.05,"Resolve",font,this,&Resolver::setResolveMode));
	buttons.push_back(new Button(width*0.78,height*0.40,width*0.12,height*0.05,"Shuffle",font,this,&Resolver::shuffle));
	
	buttons.push_back(new Button(width-height*0.05,height,height*0.05,height*0.05,"X",font,this,&Resolver::quit));
	
	buttons.push_back(new Button(width*0.4,height*0.95,width*0.2,height*0.05,"Initial position",font,this,&Resolver::resetRubikRotation));

	buttons.push_back(new Button(width*0.08,height*0.70,width*0.16,height*0.05,"Go to editor",font,this,&Resolver::switchToEditor));
	buttons.push_back(new Button(width*0.10,height*0.55,width*0.12,height*0.05,"   Load   ",font,this,&Resolver::loadRubik));
	buttons.push_back(new Button(width*0.10,height*0.40,width*0.12,height*0.05,"   Save   ",font,this,&Resolver::saveRubik));

	/* PLAYER */
	player.push_back(new Button(width*0.5-(height*0.245),height*0.08,height*0.05,height*0.05,tex_fast,true,this,&Resolver::playFastReverse));
	player.push_back(new Button(width*0.5-(height*0.14),height*0.08,height*0.05,height*0.05,tex_next,true,this,&Resolver::playOneReverse));
	player.push_back(new Button(width*0.5-(height*0.035),height*0.08,height*0.05,height*0.05,tex_play,false,this,&Resolver::play));
	player.push_back(new Button(width*0.5+(height*0.07),height*0.08,height*0.05,height*0.05,tex_next,false,this,&Resolver::playOne));
	player.push_back(new Button(width*0.5+(height*0.175),height*0.08,height*0.05,height*0.05,tex_fast,false,this,&Resolver::playFast));

	player_bar_w = width*0.75;
	player_bar_h = height*0.005;
	player_cursor_x = 0;
	player_cursor_hover = false;

	speed_rubik_normal = 4;
	speed_rubik_fast   = 6;
}



Resolver::~Resolver()
{
	delete rubik_view;
	delete rubik_test;

	for(vector<Button*>::iterator it=buttons.begin(); it!=buttons.end(); ++it)
		delete *it;

	for(vector<Button*>::iterator it=player.begin(); it!=player.end(); ++it)
		delete *it;
}



void Resolver::init()
{
	current_step = 0;
	state = SHOW;
	move = false;
	show_player = false;
}




void Resolver::animate(int dt)
{
	if(show_player && rubik_view->state == WAIT)
		pause();

	if(state == RESOLVE)
		resolve();

	rubik_view->animate(dt);
}







void Resolver::display()
{
	rubik_view->display();

	for(vector<Button*>::iterator it=buttons.begin(); it!=buttons.end(); ++it)
		(*it)->display();

	if(show_player)
		displayPlayer();
}




void Resolver::displayPlayer()
{
	for(vector<Button*>::iterator it=player.begin(); it!=player.end(); ++it)
		(*it)->display();

	unsigned int nb  = rubik_view->moves_save.size();
	unsigned int tot = rubik_view->moves.size() + nb;

	float bar_cur;

	if(!drag_player_cursor)
	{
		bar_cur = nb*player_bar_w/tot;
		player_cursor_x = bar_cur;
	}
	else
	{
		bar_cur = player_cursor_x;
		if(bar_cur < 0)                 bar_cur = 0;
		else if(bar_cur > player_bar_w) bar_cur = player_bar_w;
	}

	glPushMatrix();
	glTranslatef(width*0.25/2,height*0.12,0);

	/* TRACÉ DE LA BARRE DE PROGRESSION (PREV) */
	if(player_cursor_x > 0)
	{
		glBegin(GL_QUADS);

			glColor3ub(100,100,100);

			glVertex3f( 0       , 0            ,0);
			glVertex3f( 0       , player_bar_h ,0);
			glVertex3f( bar_cur , player_bar_h ,0);
			glVertex3f( bar_cur , 0            ,0);

		glEnd();
	}

	/* TRACÉ DE LA BARRE DE PROGRESSION (NEXT) */
	if(bar_cur < player_bar_w)
	{
		glBegin(GL_QUADS);

			glColor3ub(240,240,240);

			glVertex3f( bar_cur      , 0            ,0);
			glVertex3f( bar_cur      , player_bar_h ,0);
			glVertex3f( player_bar_w , player_bar_h ,0);
			glVertex3f( player_bar_w , 0            ,0);

		glEnd();
	}

	/* TRACÉ DU CURSEUR */
	glBegin(GL_QUADS);

		if(!player_cursor_hover)
			glColor3ub(255,255,255);
		else
			glColor3ub(150,150,150);

		glVertex3f( bar_cur-width*0.005 , -height*0.01              , -1);
		glVertex3f( bar_cur-width*0.005 ,  height*0.01+player_bar_h , -1);
		glVertex3f( bar_cur+width*0.005 ,  height*0.01+player_bar_h , -1);
		glVertex3f( bar_cur+width*0.005 , -height*0.01              , -1);

	glEnd();

	glPopMatrix();
}




void Resolver::gestionEvents(SDL_Event* event)
{
	switch(event->type)
	{
		case SDL_MOUSEBUTTONDOWN : gestionClickButtons(event,false); break;
        case SDL_MOUSEBUTTONUP   : gestionClickButtons(event,true); break;
        case SDL_MOUSEMOTION     : gestionMouseMotion(event); break;
        case SDL_KEYUP           : gestionButton(event); break;
    }
}




void Resolver::gestionClickButtons(SDL_Event* event, bool click)
{
	if(click && move)
	{
		move = false;
		return;
	}

	if(click && drag_player_cursor)
	{
		drag_player_cursor = false;
		player_cursor_hover = false;
		return;
	}

	for(vector<Button*>::iterator it=buttons.begin(); it!=buttons.end(); ++it)
	{
		if(event->motion.x>=(*it)->posx && event->motion.x<=(*it)->posx+(*it)->width)
			if(event->motion.y>=height-(*it)->posy && event->motion.y<=height-(*it)->posy+(*it)->height)
			{
				if(click)
					(*it)->click();
				move = false;
				drag_player_cursor = false;
				return;
			}
	}

	if(show_player)
	{
		/* BUTTONS PLAYER */
		for(vector<Button*>::iterator it=player.begin(); it!=player.end(); ++it)
		{
			if(event->motion.x>=(*it)->posx && event->motion.x<=(*it)->posx+(*it)->width)
				if(event->motion.y>=height-(*it)->posy && event->motion.y<=height-(*it)->posy+(*it)->height)
				{
					if(click)
						(*it)->click();
					move = false;
					drag_player_cursor = false;
					return;
				}
		}

		/* CURSOR PLAYER */
		if(!click)
		{
			float cursor_x = (width-player_bar_w)/2+rubik_view->moves_save.size()*player_bar_w/(rubik_view->moves_save.size()+rubik_view->moves.size());
			float cursor_y = height*0.12;

			if(event->motion.x>=cursor_x-width*0.005 && event->motion.x<=cursor_x+width*0.005)
				if(event->motion.y>=height-cursor_y-height*0.01-player_bar_h && event->motion.y<=height-cursor_y+height*0.01)
				{
					drag_player_cursor = true;
					player_cursor_hover = true;
					pause();
					return;
				}
		}

	}

	if(!click)
		move = true;
}



void Resolver::gestionSurvolButtons(SDL_Event* event)
{//cout << event->motion.x << endl;
	for(vector<Button*>::iterator it=buttons.begin(); it!=buttons.end(); ++it)
	{
		bool hover = false;
		if(event->motion.x>=(*it)->posx && event->motion.x<=(*it)->posx+(*it)->width)
			if(event->motion.y>=height-(*it)->posy && event->motion.y<=height-(*it)->posy+(*it)->height)
				hover = true;
		(*it)->hover = hover;
	}

	if(show_player)
	{
		for(vector<Button*>::iterator it=player.begin(); it!=player.end(); ++it)
		{
			bool hover = false;
			if(event->motion.x>=(*it)->posx && event->motion.x<=(*it)->posx+(*it)->width)
				if(event->motion.y>=height-(*it)->posy && event->motion.y<=height-(*it)->posy+(*it)->height)
					hover = true;
			(*it)->hover = hover;
		}

		float cursor_x = (width-player_bar_w)/2+rubik_view->moves_save.size()*player_bar_w/(rubik_view->moves_save.size()+rubik_view->moves.size());
		float cursor_y = height*0.12;
		
		bool hover = false;
		if(event->motion.x>=cursor_x-width*0.005 && event->motion.x<=cursor_x+width*0.005)
			if(event->motion.y>=height-cursor_y-height*0.01-player_bar_h && event->motion.y<=height-cursor_y+height*0.01)
				hover = true;
		player_cursor_hover = hover;
	}
}




void Resolver::gestionMouseMotion(SDL_Event* event)
{
	if(move)
		rubik_view->rotate(event->motion.xrel,event->motion.yrel);
	else if(drag_player_cursor)
	{
		player_cursor_x += event->motion.xrel;

		float bar_cur = player_cursor_x;

		if(bar_cur < 0)                 bar_cur = 0;
		else if(bar_cur > player_bar_w) bar_cur = player_bar_w;

		unsigned int nb  = rubik_view->moves_save.size();
		unsigned int tot = rubik_view->moves.size() + nb;
		unsigned new_nb = bar_cur*tot/player_bar_w;
		
		if(new_nb > nb)
			while(rubik_view->moves.size() > 0 && rubik_view->moves_save.size() < new_nb)
			{
				rubik_view->updateCubesPositions(rubik_view->moves.front());
				RubikMoves tmp = rubik_view->moves.front();
				rubik_view->moves_save.push_back(tmp);
				rubik_view->moves.pop_front();
			}
		else
			while(rubik_view->moves_save.size() > 0 && rubik_view->moves_save.size() > new_nb)
			{
				rubik_view->updateCubesPositions(rubik_view->getContraryMove(rubik_view->moves_save.back()));
				RubikMoves tmp = rubik_view->moves_save.back();
				rubik_view->moves.push_front(tmp);
				rubik_view->moves_save.pop_back();
			}
	}
    else
    	gestionSurvolButtons(event);
}



void Resolver::gestionButton(SDL_Event* event)
{
	switch(event->key.keysym.sym)
	{
		case SDLK_ESCAPE : 
			master->is_running = false;
			break;

		case SDLK_SPACE :
			setResolveMode();
			break;
	}
}




void Resolver::play()
{
	if(rubik_view->moves.size() > 0)
	{
		rubik_view->state = MOVE;
		rubik_view->speed = speed_rubik_normal;

		player[PLAY]->texture = tex_pause;
		player[PLAY]->callback_resolver = &Resolver::pause;
	}
}


void Resolver::playReverse()
{
	if(rubik_view->moves_save.size() > 0)
	{
		rubik_view->state = MOVE_REV;
		rubik_view->speed = speed_rubik_normal;

		player[PLAY]->texture = tex_pause;
		player[PLAY]->callback_resolver = &Resolver::pause;
	}
}



void Resolver::pause()
{
	switch(rubik_view->state)
	{
		case MOVE     :  rubik_view->state = MOVE_ONE;     break;
		case MOVE_REV :  rubik_view->state = MOVE_REV_ONE; break;
	}

	player[PLAY]->texture = tex_play;
	player[PLAY]->callback_resolver = &Resolver::play;
}


void Resolver::playFast()
{
	if(rubik_view->moves.size() > 0)
	{
		play();
		rubik_view->speed = speed_rubik_fast;
	}
}

void Resolver::playFastReverse()
{
	if(rubik_view->moves_save.size() > 0)
	{
		playReverse();
		rubik_view->speed = speed_rubik_fast;
	}
}

void Resolver::playOne()
{
	if(rubik_view->moves.size() > 0)
	{
		play();
		rubik_view->state = MOVE_ONE;
	}

	player[PLAY]->texture = tex_play;
	player[PLAY]->callback_resolver = &Resolver::play;
}

void Resolver::playOneReverse()
{
	if(rubik_view->moves_save.size() > 0)
	{
		playOne();
		rubik_view->state = MOVE_REV_ONE;
	}

	player[PLAY]->texture = tex_play;
	player[PLAY]->callback_resolver = &Resolver::play;
}


void Resolver::quit()
{
	master->is_running = false;
}




void Resolver::shuffle()
{
	if(state == SHOW && rubik_view->state!=MOVE)
	{
		rubik_view->shuffle();
		rubik_view->state = MOVE;
		rubik_view->speed = 10;
		show_player = false;
	}
}



void Resolver::setResolveMode()
{
	if(rubik_view->state == WAIT)
	{
		current_step=0;
		state=RESOLVE;
		rubik_view->moves.clear();
		rubik_view->moves_save.clear();
		rubik_test->moves.clear();
		rubik_test->moves_save.clear();
		rubik_test->loadRubikFromOther(rubik_view);
	}
}



void Resolver::resetRubikRotation()
{
	rubik_view->reinit_position = true;
}


void Resolver::resetRubik()
{
	current_step = 0;
	state = SHOW;
	move = false;
	show_player = false;

	delete rubik_view;
	rubik_view = new Rubik3x3(rubik_pos_x,rubik_pos_y,rubik_size);
}



void Resolver::switchToEditor()
{
	master->switchMode();
}


void Resolver::saveRubik()
{
	master->browser->setTitle("SAVE");
	string file = master->browser->getFile(true);
	
	if(!file.empty())
		rubik_view->saveIntoFile(file);
}


void Resolver::loadRubik()
{
	master->browser->setTitle("LOAD");
	string file = master->browser->getFile();

	if(!file.empty())
	{
		delete rubik_view;
		init();
		rubik_view = new Rubik3x3(rubik_pos_x,rubik_pos_y,rubik_size,file);
	}
}




Cube* Resolver::getCube(Cube** tab, RubikColor color, TypeCube type)
{
	unsigned int nb_colors = 0;

	switch(type)
	{
		case CENTER : nb_colors = 1; break;
		case EDGE   : nb_colors = 2; break;
		case CORNER : nb_colors = 3; break;
	}

	for(int i=0; i<9; ++i)
		if(type==UNKNOW_TYPE || tab[i]->type == type)
		{
			Color* colors = tab[i]->getColors();
			for(unsigned int c=0; c<nb_colors; ++c)
			{
				if(colors[c] == color)
					return tab[i];
			}

		}

	return NULL;
}





vector<Cube*> Resolver::getCubes(Cube** tab, RubikColor color, TypeCube type)
{
	vector<Cube*> cubes;

	unsigned int nb_colors = 0;

	switch(type)
	{
		case CENTER : nb_colors = 1; break;
		case EDGE   : nb_colors = 2; break;
		case CORNER : nb_colors = 3; break;
	}

	for(int i=0; i<9; ++i)
		if(type==UNKNOW_TYPE || tab[i]->type == type)
		{
			Color* colors = tab[i]->getColors();
			for(unsigned int c=0; c<nb_colors; ++c)
			{
				if(colors[c] == color)
					cubes.push_back(tab[i]);
			}

		}

	return cubes;
}







Cube* Resolver::getNextPosition(Rubik3x3* rubik, Cube* cube, FacesCube face, bool way, unsigned int nb_rot)
{
	bool is_in_face = false;

	for(int i=0; i<9; ++i)
		if(rubik->moves_set[face][i] == cube)
			is_in_face = true;

	if(!is_in_face || cube->type==CENTER)
		return cube;


	if(nb_rot > 0)
		cube = getNextPosition(rubik,cube,face,way,nb_rot-1);


	if(face == FRONT)
	{
		if(cube->type==EDGE)
		{
			if(cube->voisins[TOP] == NULL)         return (!way ? cube->voisins[LEFT]->voisins[BOTTOM]  : cube->voisins[RIGHT]->voisins[BOTTOM]);
			else if(cube->voisins[BOTTOM] == NULL) return (!way ? cube->voisins[RIGHT]->voisins[TOP]    : cube->voisins[LEFT]->voisins[TOP]);
			else if(cube->voisins[LEFT]   == NULL) return (!way ? cube->voisins[BOTTOM]->voisins[RIGHT] : cube->voisins[TOP]->voisins[RIGHT]);
			else if(cube->voisins[RIGHT]  == NULL) return (!way ? cube->voisins[TOP]->voisins[LEFT]     : cube->voisins[BOTTOM]->voisins[LEFT]);
		}
		else if(cube->type==CORNER)
		{
			if(cube->voisins[TOP] == NULL && cube->voisins[LEFT] == NULL )  
				return (!way ? cube->voisins[BOTTOM]->voisins[BOTTOM]  : cube->voisins[RIGHT]->voisins[RIGHT]);
			else if(cube->voisins[RIGHT] == NULL && cube->voisins[TOP] == NULL )  
				return (!way ? cube->voisins[LEFT]->voisins[LEFT]  : cube->voisins[BOTTOM]->voisins[BOTTOM]);
			else if(cube->voisins[BOTTOM] == NULL && cube->voisins[RIGHT] == NULL )  
				return (!way ? cube->voisins[TOP]->voisins[TOP]  : cube->voisins[LEFT]->voisins[LEFT]);
			else if(cube->voisins[LEFT] == NULL && cube->voisins[BOTTOM] == NULL )  
				return (!way ? cube->voisins[RIGHT]->voisins[RIGHT]  : cube->voisins[TOP]->voisins[TOP]);
		}
	}
	if(face == BACK)
	{
		if(cube->type==EDGE)
		{
			if(cube->voisins[TOP] == NULL)         return (way ? cube->voisins[LEFT]->voisins[BOTTOM]  : cube->voisins[RIGHT]->voisins[BOTTOM]);
			else if(cube->voisins[BOTTOM] == NULL) return (way ? cube->voisins[RIGHT]->voisins[TOP]    : cube->voisins[LEFT]->voisins[TOP]);
			else if(cube->voisins[LEFT]   == NULL) return (way ? cube->voisins[BOTTOM]->voisins[RIGHT] : cube->voisins[TOP]->voisins[RIGHT]);
			else if(cube->voisins[RIGHT]  == NULL) return (way ? cube->voisins[TOP]->voisins[LEFT]     : cube->voisins[BOTTOM]->voisins[LEFT]);
		}
		else if(cube->type==CORNER)
		{
			if(cube->voisins[TOP] == NULL && cube->voisins[LEFT] == NULL )  
				return (way ? cube->voisins[BOTTOM]->voisins[BOTTOM]  : cube->voisins[RIGHT]->voisins[RIGHT]);
			else if(cube->voisins[RIGHT] == NULL && cube->voisins[TOP] == NULL )  
				return (way ? cube->voisins[LEFT]->voisins[LEFT]  : cube->voisins[BOTTOM]->voisins[BOTTOM]);
			else if(cube->voisins[BOTTOM] == NULL && cube->voisins[RIGHT] == NULL )  
				return (way ? cube->voisins[TOP]->voisins[TOP]  : cube->voisins[LEFT]->voisins[LEFT]);
			else if(cube->voisins[LEFT] == NULL && cube->voisins[BOTTOM] == NULL )  
				return (way ? cube->voisins[RIGHT]->voisins[RIGHT]  : cube->voisins[TOP]->voisins[TOP]);
		}
	}
	if(face == TOP)
	{
		if(cube->type==EDGE)
		{
			if(cube->voisins[FRONT] == NULL)      return (way ? cube->voisins[LEFT]->voisins[BACK]   : cube->voisins[RIGHT]->voisins[BACK]);
			else if(cube->voisins[BACK]  == NULL) return (way ? cube->voisins[RIGHT]->voisins[FRONT] : cube->voisins[LEFT]->voisins[FRONT]);
			else if(cube->voisins[LEFT]  == NULL) return (way ? cube->voisins[BACK]->voisins[RIGHT]  : cube->voisins[FRONT]->voisins[RIGHT]);
			else if(cube->voisins[RIGHT] == NULL) return (way ? cube->voisins[FRONT]->voisins[LEFT]  : cube->voisins[BACK]->voisins[LEFT]);
		}
		else if(cube->type==CORNER)
		{
			if(cube->voisins[FRONT] == NULL && cube->voisins[LEFT] == NULL )  
				return (way ? cube->voisins[BACK]->voisins[BACK]  : cube->voisins[LEFT]->voisins[LEFT]);
			else if(cube->voisins[RIGHT] == NULL && cube->voisins[FRONT] == NULL )  
				return (way ? cube->voisins[LEFT]->voisins[LEFT]  : cube->voisins[BACK]->voisins[BACK]);
			else if(cube->voisins[BACK] == NULL && cube->voisins[RIGHT] == NULL )  
				return (way ? cube->voisins[FRONT]->voisins[FRONT]  : cube->voisins[LEFT]->voisins[LEFT]);
			else if(cube->voisins[LEFT] == NULL && cube->voisins[BACK] == NULL )  
				return (way ? cube->voisins[RIGHT]->voisins[RIGHT]  : cube->voisins[FRONT]->voisins[FRONT]);
		}
	}
	if(face == BOTTOM)
	{
		if(cube->type==EDGE)
		{
			if(cube->voisins[FRONT] == NULL)      return (!way ? cube->voisins[LEFT]->voisins[BACK]   : cube->voisins[RIGHT]->voisins[BACK]);
			else if(cube->voisins[BACK]  == NULL) return (!way ? cube->voisins[RIGHT]->voisins[FRONT] : cube->voisins[LEFT]->voisins[FRONT]);
			else if(cube->voisins[LEFT]  == NULL) return (!way ? cube->voisins[BACK]->voisins[RIGHT]  : cube->voisins[FRONT]->voisins[RIGHT]);
			else if(cube->voisins[RIGHT] == NULL) return (!way ? cube->voisins[FRONT]->voisins[LEFT]  : cube->voisins[BACK]->voisins[LEFT]);
		}
		else if(cube->type==CORNER)
		{
			if(cube->voisins[FRONT] == NULL && cube->voisins[LEFT] == NULL )  
				return (!way ? cube->voisins[BACK]->voisins[BACK]  : cube->voisins[LEFT]->voisins[LEFT]);
			else if(cube->voisins[RIGHT] == NULL && cube->voisins[FRONT] == NULL )  
				return (!way ? cube->voisins[LEFT]->voisins[LEFT]  : cube->voisins[BACK]->voisins[BACK]);
			else if(cube->voisins[BACK] == NULL && cube->voisins[RIGHT] == NULL )  
				return (!way ? cube->voisins[FRONT]->voisins[FRONT]  : cube->voisins[LEFT]->voisins[LEFT]);
			else if(cube->voisins[LEFT] == NULL && cube->voisins[BACK] == NULL )  
				return (!way ? cube->voisins[RIGHT]->voisins[RIGHT]  : cube->voisins[FRONT]->voisins[FRONT]);
		}
	}
	if(face == LEFT)
	{
		if(cube->type==EDGE)
		{
			if(cube->voisins[FRONT]  == NULL)      return (way ? cube->voisins[BOTTOM]->voisins[BACK]  : cube->voisins[TOP]->voisins[BACK]);
			else if(cube->voisins[BACK]   == NULL) return (way ? cube->voisins[TOP]->voisins[FRONT]    : cube->voisins[BOTTOM]->voisins[FRONT]);
			else if(cube->voisins[TOP]    == NULL) return (way ? cube->voisins[FRONT]->voisins[BOTTOM] : cube->voisins[BACK]->voisins[BOTTOM]);
			else if(cube->voisins[BOTTOM] == NULL) return (way ? cube->voisins[BACK]->voisins[TOP]     : cube->voisins[FRONT]->voisins[TOP]);
		}
		else if(cube->type==CORNER)
		{
			if(cube->voisins[FRONT] == NULL && cube->voisins[TOP] == NULL )  
				return (way ? cube->voisins[BOTTOM]->voisins[BOTTOM]  : cube->voisins[BACK]->voisins[BACK]);
			else if(cube->voisins[BOTTOM] == NULL && cube->voisins[FRONT] == NULL )  
				return (way ? cube->voisins[BACK]->voisins[BACK]  : cube->voisins[TOP]->voisins[TOP]);
			else if(cube->voisins[BACK] == NULL && cube->voisins[BOTTOM] == NULL )  
				return (way ? cube->voisins[TOP]->voisins[TOP]  : cube->voisins[FRONT]->voisins[FRONT]);
			else if(cube->voisins[TOP] == NULL && cube->voisins[BACK] == NULL )  
				return (way ? cube->voisins[FRONT]->voisins[FRONT]  : cube->voisins[BOTTOM]->voisins[BOTTOM]);
		}
	}
	if(face == RIGHT)
	{
		if(cube->type==EDGE)
		{
			if(cube->voisins[FRONT]  == NULL)      return (!way ? cube->voisins[BOTTOM]->voisins[BACK]  : cube->voisins[TOP]->voisins[BACK]);
			else if(cube->voisins[BACK]   == NULL) return (!way ? cube->voisins[TOP]->voisins[FRONT]    : cube->voisins[BOTTOM]->voisins[FRONT]);
			else if(cube->voisins[TOP]    == NULL) return (!way ? cube->voisins[FRONT]->voisins[BOTTOM] : cube->voisins[BACK]->voisins[BOTTOM]);
			else if(cube->voisins[BOTTOM] == NULL) return (!way ? cube->voisins[BACK]->voisins[TOP]     : cube->voisins[FRONT]->voisins[TOP]);
		}
		else if(cube->type==CORNER)
		{
			if(cube->voisins[FRONT] == NULL && cube->voisins[TOP] == NULL )  
				return (!way ? cube->voisins[BOTTOM]->voisins[BOTTOM]  : cube->voisins[BACK]->voisins[BACK]);
			else if(cube->voisins[BOTTOM] == NULL && cube->voisins[FRONT] == NULL )  
				return (!way ? cube->voisins[BACK]->voisins[BACK]  : cube->voisins[TOP]->voisins[TOP]);
			else if(cube->voisins[BACK] == NULL && cube->voisins[BOTTOM] == NULL )  
				return (!way ? cube->voisins[TOP]->voisins[TOP]  : cube->voisins[FRONT]->voisins[FRONT]);
			else if(cube->voisins[TOP] == NULL && cube->voisins[BACK] == NULL )  
				return (!way ? cube->voisins[FRONT]->voisins[FRONT]  : cube->voisins[BOTTOM]->voisins[BOTTOM]);
		}
	}

}






void Resolver::resolve()
{
	if(current_step > NB_STEPS) // si on a terminé la résolution on change de mode
	{
		state = SHOW;
		return;
	}

	if(rubik_test->moves.size() > 0)
		rubik_test->doMovesInstant();

	switch(current_step)
	{
		case 0 : resolveStep0(rubik_test); break;
		case 1 : resolveStep1(rubik_test); break;
		case 2 : resolveStep2(rubik_test); break;
		case 3 : resolveStep3(rubik_test); break;
		case 4 : resolveStep4(rubik_test); break;
		case 5 : resolveStep5(rubik_test); break;
		case 6 : resolveStep6(rubik_test); break;
		
		case NB_STEPS : 
			state = SHOW;
			for(int i=0; i<rubik_test->moves_save.size(); ++i)
				rubik_view->moves.push_back(rubik_test->moves_save[i]);
			if(rubik_view->moves.size() > 0)
			{
				rubik_view->movesSynthese();
				show_player = true;
			}
			break;
	}
}






void Resolver::resolveStep0(Rubik3x3* rubik)
{
	
	if(whiteCross(rubik))
	{
		//cout << "Step 0 fini" << endl;
		setNextStep();
		return;
	}

	
	Cube* edge = NULL;


	/* ARÊTE BLANCHE SUR LA FACE OPPOSÉ À CELLE DE CENTRE BLANC */
	edge = getCube(rubik->moves_set[BACK], WHITE, EDGE);

	if(edge!=NULL)
	{
		Color* edge_colors = edge->getColors();
		Color edge_color;

		if(edge_colors[0] == WHITE)
			edge_color = edge_colors[1];
		else
			edge_color = edge_colors[0];

		Cube* center = edge->voisins[FRONT];
		Color center_color = center->getColors()[0];

		Cube* new_edge = edge;

		if(!(center_color == edge_color))
		{
			if(getNextPosition(rubik,edge,BACK,false)->voisins[FRONT]->getColors()[0] == edge_color)
			{
				new_edge = getNextPosition(rubik,edge,BACK,false);
				rubik->addMove(BACK,false);
			}
			else if(getNextPosition(rubik,edge,BACK)->voisins[FRONT]->getColors()[0] == edge_color)
			{
				new_edge = getNextPosition(rubik,edge,BACK);
				rubik->addMove(BACK);
			}
			else
			{
				new_edge = getNextPosition(rubik,edge,BACK,true,1);
				rubik->addMove(BACK);
				rubik->addMove(BACK);
			}
		}

		FacesCube face = new_edge->voisins[FRONT]->getFace(edge_color);
		
		rubik->addMove(face);
		rubik->addMove(face);

		if(!(edge->colors[BACK] == WHITE))
		{
			rubik->addMove(face,false);
			rubik->addMove(A);
			switch(face)
			{
				case TOP    : rubik->addMove(DI); break;
				case RIGHT  : rubik->addMove(BI); break;
				case BOTTOM : rubik->addMove(GI); break;
				case LEFT   : rubik->addMove(HI); break;
			}
			rubik->addMove(AI);
		}

		return;
	}




	/* ARÊTE BLANCHE SUR L'ANNEAU CENTRAL */
	edge = getCube(rubik->moves_set[MID_Z], WHITE, EDGE);

	if(edge != NULL)
	{
		Color* edge_colors = edge->getColors();
		Color edge_color;

		if(edge_colors[0] == WHITE)
			edge_color = edge_colors[1];
		else
			edge_color = edge_colors[0];

		FacesCube face_edge_white = edge->getFace(WHITE);
		FacesCube face_edge       = edge->getFace(edge_color);

		Cube* test = NULL;
		Cube* test2 = NULL;

		test = getCube(rubik->moves_set[MID_Z], edge_color.getRubikColor(), CENTER);

		test = test->voisins[FRONT];

		switch(face_edge_white)
		{
			case TOP    : test2 = edge->voisins[BOTTOM]; break;
			case LEFT   : test2 = edge->voisins[RIGHT];  break;
			case BOTTOM : test2 = edge->voisins[TOP];    break;
			case RIGHT  : test2 = edge->voisins[LEFT];   break;
		}

		vector<RubikMoves> pile;

		while(test->voisins[BACK] != test2)
		{	
			test = getNextPosition(rubik, test, FRONT);
			rubik->addMove(A);
			pile.push_back(AI);
		}

		if(getNextPosition(rubik,edge,face_edge)->voisins[FRONT] == NULL)
			rubik->addMove(face_edge);
		else
			rubik->addMove(face_edge,false);

		while(!pile.empty())
		{
			rubik->addMove(pile.back());
			pile.pop_back();
		}

		return;
	}




	/* ARETE BLANCHE SUR LA BONNE FACE MAIS MAL ORIENTÉE */
	vector<Cube*> edges;

	if(!(rubik->cubes[0][2][1]->colors[TOP]==BLUE) && rubik->cubes[0][2][1]->getFace(WHITE)!=UNKNOW_FACE)
		edges.push_back(rubik->cubes[0][2][1]);

	if(!(rubik->cubes[0][1][2]->colors[RIGHT]==RED) && rubik->cubes[0][1][2]->getFace(WHITE)!=UNKNOW_FACE)
		edges.push_back(rubik->cubes[0][1][2]);

	if(!(rubik->cubes[0][0][1]->colors[BOTTOM]==GREEN) && rubik->cubes[0][0][1]->getFace(WHITE)!=UNKNOW_FACE)
		edges.push_back(rubik->cubes[0][0][1]);

	if(!(rubik->cubes[0][1][0]->colors[LEFT]==ORANGE) && rubik->cubes[0][1][0]->getFace(WHITE)!=UNKNOW_FACE)
		edges.push_back(rubik->cubes[0][1][0]);


	while(!edges.empty())
	{
		Cube* edge = edges.back();
		edges.pop_back();

		Color centre_color =  edge->voisins[BACK]->getColors()[0];
		
		Color* tmp = edge->getColors();
		Color edge_color = (tmp[0]==WHITE ? tmp[1] : tmp[0]);

		if(edge->colors[FRONT] == WHITE)
		{
			FacesCube face = edge->getFace(edge_color);

			rubik->addMove(face);
			rubik->addMove(face);
		}
		else
		{
			FacesCube face = edge->getFace(WHITE);

			if(!(edge_color==centre_color))
			{
				rubik->addMove(face);
				rubik->addMove(face);
			}
			else
			{
				FacesCube face = edge->getFace(WHITE);
				rubik->addMove(face,false);
				rubik->addMove(A);
				switch(face)
				{
					case TOP    : rubik->addMove(DI); break;
					case RIGHT  : rubik->addMove(BI); break;
					case BOTTOM : rubik->addMove(GI); break;
					case LEFT   : rubik->addMove(HI); break;
				}
				rubik->addMove(AI);

				return;
			}
		}
	}
}





void Resolver::resolveStep1(Rubik3x3* rubik)
{
	if(whiteCorners(rubik))
	{
		//cout << "Step 1 fini" << endl;
		setNextStep();
		return;
	}

	Cube* corner = NULL;


	/* LE COIN EST À L'ARRIERE */
	corner = getCube(rubik->moves_set[BACK], WHITE, CORNER);

	if(corner != NULL)
	{
		Color* corner_colors = corner->getColors();

		Cube* tests[2];

		bool ok = false;

		Cube corner_next = *corner;

		while(!ok)
		{
			int cpt = 0;

			for(int i=TOP; i<=RIGHT && cpt<2; ++i)
			{
				if(corner->voisins[i] != NULL)
				{
					tests[cpt] = corner->voisins[FRONT]->voisins[i];
					cpt++;
				}
			}

			if(!(tests[0]->getColors()[0].isIn(corner_colors,3)) || !(tests[1]->getColors()[0].isIn(corner_colors,3)))
			{
				rubik->addMove(BACK);
				corner = getNextPosition(rubik,corner,BACK);
				corner_next.rotate(BACK);
			}
			else
				ok = true;
		}
		
		FacesCube face;

		if(corner->voisins[TOP]==NULL && corner->voisins[LEFT]==NULL)
			face = LEFT;
		else if(corner->voisins[LEFT]==NULL && corner->voisins[BOTTOM]==NULL)
			face = BOTTOM;
		else if(corner->voisins[BOTTOM]==NULL && corner->voisins[RIGHT]==NULL)
			face = RIGHT;
		else if(corner->voisins[RIGHT]==NULL && corner->voisins[TOP]==NULL)
			face = TOP;

		int nb_tours = 0;
		bool way = true;

		if(corner_next.colors[face] == WHITE) nb_tours = 1; 
		else if(corner_next.colors[BACK] == WHITE) nb_tours = 3;
		else way = false;

		if(way)
		{
			for(int i=0; i<nb_tours; ++i)
			{
				rubik->addMove(face,false);
				rubik->addMove(PI);
				rubik->addMove(face);
				rubik->addMove(P);	
			}
			rubik->moves.pop_back(); // On enlève le dernier mouvement qui est inutile.
		}
		else
		{
			rubik->addMove(PI);
			rubik->addMove(face,false);
			rubik->addMove(P);
			rubik->addMove(face);	
		}

		return;
	}



	/* LE COIN EST À L'AVANT */
	vector<Cube*> corners = getCubes(rubik->moves_set[FRONT], WHITE, CORNER);

	while(!corners.empty())
	{
		bool ok = true;

		corner = corners.back();
		corners.pop_back();

		if(!(corner->colors[FRONT] == WHITE))
			ok = false;

		for(int i=TOP; i<=RIGHT && ok; ++i)
		{
			if(corner->voisins[i] != NULL)
			{
				Color tmp_color = corner->voisins[BACK]->voisins[i]->getColors()[0];
				FacesCube tmp_face = corner->voisins[BACK]->voisins[i]->getFace(tmp_color);

				if(!(corner->colors[tmp_face]==tmp_color))
					ok = false;
			}
		}

		if(!ok)
		{
			FacesCube face;

			if(corner->voisins[TOP]==NULL && corner->voisins[LEFT]==NULL)
				face = LEFT;
			else if(corner->voisins[LEFT]==NULL && corner->voisins[BOTTOM]==NULL)
				face = BOTTOM;
			else if(corner->voisins[BOTTOM]==NULL && corner->voisins[RIGHT]==NULL)
				face = RIGHT;
			else if(corner->voisins[RIGHT]==NULL && corner->voisins[TOP]==NULL)
				face = TOP;

			rubik->addMove(face,false);
			rubik->addMove(PI);
			rubik->addMove(face);

			return;
		}
	}

}




void Resolver::resolveStep2(Rubik3x3* rubik)
{
	if(middleRing(rubik))
	{
		//cout << "Step 2 fini" << endl;
		setNextStep();
		return;
	}

	
	Cube* edge = NULL;
	Color* edge_colors = NULL;

	for(unsigned int i=1; i<9; i+=2)
	{
		edge = rubik->moves_set[BACK][i];
		edge_colors = edge->getColors();

		if( edge_colors[0] == YELLOW || edge_colors[1] == YELLOW )
			edge = NULL;
		else
			break;
	}

	// SI UNE ARÊTE NE POSSÈDE PAS DE FACE JAUNE
	if(edge != NULL)
	{
		Cube* edge_save = edge;
		Cube edge_virtual = *edge;

		FacesCube edge_face;

		for(int i=TOP; i<=RIGHT; ++i)
			if(edge->voisins[i] == NULL)
			{
				edge_face = FacesCube(i);
				break;
			}

		Color edge_color = edge->colors[edge_face];

		while( !(edge_save->colors[edge_face] == edge->voisins[FRONT]->getColors()[0]) )
		{
			rubik->addMove(P);
			edge = getNextPosition(rubik,edge,BACK);
			edge_virtual.rotate(P);
		}

		edge_face = edge_virtual.getFace(edge_color);

		FacesCube next_face;

		if(edge_save->colors[BACK] == getNextPosition(rubik,edge,BACK,true)->voisins[FRONT]->getColors()[0])
		{
			edge_virtual.rotate(BACK);
			next_face = edge_virtual.getFace(edge_color);

			rubik->addMove(PI);
			rubik->addMove(next_face,false);
			rubik->addMove(P);
			rubik->addMove(next_face);
			rubik->addMove(P);
			rubik->addMove(edge_face);
			rubik->addMove(PI);
			rubik->addMove(edge_face,false);
		}
		else
		{
			edge_virtual.rotate(BACK,false);
			next_face = edge_virtual.getFace(edge_color);

			rubik->addMove(P);
			rubik->addMove(next_face);
			rubik->addMove(PI);
			rubik->addMove(next_face,false);
			rubik->addMove(PI);
			rubik->addMove(edge_face,false);
			rubik->addMove(P);
			rubik->addMove(edge_face);
		}
	}

	// SI UNE ARÊTE POSSÈDE UNE FACE JAUNE
	else
	{
		edge = rubik->moves_set[BACK][1];

		Cube edge_virtual = *edge;
		FacesCube edge_face;
		Color edge_color;

		bool ok = false;
		bool way = true;

		while(!ok)
		{
			for(int i=TOP; i<=RIGHT; ++i)
				if(edge->voisins[i] == NULL)
				{
					edge_face = FacesCube(i);
					edge_color = edge_virtual.colors[edge_face];
					break;
				}

			Cube* center = edge->voisins[FRONT];
			Color center_color = center->getColors()[0];
			Cube* center_voisins[2];

			if(edge->voisins[TOP]==NULL)
			{
				center_voisins[0] = center->voisins[LEFT];
				center_voisins[1] = center->voisins[RIGHT];
			}
			else if(edge->voisins[BOTTOM]==NULL)
			{
				center_voisins[0] = center->voisins[RIGHT];
				center_voisins[1] = center->voisins[LEFT];
			}
			else if(edge->voisins[LEFT]==NULL)
			{
				center_voisins[0] = center->voisins[BOTTOM];
				center_voisins[1] = center->voisins[TOP];
			}
			else if(edge->voisins[RIGHT]==NULL)
			{
				center_voisins[0] = center->voisins[TOP];
				center_voisins[1] = center->voisins[BOTTOM];
			}

			if( !(center_voisins[0]->colors[edge_face]==center->colors[edge_face]) )
			{
				ok = true;
				way = false;
			}
			else if( !(center_voisins[0]->colors[edge_face]==center->colors[edge_face]) )
				ok = true;
			else
			{
				edge = getNextPosition(rubik,edge,BACK);
				edge_virtual.rotate(BACK);
				edge_face = edge_virtual.getFace(edge_color);
			}
		}

		// ON CHANGE L'ARÊTE DE PLACE
		if(!way)
		{
			edge_virtual.rotate(BACK);
			FacesCube next_face = edge_virtual.getFace(edge_color);

			rubik->addMove(PI);
			rubik->addMove(next_face,false);
			rubik->addMove(P);
			rubik->addMove(next_face);
			rubik->addMove(P);
			rubik->addMove(edge_face);
			rubik->addMove(PI);
			rubik->addMove(edge_face,false);
		}
		else
		{
			edge_virtual.rotate(BACK,false);
			FacesCube next_face = edge_virtual.getFace(edge_color);

			rubik->addMove(P);
			rubik->addMove(next_face);
			rubik->addMove(PI);
			rubik->addMove(next_face,false);
			rubik->addMove(PI);
			rubik->addMove(edge_face,false);
			rubik->addMove(P);
			rubik->addMove(edge_face);
		}

	}

}





void Resolver::resolveStep3(Rubik3x3* rubik)
{
	if(yellowCross(rubik))
	{
		//cout << "Step 3 fini" << endl;
		setNextStep();
		return;
	}

	FacesCube face;
	FacesCube next_face;

	/* COUDE JAUNE BIEN PLACÉ */
	if(yellowBend(rubik,&face,&next_face))
	{
		rubik->addMove(face);
		rubik->addMove(P);
		rubik->addMove(next_face);
		rubik->addMove(PI);
		rubik->addMove(next_face,false);
		rubik->addMove(face,false);
	}

	/* LIGNE JAUNE BIEN PLACÉE */
	else if(yellowLine(rubik,&face,&next_face))
	{
		rubik->addMove(face);
		rubik->addMove(next_face);
		rubik->addMove(P);
		rubik->addMove(next_face,false);
		rubik->addMove(PI);
		rubik->addMove(face,false);
	}

	/* SINON */
	else
	{
		rubik->addMove(H);
		rubik->addMove(P);
		rubik->addMove(D);
		rubik->addMove(PI);
		rubik->addMove(DI);
		rubik->addMove(HI);
	}
}





void Resolver::resolveStep4(Rubik3x3* rubik)
{
	if(yellowCorners(rubik))
	{
		//cout << "Step 4 fini" << endl;
		setNextStep();
		return;
	}

	FacesCube face;

	int nb_corners = 0;

	for(int i=0; i<9; i+=2)
	{
		if(i==4)
			continue;

		if(rubik->moves_set[BACK][i]->colors[BACK] == YELLOW)
			nb_corners++;
	}

	/* AUCUN COIN JAUNE PLACÉ */
	if(nb_corners == 0)
	{
		if(rubik->moves_set[BACK][0]->colors[TOP] == YELLOW)
			face = BOTTOM;
		else if(rubik->moves_set[BACK][2]->colors[LEFT] == YELLOW)
			face = RIGHT;
		else if(rubik->moves_set[BACK][6]->colors[RIGHT] == YELLOW)
			face = LEFT;
		else if(rubik->moves_set[BACK][8]->colors[BOTTOM] == YELLOW)
			face = TOP;
	}

	/* UN COIN JAUNE PLACÉ */
	if(nb_corners == 1)
	{
		if(rubik->moves_set[BACK][0]->colors[BACK] == YELLOW)
			face = BOTTOM;
		else if(rubik->moves_set[BACK][2]->colors[BACK] == YELLOW)
			face = RIGHT;
		else if(rubik->moves_set[BACK][6]->colors[BACK] == YELLOW)
			face = LEFT;
		else if(rubik->moves_set[BACK][8]->colors[BACK] == YELLOW)
			face = TOP;
	}

	/* DEUX COINS BIEN PLACÉS */
	else if(nb_corners == 2)
	{
		if(rubik->moves_set[BACK][0]->colors[RIGHT] == YELLOW)
			face = BOTTOM;
		else if(rubik->moves_set[BACK][2]->colors[TOP] == YELLOW)
			face = RIGHT;
		else if(rubik->moves_set[BACK][6]->colors[BOTTOM] == YELLOW)
			face = LEFT;
		else if(rubik->moves_set[BACK][8]->colors[LEFT] == YELLOW)
			face = TOP;
	}

	rubik->addMove(face);
	rubik->addMove(P);
	rubik->addMove(face,false);
	rubik->addMove(P);
	rubik->addMove(face);
	rubik->addMove(P);
	rubik->addMove(P);
	rubik->addMove(face,false);

}






void Resolver::resolveStep5(Rubik3x3* rubik)
{
	if(yellowCornersCorrect(rubik))
	{
		//cout << "Step 5 fini" << endl;
		setNextStep();
		return;
	}

	FacesCube front_face;
	FacesCube right_face;
	FacesCube back_face;

	if(!yellowCornersDouble(rubik,&front_face,&right_face,&back_face))
	{
		rubik->addMove(P);
	}
	else
	{
		rubik->addMove(right_face,false);
		rubik->addMove(front_face);
		rubik->addMove(right_face,false);
		rubik->addMove(back_face);
		rubik->addMove(back_face);
		rubik->addMove(right_face);
		rubik->addMove(front_face,false);
		rubik->addMove(right_face,false);
		rubik->addMove(back_face);
		rubik->addMove(back_face);
		rubik->addMove(right_face);
		rubik->addMove(right_face);
		rubik->addMove(PI);
	}

}






void Resolver::resolveStep6(Rubik3x3* rubik)
{
	if(yellowCrossCorrect(rubik))
	{
		//cout << "Step 6 fini" << endl;
		setNextStep();
		return;
	}

	FacesCube front_face, left_face, right_face;
	bool way = getSensRotationFinal(rubik,&front_face);

	switch(front_face)
	{
		case TOP    : left_face = LEFT;   right_face = RIGHT;  break;
		case LEFT   : left_face = BOTTOM; right_face = TOP;    break;
		case BOTTOM : left_face = RIGHT;  right_face = LEFT;   break;
		case RIGHT  : left_face = TOP;    right_face = BOTTOM; break;
	}

	if(way)
	{
		rubik->addMove(front_face);
		rubik->addMove(front_face);
		rubik->addMove(P);
		rubik->addMove(left_face);
		rubik->addMove(right_face,false);
		rubik->addMove(front_face);
		rubik->addMove(front_face);
		rubik->addMove(left_face,false);
		rubik->addMove(right_face);
		rubik->addMove(P);
		rubik->addMove(front_face);
		rubik->addMove(front_face);
	}
	else
	{
		rubik->addMove(front_face);
		rubik->addMove(front_face);
		rubik->addMove(PI);
		rubik->addMove(left_face);
		rubik->addMove(right_face,false);
		rubik->addMove(front_face);
		rubik->addMove(front_face);
		rubik->addMove(left_face,false);
		rubik->addMove(right_face);
		rubik->addMove(PI);
		rubik->addMove(front_face);
		rubik->addMove(front_face);
	}

}




void Resolver::setNextStep()
{
	current_step++;
}




bool Resolver::whiteCross(Rubik3x3* rubik)
{
	if(rubik->moves_set[FRONT][1]->colors[FRONT]==WHITE && rubik->moves_set[FRONT][1]->colors[TOP]==BLUE)
		if(rubik->moves_set[FRONT][3]->colors[FRONT]==WHITE && rubik->moves_set[FRONT][3]->colors[LEFT]==ORANGE)
			if(rubik->moves_set[FRONT][5]->colors[FRONT]==WHITE && rubik->moves_set[FRONT][5]->colors[RIGHT]==RED)
				if(rubik->moves_set[FRONT][7]->colors[FRONT]==WHITE && rubik->moves_set[FRONT][7]->colors[BOTTOM]==GREEN)
					return true;
	return false;
}




bool Resolver::whiteCorners(Rubik3x3* rubik)
{
	Cube* corner;

	corner = rubik->moves_set[FRONT][0];
	if( !(corner->colors[FRONT]==WHITE) || !(corner->colors[TOP]==BLUE) || !(corner->colors[LEFT]==ORANGE) )
		return false;

	corner = rubik->moves_set[FRONT][2];
	if( !(corner->colors[FRONT]==WHITE) || !(corner->colors[TOP]==BLUE) || !(corner->colors[RIGHT]==RED) )
		return false;

	corner = rubik->moves_set[FRONT][6];
	if( !(corner->colors[FRONT]==WHITE) || !(corner->colors[BOTTOM]==GREEN) || !(corner->colors[LEFT]==ORANGE) )
		return false;

	corner = rubik->moves_set[FRONT][8];
	if( !(corner->colors[FRONT]==WHITE) || !(corner->colors[BOTTOM]==GREEN) || !(corner->colors[RIGHT]==RED) )
		return false;

	return true;
}



bool Resolver::middleRing(Rubik3x3* rubik)
{
	Cube* corner;

	corner = rubik->moves_set[TOP][3];
	if( !(corner->colors[TOP]==BLUE) || !(corner->colors[LEFT]==ORANGE) )
		return false;

	corner = rubik->moves_set[TOP][5];
	if( !(corner->colors[TOP]==BLUE) || !(corner->colors[RIGHT]==RED) )
		return false;

	corner = rubik->moves_set[BOTTOM][3];
	if( !(corner->colors[BOTTOM]==GREEN) || !(corner->colors[LEFT]==ORANGE) )
		return false;

	corner = rubik->moves_set[BOTTOM][5];
	if( !(corner->colors[BOTTOM]==GREEN) || !(corner->colors[RIGHT]==RED) )
		return false;


	return true;
}



bool Resolver::yellowCross(Rubik3x3* rubik)
{
	if(rubik->moves_set[BACK][1]->colors[BACK]==YELLOW)
		if(rubik->moves_set[BACK][3]->colors[BACK]==YELLOW)
			if(rubik->moves_set[BACK][5]->colors[BACK]==YELLOW)
				if(rubik->moves_set[BACK][7]->colors[BACK]==YELLOW)
					return true;
	return false;
}




bool Resolver::yellowCorners(Rubik3x3* rubik)
{
	if(rubik->moves_set[BACK][0]->colors[BACK]==YELLOW)
		if(rubik->moves_set[BACK][2]->colors[BACK]==YELLOW)
			if(rubik->moves_set[BACK][6]->colors[BACK]==YELLOW)
				if(rubik->moves_set[BACK][8]->colors[BACK]==YELLOW)
					return true;
	return false;
}





bool Resolver::yellowCornersCorrect(Rubik3x3* rubik)
{
	if(rubik->moves_set[BACK][0]->colors[TOP]==BLUE && rubik->moves_set[BACK][0]->colors[RIGHT]==RED)
		if(rubik->moves_set[BACK][2]->colors[TOP]==BLUE && rubik->moves_set[BACK][2]->colors[LEFT]==ORANGE)
			if(rubik->moves_set[BACK][6]->colors[BOTTOM]==GREEN && rubik->moves_set[BACK][6]->colors[RIGHT]==RED)
				if(rubik->moves_set[BACK][8]->colors[BOTTOM]==GREEN && rubik->moves_set[BACK][8]->colors[LEFT]==ORANGE)
					return true;
	return false;
}




bool Resolver::yellowCrossCorrect(Rubik3x3* rubik)
{
	if(rubik->moves_set[BACK][1]->colors[TOP] == BLUE)
		if(rubik->moves_set[BACK][3]->colors[RIGHT] == RED)
			if(rubik->moves_set[BACK][5]->colors[LEFT] == ORANGE)
				if(rubik->moves_set[BACK][7]->colors[BOTTOM] == GREEN)
					return true;

	return false;
}





bool Resolver::yellowBend(Rubik3x3* rubik, FacesCube* face, FacesCube* next_face)
{
	if(rubik->moves_set[BACK][1]->colors[BACK]==YELLOW && rubik->moves_set[BACK][3]->colors[BACK]==YELLOW)
	{
		*face = BOTTOM;
		*next_face = LEFT;
		return true;
	}
	else if(rubik->moves_set[BACK][1]->colors[BACK]==YELLOW && rubik->moves_set[BACK][5]->colors[BACK]==YELLOW)
	{
		*face = RIGHT;
		*next_face = BOTTOM;
		return true;
	}
	else if(rubik->moves_set[BACK][7]->colors[BACK]==YELLOW && rubik->moves_set[BACK][5]->colors[BACK]==YELLOW)
	{
		*face = TOP;
		*next_face = RIGHT;
		return true;
	}
	else if(rubik->moves_set[BACK][7]->colors[BACK]==YELLOW && rubik->moves_set[BACK][3]->colors[BACK]==YELLOW)
	{
		*face = LEFT;
		*next_face = TOP;
		return true;
	}

	return false;
}




bool Resolver::yellowLine(Rubik3x3* rubik, FacesCube* face, FacesCube* next_face)
{
	if(rubik->moves_set[BACK][1]->colors[BACK]==YELLOW && rubik->moves_set[BACK][7]->colors[BACK]==YELLOW)
	{
		*face = LEFT;
		*next_face = TOP;
		return true;
	}
	else if(rubik->moves_set[BACK][3]->colors[BACK]==YELLOW && rubik->moves_set[BACK][5]->colors[BACK]==YELLOW)
	{
		*face = TOP;
		*next_face = RIGHT;
		return true;
	}

	return false;
}




bool Resolver::yellowCornersDouble(Rubik3x3* rubik, FacesCube* front_face, FacesCube* right_face, FacesCube* back_face)
{
	Cube* corner0 = rubik->moves_set[BACK][0];
	Cube* corner2 = rubik->moves_set[BACK][2];
	Cube* corner6 = rubik->moves_set[BACK][6];
	Cube* corner8 = rubik->moves_set[BACK][8];

	bool ok0 = false;
	bool ok2 = false;
	bool ok6 = false;
	bool ok8 = false;

	if(corner0->colors[TOP]==BLUE && corner0->colors[RIGHT]==RED)
		ok0 = true;

	if(corner2->colors[TOP]==BLUE && corner2->colors[LEFT]==ORANGE)
		ok2 = true;

	if(corner6->colors[BOTTOM]==GREEN && corner6->colors[RIGHT]==RED)
		ok6 = true;

	if(corner8->colors[BOTTOM]==GREEN && corner8->colors[LEFT]==ORANGE)
		ok8 = true;


	/* COINS CONSÉCUTIFS */	
	if(ok0 && ok2)
	{
		*front_face = BOTTOM;
		*right_face = LEFT;
		*back_face = TOP;
		return true;
	}
	
	if(ok0 && ok6)
	{
		*front_face = LEFT;
		*right_face = TOP;
		*back_face = RIGHT;
		return true;
	}

	if(ok8 && ok2)
	{
		*front_face = RIGHT;
		*right_face = BOTTOM;
		*back_face = LEFT;
		return true;
	}

	if(ok8 && ok6)
	{
		*front_face = TOP;
		*right_face = RIGHT;
		*back_face = BOTTOM;
		return true;
	}


	/* COINS OPPOSÉS */
	if(ok0 && ok8)
	{
		*front_face = TOP;
		*right_face = RIGHT;
		*back_face = BOTTOM;
		return true;
	}

	if(ok2 && ok6)
	{
		*front_face = RIGHT;
		*right_face = BOTTOM;
		*back_face = LEFT;
		return true;
	}

	return false;
}





bool Resolver::getSensRotationFinal(Rubik3x3* rubik, FacesCube* front_face)
{
	Color tmp1, tmp2;

	if(rubik->moves_set[BACK][1]->colors[TOP] == BLUE)
	{
		*front_face = BOTTOM;
		tmp1 = rubik->moves_set[BACK][7]->colors[BOTTOM];
		tmp2 = rubik->moves_set[RIGHT][4]->colors[RIGHT];
		return (tmp1==tmp2);
	}

	if(rubik->moves_set[BACK][3]->colors[RIGHT] == RED)
	{
		*front_face = LEFT;
		tmp1 = rubik->moves_set[BACK][5]->colors[LEFT];
		tmp2 = rubik->moves_set[BOTTOM][4]->colors[BOTTOM];
		return (tmp1==tmp2);
	}

	if(rubik->moves_set[BACK][5]->colors[LEFT] == ORANGE)
	{
		*front_face = RIGHT;
		tmp1 = rubik->moves_set[BACK][3]->colors[RIGHT];
		tmp2 = rubik->moves_set[TOP][4]->colors[TOP];
		return (tmp1==tmp2);
	}

	else
	{
		*front_face = TOP;
		tmp1 = rubik->moves_set[BACK][1]->colors[TOP];
		tmp2 = rubik->moves_set[LEFT][4]->colors[LEFT];
		return (tmp1==tmp2);
	}

}