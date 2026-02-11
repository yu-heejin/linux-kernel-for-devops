import MySQLdb as mdb
import sys
import time
from datetime import datetime

con = mdb.connect('dbserver', 'root', 'password', 'test')

while True:
    cur = con.cursor()
    cur.execute("SELECT VERSION()")

    ver = cur.fetchone()

    print("Database version : %s" % ver)
    time.sleep(600)