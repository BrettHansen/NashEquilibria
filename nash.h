/* nash.h */
#ifndef NASH_H
#define NASH_H

struct sgp_node
	{
		int s1;
		int s2;
		struct sgp_node* next;
	};

void find_stage_nash();
void calculate_nash_all_sgp();
void fill_strats(int** all_strats, int size, int a, int b, int s);
void convert_to_bits_base(int* store, int num, int base);
void reverse(int* store, int size);

#endif /* NASH_H */
