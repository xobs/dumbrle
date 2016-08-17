#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <strings.h>
#include <string.h>

#define STATE_ZEROES 0
#define STATE_DATA 1

struct dumb_rle_state {
  const uint8_t *data;
  int size;

  const uint8_t *read_offset; // Where in *data we're reading from
  int state;      // 0 = zeroes, 1 = data
  int state_left; // Number of bytes left in this state
  int bytes_left; // Number of bytes left until EOF.
};

static int dumb_rle_get_state(struct dumb_rle_state *state) {
  uint16_t type;

  memcpy(&type, state->read_offset, sizeof(type));

  if (state->bytes_left <= 0)
    return 0;

  state->read_offset += sizeof(type);
  state->bytes_left -= sizeof(type);

  /* If the high bit is set, then it's a run of zeroes. */
  if (type & 0x8000) {
    type &= ~0x8000;
    state->state = STATE_ZEROES;
  }
  else
    state->state = STATE_DATA;
  state->state_left = type;

  return 1;
}

void dumbRleInit(struct dumb_rle_state *state,
                 size_t size,
                 const uint8_t *data)
{

  memset(state, 0, sizeof(*state));
  state->data = data;
  state->size = size;

  state->read_offset = state->data;
  state->bytes_left = state->size;

  dumb_rle_get_state(state);
}

int dumbRleRead(struct dumb_rle_state *state, size_t size, uint8_t *data) {
  int count = 0;

  while (size) {
    /* If there are no bytes left in this state, get a new state */
    if (!state->state_left) {

      /* If we can't get a new state, assume EOF */
      if (!dumb_rle_get_state(state))
        return count;
    }

    if (state->state == STATE_ZEROES) {
      int to_copy = size;
      if (to_copy > state->state_left)
        to_copy = state->state_left;
      memset(data, 0, to_copy);

      state->state_left -= to_copy;
      data += to_copy;
      size -= to_copy;
      count += to_copy;
    }
    else if (state->state == STATE_DATA) {
      int to_copy = size;
      if (to_copy > state->state_left)
        to_copy = state->state_left;

      memcpy(data, state->read_offset, to_copy);
      state->read_offset += to_copy;
      state->bytes_left -= to_copy;
      state->state_left -= to_copy;
      data += to_copy;
      size -= to_copy;
      count += to_copy;
    }
  }

  return count;
}

int dumbRleEof(struct dumb_rle_state *state) {

  if ((state->bytes_left <= 0) && (state->state_left <= 0))
    return 1;
  return 0;
}

int main(int argc, char **argv) {

    char outfilename[128];
    uint8_t tempfile[3 * 1024 * 1024];
    struct dumb_rle_state state;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s [file]\n", argv[0]);
        return 1;
    }

    int infd = open(argv[1], O_RDONLY);
    if (infd == -1) {
        perror("Unable to open source file");
        return 1;
    }

    snprintf(outfilename, sizeof(outfilename) - 1, "%s.dec", argv[1]);
    int outfd = open(outfilename, O_WRONLY | O_CREAT | O_TRUNC, 0777);
    if (outfd == -1) {
        perror("Unable to open output file");
        return 1;
    }

    int insize = read(infd, tempfile, sizeof(tempfile));
    if (insize == -1) {
        perror("Unable to read file");
        return 1;
    }
    close(infd);

    fprintf(stderr, "insize: %d\n", insize);
    dumbRleInit(&state, insize, tempfile);

    while (!dumbRleEof(&state)) {
        uint8_t buffer[256];

        int count = dumbRleRead(&state, sizeof(buffer), buffer);
        if (count)
            write(outfd, buffer, count);
    }
    close(outfd);

    return 0;
}
