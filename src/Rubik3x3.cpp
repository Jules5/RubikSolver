#include "Rubik3x3.h"



Rubik3x3::Rubik3x3(float x, float y, float s, string filename)
:posx(x), posy(y), size(s)
{
	state = WAIT;
	reinit_position = false;
	angle = 0;
	rotate_x = 30;
	rotate_y = 45;
	speed = 5;

	moves.clear();

	for(int i=0; i<NB_FACES; ++i)
		for(int j=0; j<9; ++j)
			moves_set[i][j] = NULL;

	for(int i=0; i<3; ++i)
		for(int j=0; j<3; ++j)
			for(int k=0; k<3; ++k)
				cubes[i][j][k] = NULL;

	initCubes();
	defineGroupes();
	defineVoisins();

	if(filename.empty())
		loadRubikComplete();
	else
		loadRubikFromFile(filename);

	/* Lumière */
	GLfloat position[] = { 0, -1, 1, 0};
	GLfloat LightAmbient[] = { 1.0f, 1.0f, 1.0f, 1.f };
	GLfloat LightDiffuse[]= { 0.3f, 0.3f, 0.3f, 1.f };
	glLightfv(GL_LIGHT0, GL_POSITION, position);
	glLightfv(GL_LIGHT0, GL_AMBIENT, LightAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, LightDiffuse);

	/* Texture */
	texture_square = loadTexture("./res/tex/square.png");
}





Rubik3x3::~Rubik3x3()
{
	moves.clear();
	moves_save.clear();

	for(int i=0; i<3; ++i)
		for(int j=0; j<3; ++j)
			for(int k=0; k<3; ++k)
				delete cubes[i][j][k];
}




void Rubik3x3::save(string filename)
{
	ofstream file(("saves/"+filename+".rbk").c_str(), ios::out | ios::trunc);

	FacesCube liste[6] = {FRONT,RIGHT,TOP,LEFT,BOTTOM,BACK};
        
    if(!file)  // si l'ouverture a réussi
   		cerr << "Échec dans l'ouverture du fichier." << endl;
   	else
   	{
   		for(int i=0; i<6; ++i)
   			for(int j=0; j<9; ++j)
   			{
   				file << moves_set[liste[i]][j]->colors[liste[i]].red;
   				file << moves_set[liste[i]][j]->colors[liste[i]].green;
   				file << moves_set[liste[i]][j]->colors[liste[i]].blue;
   			}

   		file.close();
   	}
}



void Rubik3x3::initCubes()
{
	int d = 0;

	cubes[0][0][0] = new Cube(-size/3-d , -size/3-d , -size/3-d , size/3);
	cubes[0][0][1] = new Cube( 0        , -size/3-d , -size/3-d , size/3);	
	cubes[0][0][2] = new Cube( size/3+d , -size/3-d , -size/3-d , size/3);
	cubes[0][1][0] = new Cube(-size/3-d ,  0        , -size/3-d , size/3);
	cubes[0][1][1] = new Cube( 0        ,  0        , -size/3-d , size/3);
	cubes[0][1][2] = new Cube( size/3+d ,  0        , -size/3-d , size/3);
	cubes[0][2][0] = new Cube(-size/3-d , size/3+d  , -size/3-d , size/3);
	cubes[0][2][1] = new Cube( 0        , size/3+d  , -size/3-d , size/3);
	cubes[0][2][2] = new Cube( size/3+d , size/3+d  , -size/3-d , size/3);
	cubes[1][0][0] = new Cube(-size/3-d , -size/3-d , 0       , size/3);
	cubes[1][0][1] = new Cube( 0        , -size/3-d , 0       , size/3);
	cubes[1][0][2] = new Cube( size/3+d , -size/3-d , 0       , size/3);
	cubes[1][1][0] = new Cube(-size/3-d , 0         , 0       , size/3);
	cubes[1][1][1] = new Cube( 0        , 0         , 0       , size/3);
	cubes[1][1][2] = new Cube( size/3+d , 0         , 0       , size/3);
	cubes[1][2][0] = new Cube(-size/3-d , size/3+d  , 0       , size/3);
	cubes[1][2][1] = new Cube( 0        , size/3+d  , 0       , size/3);
	cubes[1][2][2] = new Cube( size/3+d , size/3+d  , 0       , size/3);
	cubes[2][0][0] = new Cube(-size/3-d , -size/3-d , size/3+d  , size/3);
	cubes[2][0][1] = new Cube( 0        , -size/3-d , size/3+d  , size/3);
	cubes[2][0][2] = new Cube( size/3+d , -size/3-d , size/3+d  , size/3);
	cubes[2][1][0] = new Cube(-size/3-d , 0         , size/3+d  , size/3);
	cubes[2][1][1] = new Cube( 0        , 0         , size/3+d  , size/3);
	cubes[2][1][2] = new Cube( size/3+d , 0         , size/3+d  , size/3);
	cubes[2][2][0] = new Cube(-size/3-d , size/3+d  , size/3+d  , size/3);
	cubes[2][2][1] = new Cube( 0        , size/3+d  , size/3+d  , size/3);
	cubes[2][2][2] = new Cube( size/3+d , size/3+d  , size/3+d  , size/3);
}



