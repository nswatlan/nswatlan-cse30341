
CMP = gcc 
MAIN = myshell
EXEC = shell
$(EXEC): $(MAIN).o 
	$(CMP) $(MAIN).o  -lm -o $(EXEC)

$(MAIN).o: $(MAIN).c 
	$(CMP) -std=c99 -Wall -c $(MAIN).c -o $(MAIN).o 

clean:
	rm *.o
	rm $(EXEC) 
