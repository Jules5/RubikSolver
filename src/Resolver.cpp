#include <Resolver.h>



const unsigned int Resolver::NB_STEPS = 7;



Resolver::Resolver()
{
	is_test = false;
	compteur = 0;

	state = SHOW;

	move = false;
	rotate_x = rotate_y = -20;

	SDL_Init(SDL_INIT_VIDEO);
    SDL_WM_SetCaption("Rubik Solver",NULL);
    SDL_SetVideoMode(600, 600, 32, SDL_OPENGL);

    glClearColor(0.1,0.1,0.1,1.);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_LINE_SMOOTH);

	id_logo = loadTexture("./res/rubik.png");

	rubik = new Rubik3x3(1,id_logo,"saves/test.rbk");
	//rubik = new Rubik3x3(1,id_logo);

	current_step = 0;
	save.clear();
}



Resolver::~Resolver()
{

}



void Resolver::run()
{
	bool continuer = true;
    SDL_Event event;

    int tx = 0;
    int ty = 0;
    int dt = 0;

    while (continuer)
    {
    	ty = SDL_GetTicks();
    	dt = ty - tx;
    	tx = ty;

	    while(SDL_PollEvent(&event))
	    {
	        switch(event.type)
	        {
	            case SDL_QUIT :
	                continuer = false;
	                break;

	            case SDL_MOUSEBUTTONDOWN :
	            	move = true;
	            	break;

	            case SDL_MOUSEBUTTONUP :
	            	move = false;
	            	break;

	            case SDL_MOUSEMOTION :
	            	if(move)
	            	{
		            	rotate_x -= event.motion.yrel;
		            	rotate_y -= event.motion.xrel;
		            }
	            	break;

	            case SDL_KEYUP :
	            	switch (event.key.keysym.sym)
	       			{
	       				case SDLK_ESCAPE : 
	       					exit(0);
	       					break;

	       				case SDLK_SPACE :
	       					if(state==SHOW) {state=RESOLVE; current_step=0;}
	       					else state=SHOW;
	       					break;

	       				// case SDLK_KP5 :
	       				// 	rubik->save("test");
	       				// 	break;

	       				case SDLK_r : if(state==SHOW) rubik->shuffle();

	       			}
	            	break;

	        }
	    }

    	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    	if(state == RESOLVE)
    		resolve();

    	display(dt);

        glFlush();
        SDL_GL_SwapBuffers();
    }

    SDL_Quit();
}







