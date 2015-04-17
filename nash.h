/* nash.h */
#ifndef NASH_H
#define NASH_H

struct sgp_node
{
	int* s1;
	int* s2;
	struct sgp_node* next;
};

void find_all_nash();
void build_strategies(int s);
double s_for_overflow(int b, int d);
double log_base(double b, double n);
void fill_strats(int** all_strats, int size, int a, int b, int s);
void convert_to_bits_base(int* store, int num, int base);
void reverse(int* store, int size);
void calculate_nash();
int* calculate_payoff(int* payoff, int s1, int s2);
int test_and_add_sgp(int a, int b, int s);
int array_match(int* a1, int* a2, int offset, int n);

#endif /* NASH_H */
