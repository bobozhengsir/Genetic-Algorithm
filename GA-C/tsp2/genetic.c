#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include <math.h>
#include <float.h>
//#define NDEBUG
#include <assert.h>
#include "genetic.h"
//#include "mt19937ar.h"


int main(int argc, char const *argv[])
{
	int num;
	//char bestRoute[2000];
	//unsigned long init[4]={0x123, 0x234, 0x345, 0x456}, length=4;
    //init_by_array(init, length);
    //init_genrand(time(NULL));
	//Route *r;
	Node *no;
	Map *m;
	Status st1, st2, st3;
	List ls2;
	List ls = InitEdges();
	ls->next = InitEdges();
	ls->next->data = 11;
	ls->size = 1;
	ls->next->next = InitEdges();
	ls->next->next->data = 12;
	ls->size = 2;
	EdgesAddTail(&ls, 13);

	ls2 = InitEdges();
	EdgesAddTail(&ls2, 12);
	EdgesAddTail(&ls2, 14);
	EdgesAddTail(&ls2, -11);

	st1 = EdgesContains(ls, 12);
	st2 = EdgesContains(ls, 13);
	st3 = EdgesContains(ls, 0);

	m = (Map *)malloc(sizeof(Map)*2);
	m->key = 0;
	m->edge = ls;
	(m+1)->key = 1;
	(m+1)->edge = ls2;

	ListRemove(m,2, 12);
	ListRemove(m,2, 11);
	ListRemove(m,2, 13);
	//EdgesRemove(&ls, 13);
	//no = EdgesnNode(ls, 3);

	LoadData("cities.in");
	
	//printf("%f\n",CalcRouteLength(data));
	//r = NearestNeighborTour(&data[1]);
	//CreatePopulation();
	//evolve();
	solve();
	if(validRoute(routes,1)==FALSE)
		printf("it's wrong!\n");
	printLocIds(routes);
	//printf("%s\n",bestRoute);
	scanf("%d",&num);
	return 0;
}

int LoadData(char const *path)
{
	int i = 0;
	int j;
	char line[100];
	char buffTemp[5][10];
    char split[] = " ";
	char *token;
	FILE *fp;
	fp = fopen(path, "r");
	if (fp == NULL)
	{
		perror(path);
		exit(EXIT_FAILURE);
	}
	while(!feof(fp) && i<NUMCITIES && fgets(line,sizeof(line),fp))
	{	
		j = 0;
		token = (char *)strtok(line, split);
		while(token != NULL)
		{
			strcpy(buffTemp[j], token);
			++j;
			token = (char *)strtok(NULL, split);
		}
		strcpy(data[i].name, buffTemp[0]); //printf("%s\n", data[i].name);
		data[i].id = i; //printf("%d\n", i);
		data[i].x = strtod(buffTemp[1], NULL); //printf("%f\n", data[i].x);
		data[i].y = strtod(buffTemp[2], NULL); //printf("%f\n", data[i].y);
		++i; 
	}
	return i;
}

void CreateMartix(void)
{
	int i,j;
	double distance;
	City city1,city2;// = (City *)malloc(sizeof(City));
	//City *city2 = (City *)malloc(sizeof(City));
	for (i = 0; i < NUMCITIES - 1; ++i)
	{
		martix[i][i] = 0.0;
		city1 = data[i];
		for (j = i + 1; j < NUMCITIES; ++j)
		{
			city2 = data[j];
			distance = sqrt(pow((city1.x - city2.x), 2) + pow((city1.y - city2.y), 2));
			martix[i][j] = distance;
			martix[j][i] = distance;
		}
	}
	martix[NUMCITIES-1][NUMCITIES-1] = 0.0;

}

double CalcRouteLength(City const *c)
{
	double distance = 0.0;
	int i, id0, id1, id2;
	id1 = c->id;
	id0 = id1;
	for (i = 0; i < NUMCITIES; ++i)
	{
		id2 = (c++)->id;
		distance += martix[id1][id2];
		id1 = id2;
	}
	distance += martix[id1][id0];/*plus the distance between last city and first city*/
	return distance;
}

