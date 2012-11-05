#ifndef __GENETIC_H__
#define __GENETIC_H__



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
	char name[8];
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

typedef struct _Node
{
	int data;
	int size;
	struct _Node *next;
	//struct _Node *prev;
} Node;

typedef struct _Node *List;
//#define Edges Node *
typedef struct _Map
{
	int key;//unique,TODO: use hash
	//int size;
	List edge;
	//struct _Map *next;
} Map;
//typedef struct _Map *List;
//#define List Map *

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
static City PickNextCity(Map *L, int k);
List getEdges(City dadArray[], City momArray[], int dadIndex, int momIndex);
Status ListRemove(Map * L,int n, int e);
Status EdgesRemove(List *L, int e);
Node *GetMap(const Map * L,int n, int k);
Status EdgesContains(const Node *L, int e);
Node *EdgesnNode(const Node * const L,int n);
Status EdgesAddTail(List *L, int e);
void EdgesClear(Node * L);
void EdgesFree(List * L);
List InitEdges(void);
void EdgesPrint(const Node *L);
void ERCrossover(Route *child, Route *parents);
void solve(void);

#endif
