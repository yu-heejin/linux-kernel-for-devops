#!/usr/bin/python

import datetime

start_time = datetime.datetime.now()
print("START : " + str(start_time))
sum = 0

for i in range(1,50000000):
    sum = sum + i
    # print(i)
print(sum)
end_time = datetime.datetime.now()
print("END : " + str(end_time))
elapsed_time = end_time - start_time
print("ELAPSED TIME : " + str(elapsed_time))