Status Search(int const *arr, int const id, int const n)
{
	assert(arr != NULL);
	int i;
	for (i = 0; i < n; ++i)
	{
		if(*arr++ == id)
			return TRUE;
	}
	return FALSE;
}

City *CitynCpy(City *dest,const City *src,int n)
{
	assert(dest!=NULL && src !=NULL);
	City * cp = dest;
	int i;
	for (i = 0; i < n; ++i)
	{
		*dest++ = *src++;
	}
	return cp;
} 

Route *RoutenCpy(Route *dest,const Route *src,int n)
{
	assert(dest!=NULL && src !=NULL);
	Route * cp = dest;
	int i;
	for (i = 0; i < n; ++i)
	{
		dest[i] = src[i];
	}
	//while((*dest++ = *src++)
	return cp;
} 

/*
**as a city and calc the nearest Neighbor tour 
**return a Route
*/
Route *NearestNeighborTour(City const *c)
{
	int i,j;
	int id;
	int index;
	double minDist, val;
	City cityList[NUMCITIES];
	int  citiesVisited[NUMCITIES];
	Route *r;
	//City *temp;// = (City *)malloc(sizeof(City));
	//temp = c;
	City temp;
	temp = *c;
	for (i = 0; i < NUMCITIES; ++i)
	{
		cityList[i] = temp;
		id = temp.id;
		//citiesVisited = (int *)malloc(sizeof(int));
		citiesVisited[i] = id;
		minDist = DBL_MAX; //DBL_MAX defined in float.h
		index = 0;
		for (j = 0; j < NUMCITIES; ++j)
		{
			val = martix[id][j];
			if (!Search(citiesVisited,j,i+1) && val < minDist)
			{
				minDist = val;
				index = j;
			}
		}
		temp = data[index];//data + index;
	}
	//free(temp);
	r = (Route *)malloc(sizeof(Route));
	if (r == NULL)
	{
		printf("malloc error\n");
		exit(EXIT_FAILURE);
	}
	CitynCpy(r->cities, cityList, NUMCITIES);
	r->length = CalcRouteLength(cityList);
	return r;
}

City *ShuffleArray(City *dest,const City *src, int n)//, unsigned int seed
{
	assert(dest != NULL && src !=NULL);
	int i,j, t=NUMCITIES;
	City temp;
	City *c = dest;
	CitynCpy(dest, src, n);
	//srand(time(NULL)+seed);
	while(t--)
	{
		do{
			i=rand()%n;//printf("i%d\n", i);//i = genrand_int32()%n;
			j=rand()%n;//printf("j%d\n", j);//j = genrand_int32()%n;
		}while(i==j);
		temp = *(dest+i);
		*(dest+i) = *(dest+j);
		*(dest+j) = temp;
	}
	return c;
}

/*
**array sorted by ascending
*/
int CmpArrayASC(const void *p1, const void *p2)
{
	Route *m = (Route *)p1;
	Route *n = (Route *)p2;
	if (m->length > n->length)
		return 1;
	else if(m->length < n->length)
		return -1;
	else
		return 0;
}
/*
**array sorted by desending
*/
int CmpArrayDES(const void *p1, const void *p2)
{
	Route *m = (Route *)p1;
	Route *n = (Route *)p2;
	if (m->length > n->length)
		return -1;
	else if(m->length < n->length)
		return 1;
	else
		return 0;
}


