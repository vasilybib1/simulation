.SUFFIXES:
.SUFFIXES: .c .o

CC = gcc
CFLAGS = -g -Wno-deprecated-declarations 
LLOC = -L/opt/homebrew/lib 
ILOC = -I/opt/homebrew/include
LDLIBS = -lglfw
FRAMEWORK = -framework OpenGL

OBJDIR = obj

vpath %.c src
vpath %.h src

OBJ = shader.o sim.o util.o verletObj.o simulation.o
HEADER = shader.h util.h verletObj.h sim.h
objects = $(addprefix $(OBJDIR)/, $(OBJ))

simulation: $(objects)
	$(CC) $(CFLAGS) -o simulation $(objects) $(ILOC) $(LLOC) $(LDLIBS) $(FRAMEWORK) 

.PHONY: clean
clean:
	$(RM) -r *~ *.o $(OBJDIR)/*.o *.dSYM simulation

$(OBJDIR) obj/%.o: %.c $(HEADER)
	$(CC) $(CFLAGS) -c $(ILOC) $< -o $@
