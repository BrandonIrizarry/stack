#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Stack {
	const size_t num_cells; // number of cells.
	size_t top; // next free cell.
	
	void **cells; // the storage.
	void (*destructor) (void *); // the destructor for the
				     // individual data items.

	bool full, empty;
};

struct Stack *stack_create (size_t num_cells, size_t datasize, void (*destructor) (void *)) {
	assert(num_cells >= 1);
	
	struct Stack prototype = {
		.num_cells = num_cells,
		.top = 0,
		.cells = calloc(num_cells, datasize),
		.destructor = destructor,
		.full = false,
		.empty = true
	};
		
	struct Stack *new_stack = malloc(sizeof(*new_stack));

	memcpy(new_stack, &prototype, sizeof(prototype));

	return new_stack;
}

void foreach (struct Stack *stack, void (*fn) (void *)) {
	size_t top = stack->top;
	
	for (size_t i = 0; i < top; i++) {
		fn(stack->cells[i]);
	}
}

struct Stack stack_destroy (struct Stack *stack) {
	foreach(stack, stack->destructor);
}

bool stack_push (struct Stack *stack, void *item) {
	if (stack->full) {
		return false;
	}
	
	stack->cells[stack->top] = item;
	stack->top++;

	if (stack->top == stack->num_cells) {
		stack->full = true;
	}

	return true;
}

void *stack_pop (struct Stack *stack) {
	if (stack->empty) {
		return NULL;
	}
	
	void *top_element = stack->cells[stack->top];

	stack->top--;

	if (stack->top == 0) {
		stack->empty = true;
	}

	return top_element;
}

/* int stacks */
void print_int (void *int_cell) {
	int *cell = (int *)int_cell;

	printf("%d ", *cell);
}

void int_stack_print (struct Stack *int_stack) {
	foreach(int_stack, print_int);

	printf("\n");
}

void int_dtor (void *int_cell) {
	int *cell = (int *)int_cell;
	
	free(int_cell);
}

int *int_cell (int x) {
	int *cell = malloc(sizeof(int));
	*cell = x;

	return cell;
}

struct Stack *int_stack_create (size_t num_cells) {
	return stack_create(num_cells, sizeof(int), int_dtor);
}

/* end int stacks */

/* char stacks */
// reuse int_cell, and int_dtor.
char *export_string (struct Stack *char_stack) {
	size_t num_cells = char_stack->num_cells;
	size_t top = char_stack->top;
	
	char *buffer = calloc(num_cells + 1, 1);

	for (int i = 0; i < top; i++) {
		buffer[i] = *(char *)char_stack->cells[i];
	}

	return buffer;
}

void char_stack_print (struct Stack *char_stack) {
	char *buffer = export_string(char_stack);

	printf("%s", buffer);

	free(buffer);
}

struct Stack *char_stack_create (size_t num_cells) {
	return int_stack_create(num_cells);
}

/* end char stacks */


int main (void) {
	struct Stack *int_stack = int_stack_create(5);

	stack_push(int_stack, int_cell(1));
	stack_push(int_stack, int_cell(2));
	stack_push(int_stack, int_cell(-3));
	stack_push(int_stack, int_cell(44));
	assert(stack_push(int_stack, int_cell('*')));

	int_stack_print(int_stack);


	struct Stack *char_stack = char_stack_create(20);
	stack_push(char_stack, int_cell('s'));
	stack_push(char_stack, int_cell('t'));
	stack_push(char_stack, int_cell('a'));
	stack_push(char_stack, int_cell('r'));
	stack_push(char_stack, int_cell('\n'));

	char_stack_print(char_stack);
	return 0;
}
