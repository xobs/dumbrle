all:
	gcc decode-chibi.c -o decode-chibi -Wall -Wextra
	gcc dumbrle.c -o dumbrle -Wall -Wextra
	gcc decode.c -o decode -Wall -Wextra
	gcc bin2c.c -o bin2c -Wall -Wextra
