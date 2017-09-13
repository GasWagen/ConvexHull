/*
 * Ken Clarkson wrote this.  Copyright (c) 1996 by AT&T..
 * Permission to use, copy, modify, and distribute this software for any
 * purpose without fee is hereby granted, provided that this entire notice
 * is included in all copies of any software which is or includes a copy
 * or modification of this software and in all copies of the supporting
 * documentation for such software.
 * THIS SOFTWARE IS BEING PROVIDED "AS IS", WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTY.  IN PARTICULAR, NEITHER THE AUTHORS NOR AT&T MAKE ANY
 * REPRESENTATION OR WARRANTY OF ANY KIND CONCERNING THE MERCHANTABILITY
 * OF THIS SOFTWARE OR ITS FITNESS FOR ANY PARTICULAR PURPOSE.
 */


/*
 * two-dimensional convex hull
 * read points from stdin,
 *      one point per line, as two numbers separated by whitespace
 * on stdout, points on convex hull in order around hull, given
 *      by their numbers in input order
 * the results should be "robust", and not return a wildly wrong hull,
 *	despite using floating point
 * works in O(n log n); I think a bit faster than Graham scan;
 * 	somewhat like Procedure 8.2 in Edelsbrunner's "Algorithms in Combinatorial
 *	Geometry".
 */


#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <time.h>

typedef double coord;
char input_format[] = "%lf%lf";

#define N 1000000

coord points[N][2], *P[N+1]; /* an extra position is used */

int read_points(void) {
	int n = 0;
	char buf[100];
	while (fgets(buf, sizeof(buf), stdin)) {
	  if (2==sscanf(buf, input_format, &points[n][0], &points[n][1])) {
	    P[n] = points[n];
	    assert(++n <= N);
	  }
	}
	return n;
}

void print_hull(coord **P, int m) {
	int i;
	for (i=0; i<m; i++) {
	  printf("%f %f\n", P[i][0], P[i][1]);
	}
	printf("\n");
}


int ccw(coord **P, int i, int j, int k) {
	coord	a = P[i][0] - P[j][0],
		b = P[i][1] - P[j][1],
		c = P[k][0] - P[j][0],
		d = P[k][1] - P[j][1];
	return a*d - b*c <= 0;	   /* true if points i, j, k counterclockwise */
}


#define CMPM(c,A,B) \
	v = (*(coord**)A)[c] - (*(coord**)B)[c];\
	if (v>0) return 1;\
	if (v<0) return -1;

int cmpl(const void *a, const void *b) {
	double v; 
	CMPM(0,a,b);
	CMPM(1,b,a);
	return 0;
}

int cmph(const void *a, const void *b) {return cmpl(b,a);}


int make_chain(coord** V, int n, int (*cmp)(const void*, const void*)) {
	int i, j, s = 1;
	coord* t;

	qsort(V, n, sizeof(coord*), cmp);
	for (i=2; i<n; i++) {
		for (j=s; j>=1 && ccw(V, i, j, j-1); j--){}
		s = j+1;
		t = V[s]; V[s] = V[i]; V[i] = t;
	}
	return s;
}

int ch2d(coord **P, int n)  {
	int u = make_chain(P, n, cmpl);		/* make lower hull */
	if (!n) return 0;
	P[n] = P[0];
	return u+make_chain(P+u, n-u+1, cmph);	/* make upper hull */
}


int main(int argc, char** argv) {
  clock_t start, stop;
  int n, h;

  n = read_points();
  start = clock();
  h = ch2d(P, n);
  stop = clock();
  printf("%f\n", (double)(stop-start)/CLOCKS_PER_SEC);
  return 0;
}
