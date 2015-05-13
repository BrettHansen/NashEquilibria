#include "nash.h"
#include "parser.h"

int m;
int n;
int p1_strats_size;
int p2_strats_size;
int count;
int sgp_count;
int show_sgp_only;
int *p2_br_payoff;

int del = 0;
struct sgp_node* sgp_nash_list;

void find_stage_nash()
{
	m = player1->action_list->actions;
	n = player2->action_list->actions;
	p1_strat_length = 1;
	p2_strat_length = 1;
	p1_strats_size = m;
	p2_strats_size = n;

	p1_strats = (int**)malloc(sizeof(int) * p1_strats_size);
	p2_strats = (int**)malloc(sizeof(int) * p2_strats_size);

	fill_strats(p1_strats, p1_strats_size, m, n, 1);
	fill_strats(p2_strats, p2_strats_size, n, m, 1);

	calculate_nash_all_sgp();
}

void calculate_nash_all_sgp()
{
	int a;
	int b;
	int max;
	p2_br_payoff = (int*)malloc(sizeof(int) * p1_strats_size);
	struct sgp_node* iter = NULL;

	for(a = 0; a < p1_strats_size; a++)
	{
		p2_br_payoff[a] = payoff_table[a][0]->p2;
		for(b = 0; b < p2_strats_size; b++)
		{
			max = payoff_table[a][b]->p2;
			if(max > p2_br_payoff[a])
				p2_br_payoff[a] = max;
		}
	}
	for(b = 0; b < p2_strats_size; b++)
	{
		// Determine column max
		max = payoff_table[0][b]->p1;
		for(a = 0; a < p1_strats_size; a++)
		{
			if(max > payoff_table[a][b]->p1)
				max = payoff_table[a][b]->p1;
		}

		// Check if column max are also row max
		for(a = 0; a < p1_strats_size; a++)
		{
			if(max == payoff_table[a][b]->p1 && p2_br_payoff[a] == payoff_table[a][b]->p2)
			{
				// SGP NASH
				printf("found nash!\n");
				if(!iter)
				{
					iter = (struct sgp_node*)malloc(sizeof(struct sgp_node));
					iter->s1 = a;
					iter->s2 = b;
					iter->next = NULL;
					sgp_nash_list = iter;
				}
				else
				{
					iter->next = (struct sgp_node*)malloc(sizeof(struct sgp_node));
					iter = iter->next;
					iter->s1 = a;
					iter->s2 = b;
					iter->next = NULL;
				}
			}
		}
	}
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

int main()
{
	parse_program();

	show_sgp_only = FALSE;

	find_stage_nash();

	struct sgp_node* iter = sgp_nash_list;
	int i;
	for(i = 0; i < p1_strats_size; i++)
		printf("%d ", p2_br_payoff[i]);
	printf("\n");

	while(iter)
	{
		printf("%d, %d\n", iter->s1, iter->s2);
		iter = iter->next;
	}

	return 0;
}
