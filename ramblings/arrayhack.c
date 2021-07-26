// Using the Developer Command Prompt, cd to Gloom/ramblings and write > cl arrayhack.c

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

struct object;
struct array;
struct element;

struct object
{
	int id;
	struct array * array;
};

struct array
{
	struct element ** elements;
};

struct element
{
	int x;
};

typedef struct object object;
typedef struct array array;
typedef struct element element;
typedef object * objectp;
typedef array * arrayp;
typedef element * elementp;
typedef element ** elementpp;
typedef const object * cobjectp;

#define access(p) ((elementp)(p))

int main() {
	object object;

	printf("one is %i", 1);
}