void Rubik3x3::defineGroupes()
{
	moves_set[FRONT][0]  = cubes[0][2][0];
 	moves_set[FRONT][1]  = cubes[0][2][1];
	moves_set[FRONT][2]  = cubes[0][2][2];
	moves_set[FRONT][3]  = cubes[0][1][0];
	moves_set[FRONT][4]  = cubes[0][1][1];
	moves_set[FRONT][5]  = cubes[0][1][2];
	moves_set[FRONT][6]  = cubes[0][0][0];
	moves_set[FRONT][7]  = cubes[0][0][1];
	moves_set[FRONT][8]  = cubes[0][0][2];

	moves_set[BACK][0]   = cubes[2][2][2];
	moves_set[BACK][1]   = cubes[2][2][1];
	moves_set[BACK][2]   = cubes[2][2][0];
	moves_set[BACK][3]   = cubes[2][1][2];
	moves_set[BACK][4]   = cubes[2][1][1];
	moves_set[BACK][5]   = cubes[2][1][0];
	moves_set[BACK][6]   = cubes[2][0][2];
	moves_set[BACK][7]   = cubes[2][0][1];
	moves_set[BACK][8]   = cubes[2][0][0];

	moves_set[TOP][0]    = cubes[2][2][0];
	moves_set[TOP][1]    = cubes[2][2][1];
	moves_set[TOP][2]    = cubes[2][2][2];
	moves_set[TOP][3]    = cubes[1][2][0];
	moves_set[TOP][4]    = cubes[1][2][1];
	moves_set[TOP][5]    = cubes[1][2][2];
	moves_set[TOP][6]    = cubes[0][2][0];
	moves_set[TOP][7]    = cubes[0][2][1];
	moves_set[TOP][8]    = cubes[0][2][2];

	moves_set[BOTTOM][0] = cubes[0][0][0];
	moves_set[BOTTOM][1] = cubes[0][0][1];
	moves_set[BOTTOM][2] = cubes[0][0][2];
	moves_set[BOTTOM][3] = cubes[1][0][0];
	moves_set[BOTTOM][4] = cubes[1][0][1];
	moves_set[BOTTOM][5] = cubes[1][0][2];
	moves_set[BOTTOM][6] = cubes[2][0][0];
	moves_set[BOTTOM][7] = cubes[2][0][1];
	moves_set[BOTTOM][8] = cubes[2][0][2];

	moves_set[LEFT][0]   = cubes[2][2][0];
	moves_set[LEFT][1]   = cubes[1][2][0];
	moves_set[LEFT][2]   = cubes[0][2][0];
	moves_set[LEFT][3]   = cubes[2][1][0];
	moves_set[LEFT][4]   = cubes[1][1][0];
	moves_set[LEFT][5]   = cubes[0][1][0];
	moves_set[LEFT][6]   = cubes[2][0][0];
	moves_set[LEFT][7]   = cubes[1][0][0];
	moves_set[LEFT][8]   = cubes[0][0][0];

	moves_set[RIGHT][0]  = cubes[0][2][2];
	moves_set[RIGHT][1]  = cubes[1][2][2];
	moves_set[RIGHT][2]  = cubes[2][2][2];
	moves_set[RIGHT][3]  = cubes[0][1][2];
	moves_set[RIGHT][4]  = cubes[1][1][2];
	moves_set[RIGHT][5]  = cubes[2][1][2];
	moves_set[RIGHT][6]  = cubes[0][0][2];
	moves_set[RIGHT][7]  = cubes[1][0][2];
	moves_set[RIGHT][8]  = cubes[2][0][2];

	moves_set[MID_X][0]  = cubes[0][0][1];
	moves_set[MID_X][1]  = cubes[1][0][1];
	moves_set[MID_X][2]  = cubes[2][0][1];
	moves_set[MID_X][3]  = cubes[0][1][1];
	moves_set[MID_X][4]  = cubes[1][1][1];
	moves_set[MID_X][5]  = cubes[2][1][1];
	moves_set[MID_X][6]  = cubes[0][2][1];
	moves_set[MID_X][7]  = cubes[1][2][1];
	moves_set[MID_X][8]  = cubes[2][2][1];

	moves_set[MID_Y][0]  = cubes[0][1][0];
	moves_set[MID_Y][1]  = cubes[1][1][0];
	moves_set[MID_Y][2]  = cubes[2][1][0];
	moves_set[MID_Y][3]  = cubes[0][1][1];
	moves_set[MID_Y][4]  = cubes[1][1][1];
	moves_set[MID_Y][5]  = cubes[2][1][1];
	moves_set[MID_Y][6]  = cubes[0][1][2];
	moves_set[MID_Y][7]  = cubes[1][1][2];
	moves_set[MID_Y][8]  = cubes[2][1][2];

	moves_set[MID_Z][0]  = cubes[1][0][0];
	moves_set[MID_Z][1]  = cubes[1][1][0];
	moves_set[MID_Z][2]  = cubes[1][2][0];
	moves_set[MID_Z][3]  = cubes[1][0][1];
	moves_set[MID_Z][4]  = cubes[1][1][1];
	moves_set[MID_Z][5]  = cubes[1][2][1];
	moves_set[MID_Z][6]  = cubes[1][0][2];
	moves_set[MID_Z][7]  = cubes[1][1][2];
	moves_set[MID_Z][8]  = cubes[1][2][2];
}






