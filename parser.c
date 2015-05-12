#include "parser.h"

int activeToken = FALSE;
int line_no = 1;

void skipSpace()
{
	char c;

	c = getchar();
	line_no += (c == '\n');
	while (!feof(stdin) && isspace(c))
		{
			c = getchar();
			line_no += (c == '\n');
		}
	ungetc(c, stdin);
}

void ungetToken()
{
	activeToken = TRUE;
}

int scan_number()
{
	char c;

	c = getchar();
	if (isdigit(c) || c == '-')
		{
			if (c == '0')
				{
					token[tokenLength] = c;
					tokenLength++;
					token[tokenLength] = '\0';
				} else
				{
					while (isdigit(c) || c == '-')
						{
							token[tokenLength] = c;
							tokenLength++;
							c = getchar();
						}
					ungetc(c, stdin);
					token[tokenLength] = '\0';
				}
			return NUM;
		} else
		return ERROR;
}

int scan_id()
{
	int ttype;
	char c;

	c = getchar();
	if (isalpha(c))
		{
			while (isalnum(c))
				{
					token[tokenLength] = c;
					tokenLength++;
					c = getchar();
				}
			if (!feof(stdin))
				ungetc(c, stdin);

			token[tokenLength] = '\0';

			if (!strcmp(token, "STAGES"))
				return STAGES;

			ttype = ID;
			return ttype;
		} else
		return ERROR;
}

int getToken()
{
	char c;

	if (activeToken)
		{
			activeToken = FALSE;
			return ttype;
		}
	skipSpace();
	tokenLength = 0;
	c = getchar();
	switch (c) {
	case ':':
		return COLON;
	case ';':
		return SEMICOLON;
	case '{':
		return LBRACE;
	case '}':
		return RBRACE;
	case '#':
		return HASH;
	default:
		if (isdigit(c) || c == '-')
			{
				ungetc(c, stdin);
				return scan_number();
			}
		else if (isalpha(c))
			{
				ungetc(c, stdin);
				return scan_id();
			}
		else if (c == EOF)
		return EOF;
		else
		return ERROR;
	}
}

void syntax_error(const char* msg)
{
	printf("Syntax error while parsing %s at line %d.\n", msg, line_no);
	exit(1);
}

void parse_program()
{
	int i = 0;
	struct action_list* iter;
	a = 0;
	b = 0;

	ttype = getToken();
	if (ttype == LBRACE)
		{
			ttype = getToken();
			if (ttype != STAGES)
				syntax_error("program. STAGES expected");

			ttype = getToken();
			if (ttype != NUM)
				syntax_error("program. NUM expected");

			stages = atoi(token);

			parse_players();

			payoff_table = (struct pair***) malloc(sizeof(struct pair**) * player1->action_list->actions);
			nash1_table = (int**) malloc(sizeof(int*) * player1->action_list->actions);
			p1_strat_names = (char**) malloc(sizeof(char*) * player1->action_list->actions);
			p2_strat_names = (char**) malloc(sizeof(char*) * player2->action_list->actions);
			for (i = 0; i < player1->action_list->actions; i++)
				{
					payoff_table[i] = (struct pair**) malloc(sizeof(struct pair*) * player2->action_list->actions);
					nash1_table[i] = (int*) malloc(sizeof(int) * player2->action_list->actions);
				}

			iter = player1->action_list;
			i = 0;
			while (iter)
				{
					p1_strat_names[i++] = iter->action;
					iter = iter->next;
				}

			iter = player2->action_list;
			i = 0;
			while (iter)
				{
					p2_strat_names[i++] = iter->action;
					iter = iter->next;
				}

			parse_payoff_table();
			ttype = getToken();
			if (ttype != RBRACE)
				syntax_error("program. RBRACE expected");
		} else
		syntax_error("program. LBRACE expected");
}

void parse_players()
{
	ttype = getToken();
	if (isalnum(token[0]))
		{
			ungetToken();
			player1 = parse_player();
			player2 = parse_player();
		} else
		syntax_error("players. player expected");
}

