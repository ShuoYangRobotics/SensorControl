
f=open("1.csv")
string = f.read()
zaza = string.split('\n')
rice = [[]*12 for x in xrange(len(zaza))]

for i in xrange(len(zaza)):
    rice[i] = zaza[i].split(',')