/*
**first create the initial population of randomized routes.
**second pick the fittest routes to form the evolving population.
*/
static void CreatePopulation()
{
	int i;
	unsigned int randint;
	Route *r;
	Route *init_routes;//[POPULATION_SIZE];
	City *cities;
	init_routes = (Route *)malloc((POPULATION_SIZE+NUMCITIES)*sizeof(Route));
	cities = (City *)malloc(sizeof(City)*NUMCITIES);
	if (init_routes == NULL || cities == NULL)
	{
		printf("malloc error\n");
		exit(EXIT_FAILURE);
	}
	CitynCpy(cities, data, NUMCITIES);
	srand((unsigned)time(NULL));
	for (i = 0; i < POPULATION_SIZE; ++i)
	{	
		//randint = rand();//genrand_int32();
		ShuffleArray((init_routes+i)->cities, cities, NUMCITIES);//, randint
		(init_routes + i)->length = CalcRouteLength((init_routes+i)->cities);
	}
	for (i = POPULATION_SIZE; i < POPULATION_SIZE + NUMCITIES; ++i)
	{
		*(init_routes + i) = *(NearestNeighborTour(&data[i-POPULATION_SIZE])); 
	}
	qsort(init_routes, POPULATION_SIZE+NUMCITIES, sizeof(Route), CmpArrayASC);
	routes = (Route *)malloc(sizeof(Route)*EVOLVING_POPULATION_SIZE);
	if (routes == NULL)
	{
		printf("Out of memory\n");
		exit(EXIT_FAILURE);
	}
	RoutenCpy(routes, init_routes, EVOLVING_POPULATION_SIZE);
	//printf("%f\n", routes->length);
	free(init_routes);
	free(cities);
}
void Crossover(Route *children, Route *parents)
{
	assert(validRoute(parents,2)==TRUE);
	int i, j, m, n;
	int x, y, tmp, tmpid;
	Route *dad = parents;
	Route *mom = parents + 1;
	Route *child1 = children;
	//Route *child2 = children + 1;
	do{
		x = rand()%NUMCITIES;//genrand_int32()%NUMCITIES;
		y = rand()%NUMCITIES;//genrand_int32()%NUMCITIES;
		if(x > y)
		{
			tmp = x;
			x = y;
			y = tmp;
		}
	}while(x==y);
	for (i = 0; i < NUMCITIES; ++i)
	{
		if (i<x || i>y)
		{
			child1->cities[i] = dad->cities[i];
			valid1:
				for (m = x; m <= y; ++m)
				{
					if ((child1->cities+i)->id == (mom->cities+m)->id)   
					{
						child1->cities[i] = dad->cities[m];
						goto valid1;	
					}
				}
			/*
			child2->cities[i] = mom->cities[i];
			valid2:
				for (m = x; m <= y; ++m)
				{
					if ((child2->cities + i)->id == (dad->cities + m)->id)   
					{
						*(child2->cities + i) = *(mom->cities + m);
						goto valid2;
					}
				}
			*/
		}
		else
		{
			*(child1->cities + i) = *(mom->cities + i);	
			//*(child2->cities + i) = *(dad->cities + i);
		}	
	}
	child1->length = CalcRouteLength(child1->cities);
	//child2->length = CalcRouteLength(child2->cities);
	//printLocIds(child1);
	assert(validRoute(child1,1)==TRUE);
	//assert(validRoute(child2,1)==TRUE);
}

/*
**Randomly swap two citites in the route
*/
void Mutate(Route *chromosome)
{
	int c1, c2;
	City tmp;
	//srand((unsigned)time(NULL));
	do{
		c1 = rand()%NUMCITIES;//genrand_int32()%NUMCITIES;
		c2 = rand()%NUMCITIES;//genrand_int32()%NUMCITIES;
	}while(c1==c2);
	//printf("%d&&%d\n",c1,c2);
	tmp = chromosome->cities[c1];
	chromosome->cities[c1] = chromosome->cities[c2];
	chromosome->cities[c2] = tmp;
	chromosome->length = CalcRouteLength(chromosome->cities);
	assert(validRoute(chromosome,1)==TRUE);
}


