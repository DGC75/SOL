#define _POSIX_C_SOURCE 200112L

#include "tokenizer.h"

int main(int argc, char *argv[]) {
    if (argc != 3) {
	fprintf(stderr, "use: %s stringa1 stringa2\n", argv[0]);
	return -1;
    }

    char *state_str = NULL;

    char* token1 = strtok_r(argv[1], " ", &state_str);

    while (token1) {
	printf("%s\n", token1);
	char* token2 = strtok_r(argv[2], " ", &state_str);
	while(token2) {
	    printf("%s\n", token2);
	    token2 = strtok_r(NULL, " ", &state_str);
	}
	token1 = strtok_r(NULL, " ", &state_str);
    }
    return 0;
}