void Rubik3x3::defineVoisins()
{
	for(int z=0; z<3; z++)
		for(int y=0; y<3; y++)
			for(int x=0; x<3; x++)
			{
				if(x > 0)
					cubes[z][y][x]->setVoisin(LEFT   , cubes[z][y][x-1]);
				if(x < 2)
					cubes[z][y][x]->setVoisin(RIGHT  , cubes[z][y][x+1]);
				if(y > 0)
					cubes[z][y][x]->setVoisin(BOTTOM , cubes[z][y-1][x]);
				if(y < 2)
					cubes[z][y][x]->setVoisin(TOP    , cubes[z][y+1][x]);
				if(z > 0)
					cubes[z][y][x]->setVoisin(FRONT  , cubes[z-1][y][x]);
				if(z < 2)
					cubes[z][y][x]->setVoisin(BACK   , cubes[z+1][y][x]);
			}
}



	

void Rubik3x3::loadRubikComplete()
{
	/* AVANT LIGNE BAS */
	cubes[0][0][0]->setColor(FRONT , WHITE);
	cubes[0][0][0]->setColor(BOTTOM, GREEN);
	cubes[0][0][0]->setColor(LEFT  , ORANGE);
	cubes[0][0][1]->setColor(FRONT , WHITE);
	cubes[0][0][1]->setColor(BOTTOM, GREEN);
	cubes[0][0][2]->setColor(FRONT , WHITE);
	cubes[0][0][2]->setColor(BOTTOM, GREEN);
	cubes[0][0][2]->setColor(RIGHT , RED);

	/* AVANT LIGNE MILIEU */
	cubes[0][1][0]->setColor(FRONT, WHITE);
	cubes[0][1][0]->setColor(LEFT , ORANGE);
	cubes[0][1][1]->setColor(FRONT, WHITE);
	cubes[0][1][2]->setColor(FRONT, WHITE);
	cubes[0][1][2]->setColor(RIGHT, RED);

	/* AVANT LIGNE HAUT */
	cubes[0][2][0]->setColor(FRONT, WHITE);
	cubes[0][2][0]->setColor(TOP  , BLUE);
	cubes[0][2][0]->setColor(LEFT , ORANGE);
	cubes[0][2][1]->setColor(FRONT, WHITE);
	cubes[0][2][1]->setColor(TOP  , BLUE);
	cubes[0][2][2]->setColor(FRONT, WHITE);
	cubes[0][2][2]->setColor(TOP  , BLUE);
	cubes[0][2][2]->setColor(RIGHT, RED);

	/* MILIEU LIGNE BAS */
	cubes[1][0][0]->setColor(BOTTOM, GREEN);
	cubes[1][0][0]->setColor(LEFT  , ORANGE);
	cubes[1][0][1]->setColor(BOTTOM, GREEN);
	cubes[1][0][2]->setColor(BOTTOM, GREEN);
	cubes[1][0][2]->setColor(RIGHT , RED);

	/* MILIEU LIGNE MILIEU */
	cubes[1][1][0]->setColor(LEFT, ORANGE);
	cubes[1][1][2]->setColor(RIGHT, RED);

	/* MILIEU LIGNE HAUT */
	cubes[1][2][0]->setColor(TOP , BLUE);
	cubes[1][2][0]->setColor(LEFT, ORANGE);
	cubes[1][2][1]->setColor(TOP, BLUE);
	cubes[1][2][2]->setColor(TOP  , BLUE);
	cubes[1][2][2]->setColor(RIGHT, RED);

	/* ARRIERE LIGNE BAS */
	cubes[2][0][0]->setColor(BACK  , YELLOW);
	cubes[2][0][0]->setColor(BOTTOM, GREEN);
	cubes[2][0][0]->setColor(LEFT  , ORANGE);
	cubes[2][0][1]->setColor(BACK  , YELLOW);
	cubes[2][0][1]->setColor(BOTTOM, GREEN);
	cubes[2][0][2]->setColor(BACK  , YELLOW);
	cubes[2][0][2]->setColor(BOTTOM, GREEN);
	cubes[2][0][2]->setColor(RIGHT , RED);

	/* ARRIERE LIGNE MILIEU */
	cubes[2][1][0]->setColor(BACK, YELLOW);
	cubes[2][1][0]->setColor(LEFT, ORANGE);
	cubes[2][1][1]->setColor(BACK, YELLOW);
	cubes[2][1][2]->setColor(BACK , YELLOW);
	cubes[2][1][2]->setColor(RIGHT, RED);

	/* ARRIERE LIGNE HAUT */
	cubes[2][2][0]->setColor(BACK, YELLOW);
	cubes[2][2][0]->setColor(TOP , BLUE);
	cubes[2][2][0]->setColor(LEFT, ORANGE);
	cubes[2][2][1]->setColor(BACK, YELLOW);
	cubes[2][2][1]->setColor(TOP , BLUE);
	cubes[2][2][2]->setColor(BACK , YELLOW);
	cubes[2][2][2]->setColor(TOP  , BLUE);
	cubes[2][2][2]->setColor(RIGHT, RED);
}