void Resolver::display(int dt)
{
	glPushMatrix();

	glRotatef(rotate_x,1,0,0);
	glRotatef(rotate_y,0,1,0);

	rubik->display(dt);

	glPopMatrix();
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







Cube* Resolver::getNextPosition(Cube* cube, FacesCube face, bool way, unsigned int nb_rot)
{
	bool is_in_face = false;

	for(int i=0; i<9; ++i)
		if(rubik->moves_set[face][i] == cube)
			is_in_face = true;

	if(!is_in_face || cube->type==CENTER)
		return cube;


	if(nb_rot > 0)
		cube = getNextPosition(cube,face,way,nb_rot-1);


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
	if(!rubik->moves.empty()) // si le rubik est en mouvement on ne fait rien
		return;

	if(current_step > NB_STEPS) // si on a terminé la résolution on change de mode
	{
		state = SHOW;
		return;
	}

	switch(current_step)
	{
		case 0 : resolveStep0(); break;
		case 1 : resolveStep1(); break;
		case 2 : resolveStep2(); break;
		case 3 : resolveStep3(); break;
		case 4 : resolveStep4(); break;
		case 5 : resolveStep5(); break;
		case 6 : resolveStep6(); break;
		
		case NB_STEPS : cout << "Rubik's Cube résolu !" << endl; current_step++; 
			if(is_test)
			{
				compteur++;
				if(compteur <= 100)
				{
					cout << "Nombre d'itération : " << compteur << endl;
					rubik->shuffle();
					rubik->shuffle();
					rubik->shuffle();
					state = RESOLVE;
					current_step = 0;
				}
			}
			break;
	}
}






void Resolver::resolveStep0()
{
	
	if(whiteCross())
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
			if(getNextPosition(edge,BACK,false)->voisins[FRONT]->getColors()[0] == edge_color)
			{
				new_edge = getNextPosition(edge,BACK,false);
				rubik->addMove(BACK,false);
			}
			else if(getNextPosition(edge,BACK)->voisins[FRONT]->getColors()[0] == edge_color)
			{
				new_edge = getNextPosition(edge,BACK);
				rubik->addMove(BACK);
			}
			else
			{
				new_edge = getNextPosition(edge,BACK,true,1);
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
			test = getNextPosition(test, FRONT);
			rubik->addMove(A);
			pile.push_back(AI);
		}

		if(getNextPosition(edge,face_edge)->voisins[FRONT] == NULL)
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





void Resolver::resolveStep1()
{
	if(whiteCorners())
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
				corner = getNextPosition(corner,BACK);
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




void Resolver::resolveStep2()
{
	if(middleRing())
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
			edge = getNextPosition(edge,BACK);
			edge_virtual.rotate(P);
		}

		edge_face = edge_virtual.getFace(edge_color);

		FacesCube next_face;

		if(edge_save->colors[BACK] == getNextPosition(edge,BACK,true)->voisins[FRONT]->getColors()[0])
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
				edge = getNextPosition(edge,BACK);
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





void Resolver::resolveStep3()
{
	if(yellowCross())
	{
		//cout << "Step 3 fini" << endl;
		setNextStep();
		return;
	}

	FacesCube face;
	FacesCube next_face;

	/* COUDE JAUNE BIEN PLACÉ */
	if(yellowBend(&face,&next_face))
	{
		rubik->addMove(face);
		rubik->addMove(P);
		rubik->addMove(next_face);
		rubik->addMove(PI);
		rubik->addMove(next_face,false);
		rubik->addMove(face,false);
	}

	/* LIGNE JAUNE BIEN PLACÉE */
	else if(yellowLine(&face,&next_face))
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





void Resolver::resolveStep4()
{
	if(yellowCorners())
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






void Resolver::resolveStep5()
{
	if(yellowCornersCorrect())
	{
		//cout << "Step 5 fini" << endl;
		setNextStep();
		return;
	}

	FacesCube front_face;
	FacesCube right_face;
	FacesCube back_face;

	if(!yellowCornersDouble(&front_face,&right_face,&back_face))
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






void Resolver::resolveStep6()
{
	if(yellowCrossCorrect())
	{
		//cout << "Step 6 fini" << endl;
		setNextStep();
		return;
	}

	FacesCube front_face, left_face, right_face;
	bool way = getSensRotationFinal(&front_face);

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
	save.clear();
	current_step++;
}




bool Resolver::whiteCross()
{
	if(rubik->moves_set[FRONT][1]->colors[FRONT]==WHITE && rubik->moves_set[FRONT][1]->colors[TOP]==BLUE)
		if(rubik->moves_set[FRONT][3]->colors[FRONT]==WHITE && rubik->moves_set[FRONT][3]->colors[LEFT]==ORANGE)
			if(rubik->moves_set[FRONT][5]->colors[FRONT]==WHITE && rubik->moves_set[FRONT][5]->colors[RIGHT]==RED)
				if(rubik->moves_set[FRONT][7]->colors[FRONT]==WHITE && rubik->moves_set[FRONT][7]->colors[BOTTOM]==GREEN)
					return true;
	return false;
}




bool Resolver::whiteCorners()
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



bool Resolver::middleRing()
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



bool Resolver::yellowCross()
{
	if(rubik->moves_set[BACK][1]->colors[BACK]==YELLOW)
		if(rubik->moves_set[BACK][3]->colors[BACK]==YELLOW)
			if(rubik->moves_set[BACK][5]->colors[BACK]==YELLOW)
				if(rubik->moves_set[BACK][7]->colors[BACK]==YELLOW)
					return true;
	return false;
}




bool Resolver::yellowCorners()
{
	if(rubik->moves_set[BACK][0]->colors[BACK]==YELLOW)
		if(rubik->moves_set[BACK][2]->colors[BACK]==YELLOW)
			if(rubik->moves_set[BACK][6]->colors[BACK]==YELLOW)
				if(rubik->moves_set[BACK][8]->colors[BACK]==YELLOW)
					return true;
	return false;
}





bool Resolver::yellowCornersCorrect()
{
	if(rubik->moves_set[BACK][0]->colors[TOP]==BLUE && rubik->moves_set[BACK][0]->colors[RIGHT]==RED)
		if(rubik->moves_set[BACK][2]->colors[TOP]==BLUE && rubik->moves_set[BACK][2]->colors[LEFT]==ORANGE)
			if(rubik->moves_set[BACK][6]->colors[BOTTOM]==GREEN && rubik->moves_set[BACK][6]->colors[RIGHT]==RED)
				if(rubik->moves_set[BACK][8]->colors[BOTTOM]==GREEN && rubik->moves_set[BACK][8]->colors[LEFT]==ORANGE)
					return true;
	return false;
}




bool Resolver::yellowCrossCorrect()
{
	if(rubik->moves_set[BACK][1]->colors[TOP] == BLUE)
		if(rubik->moves_set[BACK][3]->colors[RIGHT] == RED)
			if(rubik->moves_set[BACK][5]->colors[LEFT] == ORANGE)
				if(rubik->moves_set[BACK][7]->colors[BOTTOM] == GREEN)
					return true;

	return false;
}





bool Resolver::yellowBend(FacesCube* face, FacesCube* next_face)
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




bool Resolver::yellowLine(FacesCube* face, FacesCube* next_face)
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




bool Resolver::yellowCornersDouble(FacesCube* front_face, FacesCube* right_face, FacesCube* back_face)
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





bool Resolver::getSensRotationFinal(FacesCube* front_face)
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