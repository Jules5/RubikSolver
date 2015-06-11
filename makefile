#Makefile Linux
EXEC = RubikSolver
EXEC_EDIT = RubikEditor
COMPILO = g++
OPTION = `sdl-config --cflags --libs` 

#Path
PATH_INC = include
PATH_LIB = lib
PATH_SRC = src
PATH_OBJ = obj

#Directories
OBJ_DIR = obj

#Librairies
LIB_DEP = -lGL -lGLU -lglut -lSDL -lSDL_image -lSDL_ttf

#Liste fichiers .cpp et .O
SRC = $(wildcard $(PATH_SRC)/*.cpp) 
OBJ = $(SRC:$(PATH_SRC)/%.cpp=$(PATH_OBJ)/%.o)
SRC_EDIT = $(wildcard $(PATH_SRC)/editor/*.cpp)
OBJ_EDIT = $(SRC_EDIT:$(PATH_SRC)/editor/%.cpp=$(PATH_OBJ)/editor/%.o)

#Actions
all : $(EXEC) $(EXEC_EDIT)
	./$(EXEC)

editor : $(EXEC_EDIT)
	./$(EXEC_EDIT) 

valgrind : $(EXEC)
	valgrind ./$(EXEC)

debug : $(EXEC)
	gdb ./$(EXEC)

$(EXEC) : $(OBJ)
	@echo "\033[31m[Link] $(EXEC)\033[00m"
	@$(COMPILO) $(OPTION) -std=c++11 -o $@ $^ $(LIB_DEP)

$(PATH_OBJ)/%.o: $(PATH_SRC)/%.cpp
	@echo "\033[32m[Build] $@\033[00m"
	@$(COMPILO) $(OPTION) -o $@ -c $< -I$(PATH_INC)

$(EXEC_EDIT) : $(OBJ_EDIT) $(PATH_OBJ)/Color.o
	@echo "\033[31m[Link] $(EXEC_EDIT)\033[00m"
	@$(COMPILO) $(OPTION) -std=c++11 -o $@ $^ $(LIB_DEP)

$(PATH_OBJ)/editor/%.o: $(PATH_SRC)/editor/%.cpp
	@echo "\033[32m[Build] $@\033[00m"
	@$(COMPILO) $(OPTION) -o $@ -c $< -I$(PATH_INC)

clean :
	@echo "Suppression des .o et fichiers temporaires"
	@find . -name '*.o' -exec rm  {} \;
	@find . -name '*~' -exec rm  {} \;

commit :
	git commit -a
	git push

add :
	@git add . --verbose

init :
	mkdir -p obj obj/editor