void Rubik3x3::loadRubikFromFile(string filename)
{
	ifstream file(filename.c_str(), ios::in);  // on ouvre en lecture

    if(!file)  // si l'ouverture a fonctionné
    {
        cerr << "Impossible d'ouvrir le fichier !" << endl;
        loadRubikComplete();
        return;
    }


	Color colors[54];

    unsigned char red = 0;
    unsigned char green = 0;
    unsigned char blue = 0;

    for(int i=0; i<54; ++i)
    {
    	file >> red;
    	file >> green;
    	file >> blue;

    	colors[i] = Color(red,green,blue);
    }

	file.close();

	int ind = 0;

	// FACE AVANT 
	for(int i=0; i<9; ++i)
	{
		moves_set[FRONT][i]->setColor(FRONT,colors[ind]);
		ind ++;
	}

	// FACE DROITE 
	for(int i=0; i<9; ++i)
	{
		moves_set[RIGHT][i]->setColor(RIGHT,colors[ind]);
		ind ++;
	}

	// FACE HAUTE 
	for(int i=0; i<9; ++i)
	{
		moves_set[TOP][i]->setColor(TOP,colors[ind]);
		ind ++;
	}

	// FACE GAUCHE 
	for(int i=0; i<9; ++i)
	{
		moves_set[LEFT][i]->setColor(LEFT,colors[ind]);
		ind ++;
	}

	// FACE BASSE 
	for(int i=0; i<9; ++i)
	{
		moves_set[BOTTOM][i]->setColor(BOTTOM,colors[ind]);
		ind ++;
	}

	// FACE ARRIERE 
	for(int i=0; i<9; ++i)
	{
		moves_set[BACK][i]->setColor(BACK,colors[ind]);
		ind ++;
	}
}