void evolve()
{
	int x, i, index;
	int generation_sizes;
	Route *children;
	Route *possibleParents;
	generation_sizes = (int)((1-ELITISM_PCT) * EVOLVING_POPULATION_SIZE); 
	routes = (Route *)realloc(routes,(EVOLVING_POPULATION_SIZE + generation_sizes) * sizeof(Route));
	srand((unsigned)time(NULL));//
	for (x = 0; x < generation_sizes; ++x)
	{
		possibleParents = (Route *)malloc(sizeof(Route)*TOURNAMENT_SIZE);
		for (i = 0; i < TOURNAMENT_SIZE; ++i)
		{
			index = rand() % EVOLVING_POPULATION_SIZE;//genrand_int32()%(EVOLVING_POPULATION_SIZE);
			//printf("%d\n",index);
			*(possibleParents+i) = *(routes+index); 
		}
		//printf("%f\n", possibleParents->length);
		qsort(possibleParents, TOURNAMENT_SIZE, sizeof(Route), CmpArrayASC);
		possibleParents =(Route *)realloc(possibleParents,sizeof(Route)*2);
		children = (Route *)calloc(1,sizeof(Route));
		if ( (rand() / (double)(RAND_MAX))<= CROSSOVER_RATE )//genrand_real2() <= CROSSOVER_RATE)
		{
			//Crossover(children, possibleParents);
			ERCrossover(children, possibleParents);
		}
		else
			RoutenCpy(children, possibleParents, 1);
		
		if ( (rand() / (double)(RAND_MAX)) > MUTATION_RATE )//genrand_real2() > MUTATION_RATE)
		{
			Mutate(children);
		}
		/*if ( (rand() / (double)(RAND_MAX)) > MUTATION_RATE )//genrand_real2() > MUTATION_RATE)
		{
			Mutate(children + 1);
		}*/
		*(routes + EVOLVING_POPULATION_SIZE + x) = *children;
		//*(routes + EVOLVING_POPULATION_SIZE + 2*x + 1) = *(children+1);
		free(possibleParents);
		free(children);
	}
	qsort(routes, EVOLVING_POPULATION_SIZE + generation_sizes, sizeof(Route), CmpArrayASC);
	routes = (Route *)realloc(routes, EVOLVING_POPULATION_SIZE*sizeof(Route));
	assert(validRoute(routes,EVOLVING_POPULATION_SIZE)==TRUE);
}

void solve(void)
{
	int i;
	double dis;
	CreateMartix();
	CreatePopulation();
	dis = routes->length;
	printf("%f\n",dis);
	for (i = 0; i < NUMBER_OF_GENERATIONS; ++i)
	{
		evolve();
		if(dis > routes->length)
			dis = routes->length;
	}
	printf("%f\n",dis);
}

void printLocIds(const Route *r)
{
	if(r==NULL)
	{
		printf("there is no route");return;
		//exit(EXIT_FAILURE);
	}
	int i;
	printf("***********************\n");
	printf("%f\n", r->length);
	for(i=0;i<NUMCITIES;i++)
	{
		printf("%s->", (r->cities+i)->name);
		
	}
	printf("%s\n",r->cities->name);
	printf("EOF\n");
	printf("***********************\n");
}
Status validRoute(const Route *r,const int n)
{
	int id[NUMCITIES];
	int i,j,k, tmp;
	for(k=0;k<n;++k)
	{
		for(i=0;i<NUMCITIES;++i)
		{
			tmp = (r+k)->cities[i].id;
			id[i]= tmp;
			j = 0;
			while(j<i)
			{
				if(tmp==id[j])
					{printf("%d",tmp);return FALSE;}
				j++;
			}
		}
	}
	return TRUE;
}



