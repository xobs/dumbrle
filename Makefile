all:
	gcc decode-dumbrle-chibi.c -o decode-dumbrle-chibi -Wall -Wextra
	gcc encode-dumbrle.c -o encode-dumbrle -Wall -Wextra
	gcc decode-dumbrle.c -o decode-dumbrle -Wall -Wextra
	gcc bin2c.c -o bin2c -Wall -Wextra
