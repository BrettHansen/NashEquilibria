#include "nash.h"
#include "parser.h"

int m;
int n;
int p1_strats_size;
int p2_strats_size;
int count;
int sgp_count;
int show_sgp_only;

int del = 0;
struct sgp_node** sgp_nash_list;

void find_all_nash()
{
	int i;
	sgp_nash_list = (struct sgp_node**) malloc(sizeof(struct sgp_node*) * stages);
	for (i = 0; i < stages; i++)
		{
			sgp_nash_list[i] = NULL;
			build_strategies(i + 1);
			calculate_nash(i + 1);
		}
}

void build_strategies(int s)
{
	m = player1->action_list->actions;
	n = player2->action_list->actions;
	p1_strat_length = (s - 1) * n + 1;
	p2_strat_length = (s - 1) * m + 1;

	p1_strats_size = pow(m, p1_strat_length);
	p2_strats_size = pow(n, p2_strat_length);

	p1_strats = (int**) malloc(sizeof(int) * p1_strats_size);
	p2_strats = (int**) malloc(sizeof(int) * p2_strats_size);

	fill_strats(p1_strats, p1_strats_size, m, n, s);
	fill_strats(p2_strats, p2_strats_size, n, m, s);
}

void fill_strats(int** all_strats, int size, int a, int b, int s)
{
	int i;
	int j;
	for (i = 0; i < size; i++)
		{
			all_strats[i] = (int*) malloc(sizeof(int) * ((s - 1) * b + 1));
			for (j = 0; j < ((s - 1) * b + 1); j++)
				{
					all_strats[i][j] = 0;
				}
			convert_to_bits_base(all_strats[i], i, a);
			reverse(all_strats[i], (s - 1) * b + 1);
		}
}

void convert_to_bits_base(int* store, int num, int base)
{
	int i = 0;
	while (num > 0)
		{
			store[i++] = num % base;
			num /= base;
		}
}

void reverse(int* store, int size)
{
	int i = 0;
	int temp;
	while (i < size / 2)
		{
			temp = store[size - 1 - i];
			store[size - 1 - i] = store[i];
			store[i++] = temp;
		}
}

//	(1) Find the max payoff for each strategy for p1.
//	(2) Find all non-deviating positions for p2.
//	(3) If a p2-nash has a p1-payoff equal to row-max, it's a nash.
void calculate_nash(int s)
{
	int max;
	int* payoff = (int*) malloc(sizeof(int) * 2);
	int* col_max;
	int is_sgp;
	count = 0;
	sgp_count = 0;

	payoff[0] = 0;
	payoff[1] = 0;
	a = 0;
	col_max = (int*) malloc(sizeof(int) * p2_strats_size);
	for (b = 0; b < p2_strats_size; b++)
		{
			max = calculate_payoff(payoff, a, b)[0];
			for (a = 0; a < p1_strats_size; a++)
				{
					if (calculate_payoff(payoff, a, b)[0] > max)
						max = payoff[0];
				}
			col_max[b] = max;
			a = 0;

		}
	printf("NASH EQUILIBRIA\n");
	for (a = 0; a < p1_strats_size; a++)
		{
			b = 0;
			max = calculate_payoff(payoff, a, b)[1];
//		Find max in row
			for (b = 0; b < p2_strats_size; b++)
				{
					if (calculate_payoff(payoff, a, b)[1] > max)
						max = payoff[1];
				}
//		Test max in row for nashity
			for (b = 0; b < p2_strats_size; b++)
				{
					if (calculate_payoff(payoff, a, b)[1] == max && payoff[0] == col_max[b])
						{
							is_sgp = test_and_add_sgp(a, b, s);
							if (is_sgp)
								sgp_count++;
							if (!show_sgp_only || is_sgp)
								{
									printf("%2d:  ", count++);
									print_strat(0, a);
									printf("\t");
									print_strat(1, b);
									printf("\t%d, %d\t\t%s\n", payoff[0], payoff[1], is_sgp ? "SGP" : "");
								}
						}
				}
		}
	printf("Total NE for %d stage%s:     %d\n", s, s == 1 ? "" : "s", count);
	printf("Total SGP NE for %d stage%s: %d\n", s, s == 1 ? "" : "s", sgp_count);

	if (count == 0)
		printf("There are none!\n");
}

int* calculate_payoff(int* payoff, int s1, int s2)
{
	int p1_offset = 1;
	int p2_offset = 1;
	int p1_choice = p1_strats[s1][0];
	int p2_choice = p2_strats[s2][0];
	int temp_store;
//	First Choice
	payoff[0] = payoff_table[p1_choice][p2_choice]->p1;
	payoff[1] = payoff_table[p1_choice][p2_choice]->p2;

//	Successive Choices
	while (p1_offset < p1_strat_length)
		{
			temp_store = p1_choice;
			p1_choice = p1_strats[s1][p1_offset + p2_choice];
			p2_choice = p2_strats[s2][p2_offset + temp_store];

			payoff[0] += payoff_table[p1_choice][p2_choice]->p1;
			payoff[1] += payoff_table[p1_choice][p2_choice]->p2;

			p1_offset += n;
			p2_offset += m;
		}

	return payoff;
}

int test_and_add_sgp(int a, int b, int s)
{
	struct sgp_node* node;
	struct sgp_node* iter;
	int i;
	int is_sgp;

//	All 1-stage nash are SGP
	if (s != 1)
		{
			is_sgp = FALSE;
			iter = sgp_nash_list[s - 2];
			while (iter)
				{
//			Player's first contingent choice must match non-contingent choice of n - 1 game.
					if (p1_strats[a][p2_strats[b][0] + 1] == iter->s1[0] && p2_strats[b][p1_strats[a][0] + 1] == iter->s2[0])
						{
							if (array_match(p1_strats[a], iter->s1, n, p1_strat_length) && array_match(p2_strats[b], iter->s2, m, p2_strat_length))
								{
									is_sgp = TRUE;
									break;
								}
						}

					iter = iter->next;
				}
		} else
		is_sgp = TRUE;

	if (!is_sgp)
		return FALSE;

//	Will add node into linked list of appropriate stage
	node = (struct sgp_node*) malloc(sizeof(struct sgp_node));
	node->s1 = p1_strats[a];
	node->s2 = p2_strats[b];
	node->next = NULL;
	if (sgp_count == 0)
		sgp_nash_list[s - 1] = node;
	else
		{
			iter = sgp_nash_list[s - 1];
			for (i = 0; i < sgp_count - 1; i++)
				iter = iter->next;

			iter->next = node;
		}
	return TRUE;
}

int array_match(int* n_stage, int* n_1_stage, int offset, int length)
{
	int i;
	for (i = 1; i < length - offset; i++)
		{
			if (n_stage[i + offset] != n_1_stage[i])
				return FALSE;
		}
	return TRUE;
}

int main()
{
	parse_program();

	show_sgp_only = FALSE;

	find_all_nash();

	return 0;
}

double s_for_overflow(int b, int d)
{
	return (log_base(b, INT_MAX) - 1) / d + 1;
}

double log_base(double b, double n)
{
	return log(n) / log(b);
}
