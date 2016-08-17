#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

static int done;

static int get_next_char(int infd, char *c) {
    int ret;

    ret = read(infd, c, 1);
    if (ret != 1) {
        if (ret == -1)
            perror("Unable to read");
        else
            fprintf(stderr, "Done.\n");
        done = 1;
        return 1;
    }
    return 0;
}

static int print_usage(const char *progname) {
    fprintf(stderr, "Usage: %s infile [outfile]\n", progname);
    return 0;
}

int main(int argc, char **argv) {
    char buffer[65536];
    int buffer_size;
    char c;

    if (argc < 1) {
        print_usage(argv[0]);
        return 1;
    }

    const char *infilename = argv[1];
    int infd = open(infilename, O_RDONLY);
    if (infd == -1) {
        perror("Unable to open source file");
        return 1;
    }

    char outfilename[1024];
    if (argc < 2)
        snprintf(outfilename, sizeof(outfilename) - 1, "%s.rle", infilename);
    else
        strncpy(outfilename, argv[2], sizeof(outfilename) - 1);

    int outfd = open(outfilename, O_WRONLY | O_CREAT | O_TRUNC, 0777);
    if (outfd == -1) {
        perror("Unable to open output file");
        return 1;
    }

    if (get_next_char(infd, &c))
        done = 1;

    while (!done) {

        buffer_size = 1;
        if (c == 0) {
            while( (get_next_char(infd, &c) == 0)
                && (c == 0)
                && (buffer_size < 32767))
                buffer_size++;
            uint16_t outpkt = 0x8000 | buffer_size;
            write(outfd, &outpkt, sizeof(outpkt));
        }
        else {
            buffer[0] = c;
            while( (get_next_char(infd, &c) == 0)
                && (c != 0)
                && (buffer_size < 32767))
                buffer[buffer_size++] = c;
            uint16_t outpkt = buffer_size;
            write(outfd, &outpkt, sizeof(outpkt));
            write(outfd, buffer, buffer_size);
        }
    }

    close(infd);
    close(outfd);
    return 0;
}
