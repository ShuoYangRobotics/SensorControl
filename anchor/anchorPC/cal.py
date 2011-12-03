import sys
from math import sqrt

reference = -58.0
N = 3.8
REFLECTION_THRESHOLD = 7
DIRECT_THRESHOLD = 0.45
SIMILARITY_THRESHOLD = 0.9
NOISE = 0.43

#f=open("1.csv")
#string = f.read()
#zaza = string.split('\n')
#rice = [[]*12 for x in xrange(len(zaza))]

#for i in xrange(len(zaza)):
#    rice[i] = zaza[i].split(',')
rice = [
    ['0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0'],
    ['0.607', '0.466', '0.187', '0.033', '0', '0', '0', '0', '0', '0', '0', '0'],
    ['0.271', '0.374', '0.414', '0.202', '0.039', '0', '0', '0', '0', '0', '0', '0'], 
    ['0.033', '0.099', '0.303', '0.405', '0.212', '0.042', '0', '0', '0', '0', '0', '0'], 
    ['0', '0', '0.078', '0.282', '0.402', '0.218', '0.044', '0', '0', '0', '0', '0'], 
    ['0', '0', '0', '0.070', '0.272', '0.401', '0.222', '0.046', '0', '0', '0', '0'], 
    ['0', '0', '0', '0', '0.066', '0.266', '0.400', '0.225', '0.047', '0', '0', '0'], 
    ['0', '0', '0', '0', '0', '0.064', '0.262', '0.400', '0.227', '0.048', '0', '0'], 
    ['0', '0', '0', '0', '0', '0', '0.063', '0.259', '0.400', '0.229', '0.048', '0'], 
    ['0', '0', '0', '0', '0', '0', '0', '0.061', '0.257', '0.400', '0.230', '0.049'], 
    ['0', '0', '0', '0', '0', '0', '0', '0', '0.060', '0.255', '0.399', '0.231'], 
    ['0', '0', '0', '0', '0', '0', '0', '0', '0', '0.060', '0.254', '0.399'], 
    ['0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0.059', '0.253']
]

def getVector(distance,rssi):
    K = 10**((rssi-reference)/(10*N))
    for j in range(1,11):
        if (K - 1.0/(distance*j)) >= 0:
            p1 = 1.0/j
            p2 = (K - 1.0/(distance*j))/(1.0/(distance+1))
            if (p2/float(rice[int(distance)][int(distance)+1])>REFLECTION_THRESHOLD):
                continue
            return [p1/sqrt((p1*p1)+(p2*p2)),p2/sqrt((p1*p1)+(p2*p2))]

def generate(rssi):
    result =[]
    if (rssi>=0 or rssi<-100):
        return result
    K = 10**((rssi-reference)/(10*N))

    #print str(rssi)+", possible combination:"
    #print "(distance stack: "+str(K)+")"

    #print 1.0/K
    result.append(1.0/K)
    for i in range(10,101,4):
        for j in range(1,11):
            if (K - 1.0/(0.1*i*j)) >= 0:
                p1 = 1.0/j
                p2 = (K - 1.0/(0.1*i*j))/(1.0/(0.1*i+1))
                if (p2/float(rice[int(0.1*i)][int(0.1*i)+1])>REFLECTION_THRESHOLD):
                    break
                #print str(p1)+"x"+"1/"+str(0.1*i)+" + "+str(p2)+"x"+"1/"+str(0.1*i+1)
                if (i!=result[-1] and
                    p1>=DIRECT_THRESHOLD and
                    #((p1/sqrt((p1*p1)+(p2*p2))*vector[0]+p2/sqrt((p1*p1)+(p2*p2))*vector[1])\
                     #>=SIMILARITY_THRESHOLD) and
                    (abs(0.1*i-1.0/K) < 3)):
                        if (0.1*i - result[-1]>0.2):
                            result.append(0.1*i)
    #print ""
    return result

def calculatePosition(m1,m2,m3,m4):
    temp=[[] for x in xrange(4)]
    #haha = raw_input("Pleas input four number\n").split()
    haha = [m1,m2,m3,m4]
    print "Possible distance measurements:"
    for i in range(0,len(haha)):
        if (haha[i]<-85):
            haha[i] = -85
        temp[i] = generate(haha[i])
        for j in range(0,len(temp[i])):
            print "%2.1f"%temp[i][j],
        print "\n"

    pos = []
    pos.append(0)
    pos.append(0)
    counter = 0
    pool = []
    counter_in = 0
    counter_out = 0
    #print "Possible positions:"
    for i in range(0,len(temp[0]),3):
        for j in range(0,len(temp[1]),3):
            for k in range(0,len(temp[2]),3):
                for l in range(0,len(temp[3]),3):
                    #if ((abs(temp[0][i])+abs(temp[2][k]))<(4*sqrt(2)*NOISE) or \
                    #    (abs(temp[1][j])+abs(temp[3][l]))<(4*sqrt(2)*NOISE)or \
                    #    (abs(temp[0][i])+abs(temp[1][j]))<(4*NOISE) or \
                    #    (abs(temp[1][j])+abs(temp[2][k]))<(4*NOISE) or \
                    #    (abs(temp[2][k])+abs(temp[3][l]))<(4*NOISE) or \
                    #    (abs(temp[0][i])+abs(temp[3][l]))<(4*NOISE)):
                    #    continue
                    x = (temp[3][l]*temp[3][l]-temp[2][k]*temp[2][k]+4**2)/(2*4)
                    y = (temp[3][l]*temp[3][l]-temp[0][i]*temp[0][i]+4**2)/(2*4)
                    #print "[%2.1f,%2.1f]"%(x,y)
                    if (pool.__contains__((x,y)) == False):
                        pool.append((x,y))
                        counter = counter + 1
                        pos[0]= (pos[0] + x)
                        pos[1]= (pos[1] + y)
    #print "Average:"
    #print [pos[0]/float(counter),pos[1]/float(counter)]
    for item in pool:
        #print "[%2.1f,%2.1f]"%item,
        if (item[0]>=0 and item[0]<=4 and item[1]>=0 and item[1]<=4):
            #print "\t  in"
            counter_in = counter_in + 1
        else:
            #print "\t out"
            counter_out = counter_out + 1
    if counter == 0:
        counter = 1
    if counter_in == 0:
        counter_in = 1
    if counter_out == 0:
        counter_out = 1
    print "====================================="
    print "Results: \n%%%3.1f in, %%%3.1f out"%\
          (counter_in/float(counter_in+counter_out)*100,\
           counter_out/float(counter_in+counter_out)*100)
    print "Average Position:"
    print "[%2.1f,%2.1f]"%(pos[0]/float(counter),pos[1]/float(counter))
    #return [pos[0]/float(counter),pos[1]/float(counter)]
    if (counter_in>counter_out):
        return 1
    else:
        return 0

vector = getVector(4,-70)
#print "distance vector of (4m,-65dBm):"
#print vector
#print ""
#print sys.argv
#calculatePosition(eval(sys.argv[1]),eval(sys.argv[2]),eval(sys.argv[3]),eval(sys.argv[4]))