void Rubik3x3::loadRubikFromOther(Rubik3x3* r)
{
	for(int i=0; i<3; ++i)
		for(int j=0; j<3; ++j)
			for(int k=0; k<3; ++k)
				cubes[i][j][k]->copyColors(r->cubes[i][j][k]);
}




void Rubik3x3::shuffle()
{
	moves.clear();
	moves_save.clear();

	srand(time(NULL));

	for(int i=0; i<15; ++i)
	{
		int r = rand()%NB_MOVES;

		if(moves.size()>0 && getContraryMove(moves.back()) == RubikMoves(r))
			i--;
		else
			addMove(RubikMoves(r));
	}
}




void Rubik3x3::rotate(float x, float y)
{
	rotate_x += y*0.4;
	rotate_y += x*0.4;
}





void Rubik3x3::addMove(RubikMoves m)
{
	moves.push_back(m);
}



void Rubik3x3::addMove(FacesCube f, bool way)
{
	if(way)
		switch(f)
		{
			case FRONT  : moves.push_back(A); break;
			case BACK   : moves.push_back(P); break;
			case TOP    : moves.push_back(H); break;
			case BOTTOM : moves.push_back(B); break;
			case LEFT   : moves.push_back(G); break;
			case RIGHT  : moves.push_back(D); break;
		}
	else
		switch(f)
		{
			case FRONT  : moves.push_back(AI); break;
			case BACK   : moves.push_back(PI); break;
			case TOP    : moves.push_back(HI); break;
			case BOTTOM : moves.push_back(BI); break;
			case LEFT   : moves.push_back(GI); break;
			case RIGHT  : moves.push_back(DI); break;
		}
}




void Rubik3x3::doMovesInstant()
{
	while(moves.size() > 0)
	{
		updateCubesPositions(moves.front());
		moves_save.push_back(moves.front());
		moves.pop_front();
	}
}



RubikMoves Rubik3x3::getContraryMove(RubikMoves m)
{
	switch(m)
	{
		case A  : return AI; break;
		case AI : return A;  break;
		case P  : return PI; break;
		case PI : return P;  break;
		case H  : return HI; break;
		case HI : return H;  break;
		case B  : return BI; break;
		case BI : return B;  break;
		case G  : return GI; break;
		case GI : return G;  break;
		case D  : return DI; break;
		case DI : return D;  break;
	}
}



