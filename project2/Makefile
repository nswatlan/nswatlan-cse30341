
CMP = gcc 
MAIN = myshell
EXEC = shell
$(EXEC): $(MAIN).o 
	$(CMP) $(MAIN).o -lm -o $(EXEC)

$(MAIN).o: $(MAIN).c 
	$(CMP) -c $(MAIN).c -o $(MAIN).o 

clean:
	rm *.o
	rm $(EXEC) 
