#include <Master.h>



Master::Master(string path)
:path(path)
{
	/* INIT MASTER */
	mode = RESOLVER;

	resolver = NULL;
	editor = NULL;

	/* INIT SDL */
	SDL_Init(SDL_INIT_EVERYTHING);

	const SDL_VideoInfo* screen = SDL_GetVideoInfo();

	width  = screen->current_w;
	height = screen->current_h;
	int ratio = width/height;

	SDL_Init(SDL_INIT_VIDEO);
    SDL_WM_SetCaption("RubikSolver",NULL);
    SDL_SetVideoMode(width, height, 32, SDL_OPENGL | SDL_NOFRAME);

    /* FONT */
    TTF_Init();
	font =       TTF_OpenFont((path+"/res/fonts/Roboto-Regular.ttf").c_str(), 100);
	font_small = TTF_OpenFont((path+"/res/fonts/Roboto-Light.ttf").c_str(), 100);

	if(!font || !font_small)
	{
		cerr << "Police introuvable" << endl;
		exit(0);
	}

	/* INIT OPENGL */
    glClearColor(0.1,0.1,0.1,1.);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LINE_SMOOTH);
	glEnable(GL_BLEND);
	glDisable(GL_CULL_FACE);

	glShadeModel(GL_SMOOTH);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0,width,0,height,600.0f,-600.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	/* INIT OBJECTS */
	browser = new Browser(this,font_small,path+"/saves/");
	resolver = new Resolver(this,font);
}




Master::~Master()
{
	if(resolver != NULL) delete resolver;
	if(editor != NULL)   delete editor;

	TTF_CloseFont(font);
	TTF_Quit();
	SDL_Quit();
}




void Master::run()
{
	is_running = true;
    SDL_Event event;

    int tx = 0;
    int ty = 0;
    int dt = 0;

    while (is_running)
    {
    	ty = SDL_GetTicks();
    	dt = ty - tx;
    	tx = ty;

	    while(SDL_PollEvent(&event))
	    {
	        switch(event.type)
	        {
	            case SDL_QUIT :
	                is_running = false;
	                break;

	            case SDL_MOUSEBUTTONDOWN :
	            	if(mode == RESOLVER)
	            		resolver->gestionClickButtons(&event,false);
	            	break;

	            case SDL_MOUSEBUTTONUP :
	            	if(mode == RESOLVER)
	            		resolver->gestionClickButtons(&event,true);
	            	break;

	            case SDL_MOUSEMOTION :
	            	if(mode == RESOLVER)
	            		resolver->gestionMouseMotion(&event);
	            	break;

	            case SDL_KEYUP :
	            	if(mode == RESOLVER)
	            		resolver->gestionButton(&event);
	            	break;
	        }
	    }

    	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glLoadIdentity();

		animate(dt);
		display();

        SDL_GL_SwapBuffers();
        glFlush();
    }

    SDL_Quit();
}




void Master::animate(int dt)
{
	if(mode == RESOLVER)
    	resolver->animate(dt);
}





void Master::display()
{
	if(mode == RESOLVER)
    	resolver->display();
}