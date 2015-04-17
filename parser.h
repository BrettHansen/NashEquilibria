/* parser.h */
#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <limits.h>

enum Bool {
	FALSE = 0, TRUE
};

enum TokenTypes {
	HASH = 1, COLON, SEMICOLON, LBRACE, RBRACE, NUM, ID, ERROR, STAGES
};

struct player {
	char* name;
	struct action_list* action_list;
};

struct action_list {
	char* action;
	struct action_list* next;
	int actions;
};

struct pair {
	int p1;
	int p2;
};

#define MAX_TOKEN_LENGTH 100
char token[MAX_TOKEN_LENGTH]; // token string
int ttype; // token type
int activeToken;
int tokenLength;
int line_no;
struct player* player1;
struct player* player2;
int a;
int b;
int** nash1_table;
char** p1_strat_names;
char** p2_strat_names;
struct pair*** payoff_table;
int p1_strat_length;
int p2_strat_length;
int** p1_strats;
int** p2_strats;
int stages;

void skipSpace();
void ungetToken();
int scan_number();
int scan_id();
int getToken();
void syntax_error(const char* msg);
void parse_program();
void parse_players();
struct player* parse_player();
struct action_list* parse_action_list();
void parse_payoff_table();
void parse_payoff_row();
void parse_payoff();
void print_players();
void print_action_list(struct action_list* action_list);
void print_payoff_table();
void print_strat(int player, int s);

#endif /* PARSER_H */
