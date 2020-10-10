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

// Error sending function
// Get same arg printf
void error(char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, "\n");
	exit (1);
}

bool consume(char op)
{
	if (token->kind != TK_RESERVED || token->str[0] != op)
		return false;
	token = token->next;
	return (true);
}

// If next token is expected symbol, read next one token
// If it is not, make error
void expect(char op)
{
	if (token->kind != TK_RESERVED || token->str[0] != op)
		error("It is not '%c'", op);
	token = token->next;
}

// If next toke is int, read next token then return it
// If it is not, return error
int expect_number()
{
	if (token->kind != TK_NUM)
		error("It is not int");
	int val = token->val;
	token = token->next;
	return (val);
}

bool at_eof()
{
	return (token->kind == TK_EOF);
}

// Make new token then conect to cur
Token *new_token(TokenKind kind, Token *cur, char *str)
{
	Token *tok = calloc(1, sizeof(Token));
	tok->kind = kind;
	tok->str = str;
	cur->next = tok;
	return (tok);
}

// Tokenize input char p then return it
Token *tokenize(char *p)
{
	Token head;
	head.next = NULL;
	Token *cur = &head;

	while (*p)
	{
		if (isspace(*p))
		{
			p++;
			continue;
		}

		if (*p == '+' || *p == '-')
		{
			cur = new_token(TK_RESERVED, cur, p++);
			continue;
		}

		if (isdigit(*p))
		{
			cur = new_token(TK_NUM, cur, p);
			cur->val = strtol(p, &p, 10);
			continue;
		}

		error("Cannot tokenize");
	}

	new_token(TK_EOF, cur, p);
	return (head.next);
}

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
