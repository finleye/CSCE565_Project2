# Corey Finley                               
# 31 October 2013                             
# Addapted from makefile supplied on dropbox.                                   

#Specifies the files we will be compiling
COMPILING = extrude.c

#Specifies the executable's name
EXECUTABLE = Extrude

#Specifies the compilier we will be using

#Linux
CC = g++

#Mac
# CC = gcc


#Specifies the compiler flags to be used
# -g3 adds debugging symbols
# -Wall shows all warnings
CFLAGS = -g3 -Wall -Wno-non-template-friend -fpermissive

#Specifies the locations and libraries needed to compile the executable
# Linux
LIBRARIES = -L "/usr/X11/lib" 
LIBRARIES += -lglut -lGL -lGLU -lm

# Mac
# LIBRARIES += -framework OpenGL -framework GLUT

all:
	$(CC) $(CFLAGS) $(LIBRARIES) $(COMPILING) -o $(EXECUTABLE)

clean:
	rm $(EXECUTABLE)
