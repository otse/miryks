// usage: developer command prompt > cl arrayhack.c

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
	void ** elements;
	int size;
};

struct element
{
	int id;
	const char *name;
};

typedef struct object object;
typedef struct array array;
typedef struct element element;

typedef object * objectp;
typedef element * elementp;
typedef element ** elementpp;
typedef element *** elementppp;

object * nobject();
array * narray(int);

void add(array *, element *, int);

object * nobject()
{
	object * object = malloc(sizeof(object));
	//object->array = narray(10);
	return object;
}

array * narray(int size)
{
	array * array = malloc(sizeof(array));
	array->size = size;
	array->elements = calloc(size, sizeof(element *));
	return array;
}

element * nelement()
{
	element * element = malloc(sizeof(element));
	return element;
}

void add(array * array, element * element, int i)
{
	array->elements[i] = element;
}

#define access(p) ((elementp)(p))

int main() {
	object * object = nobject();

	object->array = narray(10);

	element * me = nelement();
	me->id = 123;
	me->name = "boo";

	add(object->array, me, 1);

	// cool access without the use of an obscuring macro
	//((element **)&object->array)[0]->name;

	// normal access and requires a cast anyway
	//((element *)object->array->elements[0])->name;

	printf("the first element in our object's array is `%s`\n", ((element **)object->array->elements)[1]->name);
	printf("the first element in our object's array is `%s`\n", (*(elementpp *)object->array)[1]->name);
	//printf("the first element in our object's array is `%s`\n", ((element **)object->array)[1]->name);

	// here is a test with an "array of int pointers"

	int **pointers = calloc(10, sizeof(int *));
	pointers[2] = malloc(sizeof(int));
	*(int *)pointers[2] = 321;

	printf("      memory of pointers       is %i\n", pointers);
	printf("      memory of pointers[2]    is %i\n", pointers[2]);
	printf("      value  of pointers[2]    is %i\n", *(int*)pointers[2]);
	printf("      *      of pointers       is %i\n", *pointers);
	printf("      **     of pointers       is %i\n", *pointers[2]);
}