/*
**Change Crossover to ERCrossOver
**Enhanced Edge Recombination (ER) Algorithm.
*/
void ERCrossover(Route *child, Route *parents)
{
	assert(validRoute(parents,2)==TRUE);
	assert(child != NULL);
	int i, j, dadIndex, momIndex, cityId;
	int tmprand;
	int dadInitialCityConnctions, momInitialCityConnctions;
	int nextCityId;
	//City childCityArray[NUMCITIES];
	//City *dadArray, *momArray;
	City c;
	City currentCity;
	List unvisitedCityIds; 
	Map *edgeMap;
	edgeMap =(Map *)calloc(NUMCITIES, sizeof(Map));
	Route *dad;
	Route *mom;
	dad	= parents;
	mom = parents + 1;
	// Create the edge map.
	// <CityId : List of neighboring CityId's in mom and dad>.
	for (dadIndex = 0; dadIndex < NUMCITIES; ++dadIndex) 
	{
		c = dad->cities[dadIndex];//dadArray[dadIndex];
		cityId = c.id;
		// Get location of current city in dad--in mom.
		momIndex = 0;
		for (j = 0; j < NUMCITIES; ++j)
		{
			if (cityId == mom->cities[j].id) 
			{
				momIndex = j;
				break;
			}
		}
		(edgeMap+dadIndex)->key = cityId;
		(edgeMap+dadIndex)->edge = getEdges(dad->cities, mom->cities, dadIndex, momIndex);
	}
	//City[] childCityArray = new City[NUMCITIES];
	//ArrayList<Integer> unvisitedCityIds = new ArrayList<Integer>(NUMCITIES);
	unvisitedCityIds = InitEdges();
	for (i = 0; i < NUMCITIES; ++i)
		EdgesAddTail(&unvisitedCityIds, i);

	// Pick start city...
	dadInitialCityConnctions = GetMap(edgeMap, NUMCITIES, dad->cities[0].id)->size;
	momInitialCityConnctions = GetMap(edgeMap, NUMCITIES, mom->cities[0].id)->size;
	if (dadInitialCityConnctions >= momInitialCityConnctions)
		currentCity = dad->cities[0];
	else
		currentCity = mom->cities[0];
		
	child->cities[0] = currentCity;//childCityArray[0] = currentCity;
	
	i = 1;
	EdgesRemove(&unvisitedCityIds, currentCity.id);
	ListRemove(edgeMap, NUMCITIES, currentCity.id);

	while (unvisitedCityIds->size > 0) 
	{
			if (GetMap(edgeMap, NUMCITIES, currentCity.id)->size > 0) // Step 4.
				currentCity = PickNextCity(edgeMap, currentCity.id);
			else 
			{ // Step 5.	
				if ( unvisitedCityIds->size == 1 )
					nextCityId = unvisitedCityIds->next->data;
				else
				{
					tmprand = rand() % unvisitedCityIds->size;
					///printf("%d\n", tmprand);
					nextCityId = EdgesnNode(unvisitedCityIds, tmprand)->data;
				}
				currentCity = data[nextCityId];
			}
			child->cities[i] = currentCity;//childCityArray[i] = currentCity;
			++i;
			
			//printf("%d\n", unvisitedCityIds->size);
			assert(unvisitedCityIds->size > 0);

			EdgesRemove(&unvisitedCityIds, currentCity.id);
			ListRemove(edgeMap, NUMCITIES, currentCity.id);
	}

	//CitynCpy(child->cities, childCityArray, NUMCITIES);
	child->length = CalcRouteLength(child->cities);
	//printf("%f\n", child->length);
	assert(validRoute(child,1)==TRUE);
	free(edgeMap);
	//return child;
}

List InitEdges(void)
{
	List p;
	p = (List)malloc(sizeof(Node));
	if(p==NULL)	
	{
		printf("memory malloc failed£¡\n");
		exit(EXIT_FAILURE);
	}
	p->size = 0;
	p->next = NULL;
	//p->prev = NULL;
	return p;
}

Map *InitMap(int n)
{
	Map *p;
	p = (Map *)malloc(sizeof(Map)*n);
	while(p++!=NULL)
		p->edge = NULL;
	return p;
}
void EdgesFree(List *L)
{
	//assert(L!=NULL);
	List p, next;
	p = *L;
	next=p->next;
	while(next!=NULL)
	{
		free(next);
		p->size--;
		next = next->next;
	}
	next = NULL;
	//free(next);
	assert((*L)->size == 0);
	assert((*L)->next == NULL);
}

