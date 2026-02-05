#!/usr/bin/python

"""
키를 설정할 때마다 Redis 서버로의 세션을 새로 연결
"""

import redis
import time

count = 0

while True:
    if count > 10000:
        break;

    r = redis.Redis(host='redis.server', port=6379, db=0)
    print("SET")
    r.setex(count,count,10)