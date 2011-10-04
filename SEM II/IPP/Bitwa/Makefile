MAIN=game

# odpalenie wszystkiego
start: $(MAIN)

# ogolny sposob kompilacji programow w pascalu
$(MAIN): %: %.pas
	fpc $<
	
# zaleznosc game od modulow
game: engine.ppu engine.o ai.ppu ai.o

ai.ppu: engine.ppu engine.o

# kompilacja modulow
%.ppu %.o: %.pas
	fpc $<

# czyszczenie
clean:
	rm -f $(MAIN) *.o *.ppu
