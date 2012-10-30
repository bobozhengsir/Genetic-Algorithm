#!/usr/env python

import random
import sys
from math import sqrt 

class City(object):
	"""docstring for City"""
	def __init__(self, name, cid, x, y):
		self.name = name
		self.cid = cid
		self.x = x
		self.y = y

class Route(object):
	"""docstring for Route"""
	def __init__(self, route, length):
		self.route = route 
		self.length = length

	def __str__(self):
		cityNames = []
		for c in self.route:
			cityNames.append(c.name)
		strCityNames = ",".join(cityNames)
		return strCityNames

	def getCityIds(self):
		cityIds = [c.cid for c in self.route]
		return cityIds

	def __cmp__(self, other):
		otherLength = other.length
		if self.length > otherLength:
			return 1
		elif self.length < otherLength:
			return -1
		else:
			return 0

class TspGA(object):
	"""docstring for TspGA"""
	def __init__(self, data=None):
		#
		#
		self.POPULATION_SIZE = 1000000;
		self.EVOLVING_POPULATION_SIZE = 500;
		self.ELITISM_PCT = 0.1;
		self.NUMBER_OF_GENERATIONS = 400;
		self.MUTATION_RATE = 0.4;
		self.CROSSOVER_RATE = 0.9;
		self.TOURNAMENT_SIZE = 10;

		self.numCities = 0
		self.martix = []
		self.directory = {}
		self.routes = []

		self.crossover = self.PMXCrossover
		self.mutate = self.mutateRandomCitySwap

		if type(data).__name__ == 'list':
			self.data = data
			self.numCities = len(self.data)

	def loadCityData(self, path=''):
		"""from the path load all cities data"""
		self.data = []
		self.directory = {}
		try:
			i = 0
			f = open(path)
			for line in f:
				params = line.split(',')
				if len(params) == 3:	
					name = params[0].strip()
					cid = i
					x = float(params[1].strip())
					y = float(params[2].strip())
					c = City(name, cid, x, y)
					self.data.append(c)
					self.directory[cid] = c
					i += 1
			self.numCities = len(self.data)
			self.createMartix()
		except Exception, e:
			raise e
		finally:
			f.close()
		self.numCities = len(self.data)
		self.createMartix()

	def createMartix(self):
		"""create martix about cities' distances"""
		self.martix = [[0.0 for i in xrange(self.numCities)] for j in xrange(self.numCities)]
		for i in xrange(self.numCities - 1):
			self.martix[i][i] = 0.0
			city1 = self.data[i]
			for j in xrange(i + 1, self.numCities):
				city2 = self.data[j]
				delta_x = float(city1.x - city2.x)
				delta_y = float(city1.y - city2.y)
				distance = round(sqrt(delta_x**2 + delta_y**2))
				self.martix[i][j] = distance
				self.martix[j][i] = distance
		self.martix[self.numCities - 1][self.numCities - 1] = 0.0

	def calcRouteLength(self, cities=[]):
		"""calc route length"""
		distance = 0.0
		cid1 = cities[0].cid
		for i in xrange(1, self.numCities):
			cid2 = cities[i].cid
			distance += self.martix[cid1][cid2]
			cid1 = cid2
		# plus the distance between last city and first city
		distance += self.martix[cid1][cities[0].cid]
		return distance

	def nearestNeighborTour(self, city):
		"""as a city and calc the nearest Neighbor tour 
		return a Route"""
		if type(city).__name__ == 'City':
			cityList = []
			citiesVisited = []
			for i in xrange(self.numCities):
				cityList.append(city)
				cid = city.cid
				citiesVisited.append(cid)
				minDist = 10E10 # select a big float number
				index = 0
				for x in xrange(self.numCities):
					val = self.martix[cid][x]
					if x not in citiesVisited and val < minDist:
						minDist = val
						index = x
				city = self.directory[index]
			r = Route(cityList, self.calcRouteLength(cityList))
			return r

	def createPopulation(self):
		"""first create the initial population of randomized routes.
		second pick the fittest routes to form the evolving population."""
		init_routes = []
		cities = self.data[:]
		for i in xrange(self.POPULATION_SIZE):
			random.shuffle(cities)
			r = Route(cities, self.calcRouteLength(cities))
			init_routes.append(r)
		for x in xrange(self.numCities):
			init_routes.append(self.nearestNeighborTour(self.data[x]))
		sort_init_routes = sorted(init_routes, key=lambda x: x.length)
		self.routes = sort_init_routes[:self.EVOLVING_POPULATION_SIZE]

	def solve(self):
		self.createPopulation()
		for i in xrange(self.NUMBER_OF_GENERATIONS):
			self.evolve()

	def evolve(self):
		"""evolve operator"""
		generation_sizes = int(round((1 - self.ELITISM_PCT) * self.EVOLVING_POPULATION_SIZE))
		newChildren = []
		for x in xrange(generation_sizes):
			# Randomly select a set of routes to vie for the right to parent.
			possibleParents = []
			for i in xrange(self.TOURNAMENT_SIZE):
				index = random.randrange(len(self.routes))
				possibleParents.append(self.routes[index])
			possibleParents = sorted(possibleParents, key=lambda p: p.length)
			dad = possibleParents[0]
			mom = possibleParents[1]
			children = []
			if random.random() > self.CROSSOVER_RATE:
				children = self.crossover(dad, mom)
			else:
				children = [dad, mom]
			if random.random() <= self.MUTATION_RATE:
				children = map(self.mutate, children)
			newChildren.extend(children)
		newChildren.extend(self.routes[:self.EVOLVING_POPULATION_SIZE - generation_sizes])
		newChildren = sorted(newChildren, key= lambda new : new.length)
		self.routes = []
		self.routes = newChildren[:self.EVOLVING_POPULATION_SIZE]
		

	def PMXCrossover(self, crossDad, crossMom):
		"""partially mapped crossover"""
		childCityIds1 = []
		childCityIds2 = []
		x = random.randrange(self.numCities)
		y = random.randrange(self.numCities)
		if x > y:
			x,y = y,x
		dadCityIds = crossDad.getCityIds()
		#dadParts = []
		dadParts = dadCityIds[x:y+1]
		momCityIds = crossMom.getCityIds()
		#momParts =[]
		momParts = momCityIds[x:y+1]
		dadPartMap = dict(zip(dadParts, momParts))
		momPartMap = dict(zip(momParts, dadParts))
		# crossover and create one child
		for i in xrange(x):
			if dadCityIds[i] in momParts:
				childCityIds1.append(momPartMap[dadCityIds[i]])
			else:
				childCityIds1.append(dadCityIds[i])
		childCityIds1.extend(momParts)
		for j in xrange(y+1, self.numCities):
			if dadCityIds[j] in momParts:
				childCityIds1.append(momPartMap[dadCityIds[j]])
			else:
				childCityIds1.append(dadCityIds[j])
		# create two child
		for i in xrange(x):
			if momCityIds[i] in dadParts:
				childCityIds2.append(dadPartMap[momCityIds[i]])
			else:
				childCityIds2.append(momCityIds[i])
		childCityIds2.extend(dadParts)
		for j in xrange(y+1, self.numCities):
			if momCityIds[j] in dadParts:
				childCityIds2.append(dadPartMap[momCityIds[j]])
			else:
				childCityIds2.append(momCityIds[j])
		childCities1 = [self.directory[m] for m in childCityIds1]
		child1 = Route(childCities1, self.calcRouteLength(childCities1))
		childCities2 = [self.directory[n] for n in childCityIds2]
		child2 = Route(childCities2, self.calcRouteLength(childCities2))
		return [child1, child2]

	def mutateRandomCitySwap(self, chromosome):
		"""Randomly swap two cities in the route."""
		if type(chromosome).__name__ == 'Route':
			cityArray = chromosome.route
			cityArray = cityArray[:self.numCities]
			c1 = random.randrange(self.numCities)
			c2 = random.randrange(self.numCities)
			# cityArray[c1], cityArray[c2] = cityArray[c2], cityArray[c1]
			temp = cityArray[c1]
			cityArray[c1] = cityArray[c2]
			cityArray[c2] = temp
			newChromosome = Route(cityArray, self.calcRouteLength(cityArray))
			return newChromosome


def main():
	"""Run the algorithm a number of times and take the best result."""
	EVOLUTIONS = 5
	best = []
	tsp = TspGA()
	tsp.loadCityData(sys.argv[1])
	for x in xrange(EVOLUTIONS):	
		tsp.solve()
		tsp.routes.sort(key=lambda r : r.length)
		best.append(tsp.routes[0])
		print("Evolving %d :get the best length %f" % (x, tsp.routes[0].length))
	best = sorted(best, key=lambda l: l.length)
	print("EVOLUTIONS RESULTS:")
	for y in xrange(EVOLUTIONS):
		print(str(best[y]))
	print("The best result is %s " % str(best[0]))
	print("The min length is %f" % best[0].length)

if __name__ == '__main__':
		main()	
	

						

