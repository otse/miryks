#ifndef NIF_NIF_H
#define NIF_NIF_H

#include <sstream>

using namespace std;

// https://gist.github.com/EmilHernvall/953968

#define api

struct nif_hedr_t
{
    char str[18];
};

struct nif_t
{
    nif_hedr_t hdr;
    int n;
    const char *path;
    const unsigned char *buf;
};

struct nmap_t {
	void *key;
	nif_t *value;
};
extern nmap_t nmap[1000];
extern int nifs;

void nif_gui();

api void nif_make(void *, nif_t *);
api void nif_add(void *, nif_t *);
api nif_t *nif_get(const char *);

api void nif_print_hedr(nif_t &, stringstream &);

#endif