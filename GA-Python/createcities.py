import sys
import os
import random

NUMCITIES = 50
def main():
	outfile = sys.argv[1]
	if os.path.isfile(outfile):
		os.remove(outfile)
	os.mknod(outfile)
	cityLocs = []
	for i in xrange(10**6):
		x = str(random.randrange(101))
		y = str(random.randrange(101))
		if len(cityLocs) == NUMCITIES:
			break
		if (x,y) in cityLocs:
			continue
		else:
			cityLocs.append((x,y))
	f = open(outfile,'a')
	for j in xrange(NUMCITIES):
		name = "LOC%d" % j
		line = [] 
		line.append(name)
		line.extend(list(cityLocs[j]))
		#line.append(os.linesep)
		f.write(",".join(line))
		f.write(os.linesep)
	f.close()

if __name__ == '__main__':
	main()