void EdgesPrint(const Node *L)
{
	int i;
	Node p;
	p = *L;
	for (i = 0; i < L->size && p.next; ++i)
	{
		printf("edges%d->", p.next->data);
		p = *(p.next);
	}
	printf("%d\n", p.data);
	printf("\n");
}
void EdgesClear(Node *L)
{
	assert(L!=NULL);
	Node *p, *next;
	p = L;
	next=p->next;
	do
	{
		next=NULL;
	}while(next!=NULL);
}
/*
**if L is the tail pointer of Edges , O(1)
**else O(n)
*/
Status EdgesAddTail(List *L, int e)
{
	List p, q;
	p = *L;
	while(p->next != NULL)//find the tail of the Edges List
		p = p->next;
	assert(p->next == NULL);		
	q = (List)malloc(sizeof(Node));
	if(q==NULL)	
	{
		printf("memory malloc failed£¡\n");
		exit(EXIT_FAILURE);
	}
	q->data = e;
	q->next = NULL;
	//q->prev = p;
	p->next = q;
	(*L)->size++;
	//EdgesPrint(L);
	return TRUE; 
}

Node *EdgesnNode(const Node * const L, int n)
{
	int i, s;
	Node p;
	p = *L;
	s = p.size;
	if(n >= s || s == 0)
		return NULL;
	i = 0;
	while(i<n)
	{	
		p = *(p.next);
		i++; 
	}
	return p.next;
}
/*
**
*/
Status EdgesContains(const Node *L, int e)
{
	int i, y;
	Node p;
	p = *L;
	//printf("psize %d\n", p.size);
	if(L->size <= 0)//p->size == 0
		return FALSE;
	y = p.next->data;
	for (i = 0; i < L->size; ++i)
	{
		//printf("%d\n", p.data);
		//printf("%d\n", y);
		if (y == e)
		{
			return TRUE;
		}
		if (p.next == NULL)
		{
			if (p.data == e)
			{
				return TRUE;
			}
			return FALSE;
		}
		p = *(p.next);
		y = p.data;
	}
	if (y == e)
	{
		return TRUE;
	}
	return FALSE;
}

Node *GetMap(const Map *L,int n, int k)
{
	int i;
	//Map p;
	//p = *L;
	//n = NUMCITIES;
	for(i = 0; i < n; i++)
	{
		if((L+i)->key == k)
			return (L+i)->edge;
	}
	return NULL;
}

Status EdgesRemove(List *L, int e)
{
	int y;
	List p,q,prev;
	p = *L;
	if(L == NULL || p == NULL || p->next == NULL)
		return FALSE;
	while(p->next != NULL)
	{	prev = p;	
		p = p->next;
		y = p->data;
		if(y == e)
		{
			q = p;
			prev->next = q->next;
			free(q);
			(*L)->size--;
			return TRUE;
		}
	}
	/*
	if(p->data == e)
	{
			q = p;
			prev->next = q->next;
			free(q);
			(*L)->size--;
			return TRUE;
	}*/
	return FALSE;
}

Status ListRemove(Map *L,int n, int e)
{
	int i;
	Map *p;
	p = L;
	for(i = 0; i < n; i++)
	{
		//if ((p+i)->edge != NULL)
		//{
			EdgesRemove(&((p+i)->edge), e);
			EdgesRemove(&((p+i)->edge), -1*e);
		//}
	}
	return TRUE;
}

List getEdges(City dadArray[], City momArray[], int dadIndex, int momIndex)
{
	Status st;
	int i, id;
	int front, back;
	int frontBack[2];
	List edges;
	edges = InitEdges();
	// Get dad's edges.
	front = (dadIndex + 1) % NUMCITIES;
	back = (NUMCITIES + dadIndex - 1) % NUMCITIES;
	//printf("%d\n", dadArray[back].id);
	EdgesAddTail(&edges, dadArray[back].id);
	//printf("%d\n", edges->next->data);
	EdgesAddTail(&edges, dadArray[front].id);
	
	// Get mom's edges.
	front = (momIndex + 1) % NUMCITIES;
	back = (NUMCITIES + momIndex - 1) % NUMCITIES;
	// If the city is already in the edges list by dad,
	// flag it with a negative sign...
	frontBack[0] = front;
	frontBack[1] = back ;
	for (i = 0; i < 2; ++i) {
		id = momArray[frontBack[i]].id;
		if (EdgesContains(edges, id) == TRUE)
		{
			EdgesRemove(&edges, id);
			//printf("boolean%d\n", st);
			EdgesAddTail(&edges, -1*id);
		} else
			// ...otherwise add as usual.
			EdgesAddTail(&edges, id);
	}
	//EdgesPrint(edges);
	return edges;
}