struct player* parse_player()
{
	struct player* player;
	ttype = getToken();
	if (isalnum(token[0]))
		{
			player = (struct player*) malloc(sizeof(struct player));
			player->name = strdup(token);
			ttype = getToken();
			if (ttype == COLON)
				{
					player->action_list = parse_action_list();
					ttype = getToken();
					if (ttype != HASH)
						syntax_error("player. HASH expected");

					return player;
				} else
				syntax_error("player. COLON expected");
		} else
		syntax_error("player. name expected");

	return NULL ;
}

void parse_payoff_table()
{
	ttype = getToken();
	if (ttype == NUM)
		{
			ungetToken();
			parse_payoff_row();
			ttype = getToken();
			if (ttype == NUM)
				{
					ungetToken();
					parse_payoff_table();
				} else if (ttype == RBRACE)
				{
					if (a != player1->action_list->actions)
						syntax_error("parse_payoff_table. Too few payoffs");
					ungetToken();
				} else
				{
					syntax_error("payoff_table. payoff_table or RBRACE expected");
				}
		} else
		syntax_error("payoff_table. payoff_row expected");
}

void parse_payoff_row()
{
	ttype = getToken();
	if (ttype == NUM)
		{
			ungetToken();
			if (a == player1->action_list->actions)
				syntax_error("payoff_row. Too many payoffs");
			parse_payoff();
			ttype = getToken();
			if (ttype == NUM)
				{
					ungetToken();
					parse_payoff_row();
				} else if (ttype == HASH)
				{
					if (b < player2->action_list->actions)
						syntax_error("payoff_row. Too few payoffs");
					b = 0;
					a++;
				}
		} else
		syntax_error("payoff_row. payoff or HASH expected");
}

void parse_payoff()
{
	int p1;
	int p2;
	struct pair* payoff;

	ttype = getToken();
	if (ttype == NUM)
		{
			p1 = atoi(token);

			ttype = getToken();
			if (ttype == NUM)
				{
					p2 = atoi(token);
					payoff = (struct pair*) malloc(sizeof(struct pair));
					payoff->p1 = p1;
					payoff->p2 = p2;
					if (b == player2->action_list->actions)
						syntax_error("payoff. Too many payoffs");

					payoff_table[a][b++] = payoff;

					ttype = getToken();
					if (ttype == SEMICOLON)
						{
						} else if (ttype == HASH)
						{
							ungetToken();
						} else
						syntax_error("payoff. SEMICOLON or HASH expected");
				} else
				syntax_error("payoff. NUM1 expected");
		} else
		syntax_error("payoff. NUM expected");
}

struct action_list* parse_action_list()
{
	struct action_list* action_list;
	ttype = getToken();
	if (isalnum(token[0]))
		{
			action_list = (struct action_list*) malloc(sizeof(struct action_list));
			action_list->action = strdup(token);
			action_list->next = NULL;
			action_list->actions = 1;
			ttype = getToken();
			if (ttype != HASH)
				{
					ungetToken();
					action_list->next = parse_action_list();
					action_list->actions = action_list->next->actions + 1;

					return action_list;
				} else
				{
					ungetToken();

					return action_list;
				}
		} else
		syntax_error("action_list. action expected");

	return NULL ;
}

void print_players()
{
	printf("%s %d: ", player1->name, player1->action_list->actions);
	print_action_list(player1->action_list);
	printf("\n%s %d: ", player2->name, player2->action_list->actions);
	print_action_list(player2->action_list);
	printf("\n");
}

void print_action_list(struct action_list* action_list)
{
	struct action_list* iter = action_list;

	while (iter)
		{
			printf("%s ", iter->action);
			iter = iter->next;
		}
}

void print_payoff_table()
{
	for (a = 0; a < player1->action_list->actions; a++)
		{
			for (b = 0; b < player2->action_list->actions; b++)
				{
					printf("(%d, %d) ", payoff_table[a][b]->p1, payoff_table[a][b]->p2);
				}
			printf("\n");
		}
}

void print_strat(int player, int s)
{
	int i;
	if (player == 0)
		{
			for (i = 0; i < p1_strat_length; i++)
				printf("%s ", p1_strat_names[p1_strats[s][i]]);
		} else if (player == 1)
		{
			for (i = 0; i < p2_strat_length; i++)
				printf("%s ", p2_strat_names[p2_strats[s][i]]);
		}
}
