# Corey Finley                               
# 31 October 2013                             
# Addapted from makefile supplied on dropbox.                                   

#Specifies the files we will be compiling
COMPILING = extruding.c

#Specifies the executable's name
EXECUTABLE = Extrude

#Specifies the compilier we will be using
CC = g++

#Specifies the compiler flags to be used
# -g3 adds debugging symbols
# -Wall shows all warnings
CFLAGS = -g3 -Wall -Wno-non-template-friend -fpermissive

#Specifies the locations and libraries needed to compile the executable
LIBRARIES = -L"/usr/X11/lib" -lglut -lGL -lGLU -lm

all:
	$(CC) $(CFLAGS) $(LIBRARIES) $(COMPILING) -o $(EXECUTABLE)

clean:
	rm $(EXECUTABLE)
