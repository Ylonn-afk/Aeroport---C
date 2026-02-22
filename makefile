# Nom de l'exécutable
EXEC = aeroport

# Compilateur
CC = gcc

# Options de compilation
CFLAGS = -Wall -I/usr/include/GL

# Librairies OpenGL / GLUT
LIBS = -lglut -lGLU -lGL -lX11 -lm

# Tous les fichiers sources
SRC = affichage.c projet.c \
      BmpLib.c ErreurLib.c ESLib.c GfxLib.c \
      OutilsLib.c SocketLib.c ThreadLib.c \
      TortueLib.c VectorLib.c WavLib.c

# Génère automatiquement la liste des .o
OBJ = $(SRC:.c=.o)

# Règle principale
all: $(EXEC)

# Edition de liens
$(EXEC): $(OBJ)
	$(CC) $(OBJ) -o $(EXEC) $(LIBS)

# Compilation des .c en .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Nettoyage
clean:
	rm -f *.o

# Nettoyage complet
mrproper: clean
	rm -f $(EXEC)

