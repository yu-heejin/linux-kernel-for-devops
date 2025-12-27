import threading

def infinite():
    test = 0
    while True:
        test += 1

threads = []

for i in range(10):
    thread = threading.Thread(target=infinite, args=[])
    thread.start()

for thread in threads:
    thread.join()