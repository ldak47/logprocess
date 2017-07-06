#!/homw/work/.jumbo/bin/python
import copy
import json


a = [1, 2, 3, 4, 5, [99, 98]]
#b全都是a的引用
b = a
#c的非子对象都是a的复制构造, 但子对象还是引用
c = copy.copy(a)
#d全都是a的复制构造
d = copy.deepcopy(a)

print "a append a new element 100"
a.append(100)

print "a: %s" % json.dumps(a)
print "b = a, b will change: %s" % json.dumps(b)
print "c = copy.copy(a): %s" % json.dumps(c)
print "d = copy.deepcopy(a): %s" % json.dumps(d)

print "a's subobject append a new element 100"
a[5].append(100)

print "a: %s" % json.dumps(a)
print "b = a, b will change: %s" % json.dumps(b)
print "c = copy.copy(a), will change: %s" % json.dumps(c)
print "d = copy.deepcopy(a): %s" % json.dumps(d)

