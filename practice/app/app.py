import redis
import time
from flask import Flask

app = Flask(__name__)

@app.route("/test/<key>")
def testApp(key):
    r = redis.StrictRedis(host='redis', port=6379, db=2)
    r.set(key, time.time())

    return r.get(key)

if __name__ == "__main__":
    app.run(host='0.0.0.0', port=5000)