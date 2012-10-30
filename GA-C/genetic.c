#include "genetic.h"
//#include "mt19937ar.h"

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
	return i+1;
}

void CreateMartix(void)
{
	int i,j;
	double distance;
	City *city1 = (City *)malloc(sizeof(City));
	City *city2 = (City *)malloc(sizeof(City));
	for (i = 0; i < NUMCITIES - 1; ++i)
	{
		martix[i][i] = 0.0;
		*city1 = data[i];
		for (j = i + 1; j < NUMCITIES; ++j)
		{
			*city2 = data[j];
			distance = sqrt(pow((city1->x - city2->x), 2) + pow((city1->y - city2->y), 2));
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
	Route *child2 = children + 1;
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
		}
		else
		{
			*(child1->cities + i) = *(mom->cities + i);	
			*(child2->cities + i) = *(dad->cities + i);
		}	
	}
	child1->length = CalcRouteLength(child1->cities);
	child2->length = CalcRouteLength(child2->cities);
	//printLocIds(child1);
	assert(validRoute(child1,1)==TRUE);
	assert(validRoute(child2,1)==TRUE);
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
	routes = (Route *)realloc(routes,(EVOLVING_POPULATION_SIZE + generation_sizes*2) * sizeof(Route));
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
		children = (Route *)calloc(2,sizeof(Route));
		if ( (rand() / (double)(RAND_MAX))<= CROSSOVER_RATE )//genrand_real2() <= CROSSOVER_RATE)
		{
			Crossover(children, possibleParents);
		}
		else
			RoutenCpy(children, possibleParents, 2);
		free(possibleParents);
		if ( (rand() / (double)(RAND_MAX)) > MUTATION_RATE )//genrand_real2() > MUTATION_RATE)
		{
			Mutate(children);
		}
		if ( (rand() / (double)(RAND_MAX)) > MUTATION_RATE )//genrand_real2() > MUTATION_RATE)
		{
			Mutate(children + 1);
		}
		*(routes + EVOLVING_POPULATION_SIZE + 2*x) = *children;
		*(routes + EVOLVING_POPULATION_SIZE + 2*x + 1) = *(children+1);
		free(children);
	}
	qsort(routes, EVOLVING_POPULATION_SIZE + generation_sizes*2, sizeof(Route), CmpArrayASC);
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
int main(int argc, char const *argv[])
{
	int num;
	LoadData("cities.in");
	
	solve();
	if(validRoute(routes,1)==FALSE)
		printf("it's wrong!\n");
	printLocIds(routes);
	return 0;
}

