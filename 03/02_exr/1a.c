#define _POSIX_C_SOURCE 200112L

#include <stdio.h>
#include <string.h>

#include "tokenizer.h"

int main(int argc, char *argv[]) {

	char *str = "NON_pROVARE_A_METTERE_QUALCHE_SPAZIO:)";

	tokenizer(str);
	tokenizer_r(str);

	return 0;
}