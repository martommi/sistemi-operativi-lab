install:
	mkdir -p obj bin etc data
	mv *.o obj/
	mv client_handler bin/

clean:
	rm -f client server client_handle
	rm -f *.o
	rm -f obj/*.o bin/client_handle