#ifndef __GENETIC_H__
#define __GENETIC_H__

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <float.h>
#include <assert.h>

#define NUMCITIES 51
#define POPULATION_SIZE  100000
#define EVOLVING_POPULATION_SIZE  500
#define ELITISM_PCT  0.1
#define NUMBER_OF_GENERATIONS  400
#define MUTATION_RATE  0.05
#define CROSSOVER_RATE  0.9
#define TOURNAMENT_SIZE  10
#define TRUE 1
#define FALSE 0

typedef int Status;

typedef struct City
{
	/* data */
	char name[10];
	int id;
	double x;
	double y;
} City;

typedef struct Route
{
	/* data */
	double length;
	City cities[NUMCITIES];
} Route;

static City data[NUMCITIES];
static double martix[NUMCITIES][NUMCITIES];
static Route *routes;//[EVOLVING_POPULATION_SIZE];

int LoadData(char const *path);
void CreateMartix(void);
double CalcRouteLength(City const *c);
Route *NearestNeighborTour(City const *c);
Status Search(int const *arr, int const id, int const n);
City *CitynCpy(City *dest,const City *src,int n);
Route *RoutenCpy(Route *dest,const Route *src,int n);
City *ShuffleArray(City *dest,const City *src, int n);
void printLocIds(const Route *r);

#endif