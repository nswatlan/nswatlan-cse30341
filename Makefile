#makefile

CMP = gcc -std=c99 -Wall
MAIN = dirwatch
EXEC = dirlist

$(EXEC): $(MAIN).o 
	$(CMP) $(MAIN).o -lm -o $(EXEC)

$(MAIN).o: $(MAIN).c 
	$(CMP) -c $(MAIN).c -o $(MAIN).o 

clean:
	rm *.o
	rm $(EXEC) 