static City PickNextCity(Map *L, int k) 
{
	int i, tmpdata, sz;
	int negs;
	int index;
	int numMinConnections;
	int numConnections; 
	City nextCity;
	Node *q;
	Map *p;
	Node *citiesToConsider;
	List possibleChoices;
	Map *e;
	//InitEdges(citiesToConsider);
	p = L;
	citiesToConsider = GetMap(L, NUMCITIES, k);
	//printf("citiesToConsider->size::%d\n", citiesToConsider->size);

	// 3 Possibilities:
	// 4 cities to consider: all positive.
	// 3 cities to consider: one of them could be negative.
	// 2 cities to consider: both could be negative.

	if (citiesToConsider->size == 3) 
	{
		// Pick the negative one if it exists.
		q = citiesToConsider->next;
		for (i = 0; i < 3; ++i)
		{
			tmpdata = q->data;
			if (tmpdata < 0) {
				nextCity = data[-1 * tmpdata];
				return nextCity;
			}
			q = q->next;
		}
	}
	else if (citiesToConsider->size == 1) 
	{
		nextCity = data[abs(citiesToConsider->next->data)];
		return nextCity;
	}

	// If one of the 2 is negative, pick it, otherwise either will do.
	if (citiesToConsider->size == 2) 
	{
		negs = 0;
		index = 0;
		if (citiesToConsider->next->data < 0) 
		{
			++negs;
			index = 0;
		}
		if (citiesToConsider->next->next->data < 0) 
		{
			++negs;
			index = 1;
		}
		if (negs == 1) 
		{
			nextCity = data[abs(EdgesnNode(citiesToConsider, index)->data)];
			return nextCity;
		}
	}

	// If not picking a negative, or if all are negative,
	// pick the one with the least connections.
	numMinConnections = INT_MAX;
	possibleChoices = InitEdges();
	//printf("p0: %d\n", possibleChoices->size);
	i = 0;
	while(i<NUMCITIES)
	{
		e = L+i;
		//printf("e->key%d\n", e->key);
		// City in edge map listing could be positive or negative.
		if (EdgesContains(citiesToConsider, e->key) == TRUE || EdgesContains(citiesToConsider, -1 * e->key) == TRUE) 
		{
			numConnections = e->edge->size;
			if (numConnections < numMinConnections) 
			{
				numMinConnections = numConnections;
				//EdgesFree(&possibleChoices);
				//printf("pf: %d\n", possibleChoices->size);
				possibleChoices = InitEdges();
				EdgesAddTail(&possibleChoices, e->key);
				//printf("pa: %d\n", possibleChoices->size);
			} 
			else if (e->edge->size == numMinConnections)
			{
				EdgesAddTail(&possibleChoices, e->key);
				//printf("p0: %d\n", possibleChoices->size);
			}
		}
		
		i++;
	}
	// If there is a tie for the least connections, randomly choose.
	if (possibleChoices->size == 1)
		nextCity = data[possibleChoices->next->data];
	else
		{
			sz = possibleChoices->size;
			//printf("sz1 :%d\n", sz);
			if(sz <= 0)
				exit(EXIT_FAILURE);
			sz = rand() % sz;
			//printf("sz :%d\n", sz);
			nextCity = data[abs(EdgesnNode(possibleChoices, sz )->data)];
		}
	free(possibleChoices);
	return nextCity;
}