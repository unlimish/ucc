#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>

//Category token
typedef enum
{
	TK_RESERVED,	// symbols
	TK_NUM,			// integer token
	TK_EOF,			// end of input sign token
}	TokenKind;

typedef struct Token Token;

// type token
struct Token
{
	TokenKind kind;	// Token type
	Token *next;	// Next input token
	int		val;	// If kind is TK_NUM, that num
	char	*str;	// Token string
};

// Now pointing token
Token *token;

/* TODO HERE/
/  Error sending function */

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		fprintf(stderr, "Argment amount is wrong\n");
		return (1);
	}

	char *p;
	p = argv[1];

	printf(".intel_syntax noprefix\n");
	printf(".globl main\n");
	printf("main:\n");
	printf("  mov rax, %ld\n", strtol(p, &p, 10));

	while ((*p))
	{
		if (*p == '+')
		{
			p++;
			printf("  add rax, %ld\n", strtol(p, &p, 10));
			continue;
		}

		if (*p == '-')
		{
			p++;
			printf("  sub rax, %ld\n", strtol(p, &p, 10));
			continue;
		}

		fprintf(stderr, "Undefined char: '%c'\n", *p);
		return (1);
	}

	printf("  ret\n");
	return (0);
}
