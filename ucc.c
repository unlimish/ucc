#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

char *user_input;

void error_at(char *loc, char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);

  int pos = loc - user_input;
  fprintf(stderr, "%s\n", user_input);
  fprintf(stderr, "%*s", pos, ""); // output pos amount of space;
  fprintf(stderr, "^ ");
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

// Category token
typedef enum {
  TK_RESERVED, // symbols
  TK_NUM,      // integer token
  TK_EOF,      // end of input sign token
} TokenKind;

typedef struct Token Token;

// type token
struct Token {
  TokenKind kind; // Token type
  Token *next;    // Next input token
  int val;        // If kind is TK_NUM, that num
  char *str;      // Token string
};

// Now pointing token
Token *token;

// Error sending function
// Get same arg printf
void error(char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

bool consume(char op) {
  if (token->kind != TK_RESERVED || token->str[0] != op)
    return false;
  token = token->next;
  return true;
}

// If next token is expected symbol, read next one token
// If it is not, make error
void expect(char op) {
  if (token->kind != TK_RESERVED || token->str[0] != op)
    error("It is not '%c'", op);
  token = token->next;
}

// If next toke is int, read next token then return it
// If it is not, return error
int expect_number() {
  if (token->kind != TK_NUM)
    error("It is not int");
  int val = token->val;
  token = token->next;
  return (val);
}

bool at_eof() { return (token->kind == TK_EOF); }

// Make new token then conect to cur
Token *new_token(TokenKind kind, Token *cur, char *str) {
  Token *tok = calloc(1, sizeof(Token));
  tok->kind = kind;
  tok->str = str;
  cur->next = tok;
  return (tok);
}

// Tokenize input char p then return it
Token *tokenize(char *p) {
  Token head;
  head.next = NULL;
  Token *cur = &head;

  while (*p) {
    if (isspace(*p)) {
      p++;
      continue;
    }

    if (*p == '+' || *p == '-') {
      cur = new_token(TK_RESERVED, cur, p++);
      continue;
    }

    if (isdigit(*p)) {
      cur = new_token(TK_NUM, cur, p);
      cur->val = strtol(p, &p, 10);
      continue;
    }

    error("Cannot tokenize");
  }

  new_token(TK_EOF, cur, p);
  return (head.next);
}

// Node type of abstract syntax tree
typedef enum
{
  ND_ADD, // +
  ND_SUB, // -
  ND_MUL, // *
  ND_DIV, // /
  ND_NUM, // int
} NodeKind;

typedef struct Node Node;

struct Node
{
  NodeKind kind;  // Type of node
  Node *lhs;      // Left side
  Node *rhs;      // Right side
  int val;        // use it if kind is ND_NUM
};

Node *new_node(NodeKind kind)
{
  Node *node = calloc(1, sizeof(Node));
  node->kind = kind;
  return (node);
}

Node *new_binary(NodeKind kind, Node *lhs, Node *rhs)
{
  Node *node = new_node(kind);
  node->lhs = lhs;
  node->rhs = rhs;
  return (node);
}

Node *new_num(int val)
{
  Node *node = new_node(ND_NUM);
  node->val = val;
  return (node);
}

Node *expr();
Node *mul();
Node *primary();

Node *expr()
{
  Node *node = mul();

  for (;;)
  {
    if (consume('+'))
      node = new_binary(ND_ADD, node, mul());
    else if (consume('-'))
      node = new_binary(ND_SUB, node, mul());
    else
      return (node);
  }
}

Node *mul()
{
  Node *node = primary();

  for (;;)
  {
    if (consume('*'))
      node = new_binary(ND_MUL, node, primary());
    else if (consume('/'))
      node = new_binary(ND_DIV, node, primary());
    else
      return (node);
  }
}

Node *primary()
{
  if (consume('('))
  {
    Node *node = expr();
    expect(')');
    return (node);
  }

  return new_num(expect_number());
}

void gen(Node *node)
{
  if (node->kind == ND_NUM)
  {
    printf("  push %d\n", node->val);
    return ;
  }

  gen(node->lhs);
  gen(node->rhs);

  printf("  pop rdi\n");
  printf("  pop rax\n");

  switch (node->kind)
  {
  case ND_ADD:
    printf("  add rax, rdi\n");
    break;
  case ND_SUB:
    printf("  sub rax, rdi\n");
    break;
  case ND_MUL:
    printf("  imul rax, rdi\n");
    break;
  case ND_DIV:
    printf("  cqo\n");
    printf("  idiv rdi\n");
    break;
  }

  printf("  push rax\n");
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Argment amount is wrong\n");
    return (1);
  }

  // Tokenize then perse
  user_input = argv[1];
  token = tokenize(user_input);
  Node *node = expr();

  // Output first part of assembly
  printf(".intel_syntax noprefix\n");
  printf(".globl main\n");
  printf("main:\n");

  // Gen code with downing abstract tree
  gen(node);

  // The result is on stack top
  // Load to RAX then make it as return function
  printf("  pop rax\n");
  printf("  ret\n");
  return (0);
}