void Rubik3x3::movesSynthese()
{
	for(unsigned int i=0; i<moves.size(); ++i)
	{
		if(i+1 < moves.size())
			if(moves[i+1] == getContraryMove(moves[i]))
				moves.erase(moves.begin()+i,moves.begin()+i+2);

		if(i+2 < moves.size())
			if(moves[i]==moves[i+1] && moves[i]==moves[i+2])
			{
				RubikMoves tmp = moves[i];
				RubikMoves to_insert;
				moves.erase(moves.begin()+i,moves.begin()+i+3);
				switch(tmp)
				{
					case A  : to_insert = AI; break;
					case AI : to_insert = A;  break;
					case P  : to_insert = PI; break;
					case PI : to_insert = P;  break;
					case B  : to_insert = BI; break;
					case BI : to_insert = B;  break;
					case H  : to_insert = HI; break;
					case HI : to_insert = H;  break;
					case G  : to_insert = GI; break;
					case GI : to_insert = G;  break;
					case D  : to_insert = DI; break;
					case DI : to_insert = D;  break;
				}
				moves.insert(moves.begin()+i,to_insert);
			}
	}
}




void Rubik3x3::animate(int dt)
{
	/* POUR QUE LES VALEURS DE ROTATION SOIENT TOUJOURS ENTRE 0 ET 360 */
	if(rotate_x<0)        rotate_x = 360.f-rotate_x;
	else if(rotate_x>360) rotate_x = rotate_x-360.f;
	
	if(rotate_y<0)        rotate_y = 360.f-rotate_y;
	else if(rotate_y>360) rotate_y = rotate_y-360.f;

	/* MODE REINITIALISATION DE LA CAMERA */
	if(reinit_position)
	{
		bool way = true;
		if(rotate_x < 210 && rotate_x > 30)
		{
			rotate_x -= dt*0.2;
			way = false;
		} 
		else 
			rotate_x += dt*0.2;

		if((way && rotate_x>=30 && rotate_x<210) || (!way && rotate_x<=30))
			rotate_x = 30;

		way = true;
		if(rotate_y < 225 && rotate_y > 45)
		{
			rotate_y -= dt*0.2;
			way = false;
		} 
		else 
			rotate_y += dt*0.2;

		if((way && rotate_y>=45 && rotate_y<225) || (!way && rotate_y<=45))
			rotate_y = 45;

		if(rotate_x==30 && rotate_y==45)
			reinit_position = false;
	}
	

	/* GESTION MOUVEMENTS */
	if((state == MOVE || state == MOVE_ONE) && moves.size() > 0)
	{	
		angle += dt*speed/10;

		if(angle > 90)
		{
			updateCubesPositions(moves.front());
			RubikMoves tmp = moves.front();
			moves_save.push_back(tmp);
			moves.pop_front();

			angle = 0;

			if(state == MOVE_ONE)
				state = WAIT;
		}

		if(moves.size() <= 0)
			state = WAIT;
	}
	else if((state == MOVE_REV || state == MOVE_REV_ONE) && moves_save.size() > 0)
	{	
		angle -= dt*speed/10;

		if(angle < -90)
		{
			updateCubesPositions(getContraryMove(moves_save.back()));
			RubikMoves tmp = moves_save.back();
			moves.push_front(tmp);
			moves_save.pop_back();

			angle = 0;

			if(state == MOVE_REV_ONE)
				state = WAIT;
		}

		if(moves_save.size() <= 0)
			state = WAIT;
	}
	else
		state = WAIT;
	
}





void Rubik3x3::display()
{
	glPushMatrix();
	
	glTranslatef(posx,posy,0);

	glRotatef(rotate_x,-1,0,0);
	glRotatef(rotate_y,0,-1,0);

	vector<Cube*> in_move;



	/* AFFICHAGE DES CUBES EN ROTATION */
	if(state != WAIT)
	{	
		glPushMatrix();

		RubikMoves next_move = (state==MOVE || state==MOVE_ONE ? moves.front() : moves_save.back());

		switch(next_move)
		{
			case A : case PI : glRotatef(angle,0,0,-1);  break;
			case P : case AI : glRotatef(angle,0,0,1);  break;
			case H : case BI : glRotatef(angle,0,1,0);  break;
			case B : case HI : glRotatef(angle,0,-1,0); break;
			case G : case DI : glRotatef(angle,-1,0,0);  break;
			case D : case GI : glRotatef(angle,1,0,0);  break;
		}

		for(int i=0; i<9; ++i)
		{
			FacesCube face;

			switch(next_move)
			{
				case A : case AI : face = FRONT;  break;
				case P : case PI : face = BACK;   break;
				case H : case HI : face = TOP;    break;
				case B : case BI : face = BOTTOM; break;
				case G : case GI : face = LEFT;   break;
				case D : case DI : face = RIGHT;  break;
			}

			in_move.push_back(moves_set[face][i]); 
			moves_set[face][i]->display(texture_square);
			
		}

		glPopMatrix();
	}


	/* AFFICHAGE DES AUTRES CUBES */
	for(int i=0; i<3; ++i)
		for(int j=0; j<3; ++j)
			for(int k=0; k<3; ++k)
				if(cubes[i][j][k] != NULL)
					if(find(in_move.begin(), in_move.end(), cubes[i][j][k]) == in_move.end())
						cubes[i][j][k]->display(texture_square);

	glPopMatrix();
}






