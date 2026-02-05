#!/usr/bin/python

"""
미리 Redis 서버로의 세션을 열어놓고 키를 설정할 때마다 가져다가 사용한다.
"""

import redis
import time

count = 0
pool = redis.ConnectionPool(host='infra-redis.redis.iwilab.com', port=6379, db=0)

while True:
    if count > 10000:
        break;

    r = redis.Redis(connection_pool=pool)
    print("SET")
    r.setex(count,count,10)