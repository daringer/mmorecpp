#from random import random as r
#
#means = 0.4, 1.2, 2.5
#
#data =  [means[0]+((0.66*r()*means[0])-means[0]) for i in xrange(2000)]
#data += [means[1]+((0.66*r()*means[1])-means[1]) for i in xrange(1000)]
#data += [means[2]+((0.66*r()*means[2])-means[2]) for i in xrange(1000)]
#diff = data[2] - data[0]
#data += [means[0] + diff*r() for i in xrange(500)]
#
#
#with file("./kmeans_test_data_1.txt", "w") as fd:
#    fd.write(" ".join(map(str, data)))
#fd.close()
#
# ====== test_data/kmeans_test_data_1.txt 

from random import random as r
from random import shuffle

means = 0.4, 1.2, 2.5

data =  [means[0]+((0.66*r()*means[0])-means[0]) for i in xrange(2000)]
data += [means[1]+((0.66*r()*means[1])-means[1]) for i in xrange(1000)]
data += [means[2]+((0.66*r()*means[2])-means[2]) for i in xrange(1000)]
diff = data[2] - data[0]
data += [means[0] + diff*r() for i in xrange(500)]

shuffle(data)

with file("./kmeans_test_data_2.txt", "w") as fd:
    fd.write(" ".join(map(str, data)))
fd.close()

# ====== test_data/kmeans_test_data_2.txt -> equal data, just shuffled once!