void Rubik3x3::updateCubesPositions(RubikMoves move)
{
	Cube tmp, tmp2;

	switch(move)
	{
		case A :
			tmp  = *(cubes[0][0][0]);
			tmp2 = *(cubes[0][0][1]);
			cubes[0][0][0]->switchColors(cubes[0][0][2]);	cubes[0][0][0]->rotate(move);
			cubes[0][0][1]->switchColors(cubes[0][1][2]);	cubes[0][0][1]->rotate(move);
			cubes[0][0][2]->switchColors(cubes[0][2][2]);	cubes[0][0][2]->rotate(move);
			cubes[0][1][2]->switchColors(cubes[0][2][1]);	cubes[0][1][2]->rotate(move);
			cubes[0][2][2]->switchColors(cubes[0][2][0]);	cubes[0][2][2]->rotate(move);
			cubes[0][2][1]->switchColors(cubes[0][1][0]);	cubes[0][2][1]->rotate(move);
			cubes[0][2][0]->switchColors(&tmp);				cubes[0][2][0]->rotate(move);
			cubes[0][1][0]->switchColors(&tmp2);			cubes[0][1][0]->rotate(move);
			break;

		case P :
			tmp  = *(cubes[2][0][0]);
			tmp2 = *(cubes[2][1][0]);
			cubes[2][0][0]->switchColors(cubes[2][2][0]);	cubes[2][0][0]->rotate(move);
			cubes[2][1][0]->switchColors(cubes[2][2][1]);	cubes[2][1][0]->rotate(move);
			cubes[2][2][0]->switchColors(cubes[2][2][2]);	cubes[2][2][0]->rotate(move);
			cubes[2][2][1]->switchColors(cubes[2][1][2]);	cubes[2][2][1]->rotate(move);
			cubes[2][2][2]->switchColors(cubes[2][0][2]);	cubes[2][2][2]->rotate(move);
			cubes[2][1][2]->switchColors(cubes[2][0][1]);	cubes[2][1][2]->rotate(move);
			cubes[2][0][2]->switchColors(&tmp);				cubes[2][0][2]->rotate(move);
			cubes[2][0][1]->switchColors(&tmp2);			cubes[2][0][1]->rotate(move);
			break;

		case H :
			tmp  = *(cubes[0][2][0]);
			tmp2 = *(cubes[0][2][1]);
			cubes[0][2][0]->switchColors(cubes[0][2][2]);	cubes[0][2][0]->rotate(move);
			cubes[0][2][1]->switchColors(cubes[1][2][2]);	cubes[0][2][1]->rotate(move);
			cubes[0][2][2]->switchColors(cubes[2][2][2]);	cubes[0][2][2]->rotate(move);
			cubes[1][2][2]->switchColors(cubes[2][2][1]);	cubes[1][2][2]->rotate(move);
			cubes[2][2][2]->switchColors(cubes[2][2][0]);	cubes[2][2][2]->rotate(move);
			cubes[2][2][1]->switchColors(cubes[1][2][0]);	cubes[2][2][1]->rotate(move);
			cubes[2][2][0]->switchColors(&tmp);	            cubes[2][2][0]->rotate(move);
			cubes[1][2][0]->switchColors(&tmp2);	        cubes[1][2][0]->rotate(move);
			break;

		case B : 
			tmp  = *(cubes[0][0][0]);
			tmp2 = *(cubes[1][0][0]);
			cubes[0][0][0]->switchColors(cubes[2][0][0]);	cubes[0][0][0]->rotate(move);
			cubes[1][0][0]->switchColors(cubes[2][0][1]);	cubes[1][0][0]->rotate(move);
			cubes[2][0][0]->switchColors(cubes[2][0][2]);	cubes[2][0][0]->rotate(move);
			cubes[2][0][1]->switchColors(cubes[1][0][2]);	cubes[2][0][1]->rotate(move);
			cubes[2][0][2]->switchColors(cubes[0][0][2]);	cubes[2][0][2]->rotate(move);
			cubes[1][0][2]->switchColors(cubes[0][0][1]);	cubes[1][0][2]->rotate(move);
			cubes[0][0][2]->switchColors(&tmp);      	  	cubes[0][0][2]->rotate(move);
			cubes[0][0][1]->switchColors(&tmp2);			cubes[0][0][1]->rotate(move);
			break;

		case G :
			tmp  = *(cubes[0][0][0]);
			tmp2 = *(cubes[0][1][0]);
			cubes[0][0][0]->switchColors(cubes[0][2][0]);	cubes[0][0][0]->rotate(move);
			cubes[0][1][0]->switchColors(cubes[1][2][0]);	cubes[0][1][0]->rotate(move);
			cubes[0][2][0]->switchColors(cubes[2][2][0]);	cubes[0][2][0]->rotate(move);
			cubes[1][2][0]->switchColors(cubes[2][1][0]);	cubes[1][2][0]->rotate(move);
			cubes[2][2][0]->switchColors(cubes[2][0][0]);	cubes[2][2][0]->rotate(move);
			cubes[2][1][0]->switchColors(cubes[1][0][0]);	cubes[2][1][0]->rotate(move);
			cubes[2][0][0]->switchColors(&tmp);	            cubes[2][0][0]->rotate(move);
			cubes[1][0][0]->switchColors(&tmp2);	        cubes[1][0][0]->rotate(move);
			break;

		case D :
			tmp  = *(cubes[0][0][2]);
			tmp2 = *(cubes[1][0][2]);
			cubes[0][0][2]->switchColors(cubes[2][0][2]);	cubes[0][0][2]->rotate(move);
			cubes[1][0][2]->switchColors(cubes[2][1][2]);	cubes[1][0][2]->rotate(move);
			cubes[2][0][2]->switchColors(cubes[2][2][2]);	cubes[2][0][2]->rotate(move);
			cubes[2][1][2]->switchColors(cubes[1][2][2]);	cubes[2][1][2]->rotate(move);
			cubes[2][2][2]->switchColors(cubes[0][2][2]);	cubes[2][2][2]->rotate(move);
			cubes[1][2][2]->switchColors(cubes[0][1][2]);	cubes[1][2][2]->rotate(move);
			cubes[0][2][2]->switchColors(&tmp);	            cubes[0][2][2]->rotate(move);
			cubes[0][1][2]->switchColors(&tmp2);	        cubes[0][1][2]->rotate(move);
			break;

		case AI : 
			updateCubesPositions(A);
			updateCubesPositions(A);
			updateCubesPositions(A);
			break;

		case PI : 
			updateCubesPositions(P);
			updateCubesPositions(P);
			updateCubesPositions(P);
			break;

		case HI : 
			updateCubesPositions(H);
			updateCubesPositions(H);
			updateCubesPositions(H);
			break;

		case BI : 
			updateCubesPositions(B);
			updateCubesPositions(B);
			updateCubesPositions(B);
			break;

		case GI : 
			updateCubesPositions(G);
			updateCubesPositions(G);
			updateCubesPositions(G);
			break;

		case DI : 
			updateCubesPositions(D);
			updateCubesPositions(D);
			updateCubesPositions(D);
			break;
	}
}