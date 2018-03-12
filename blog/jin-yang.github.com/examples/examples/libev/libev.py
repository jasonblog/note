import signal
import pyev

def sig_cb(watcher, revents):
    print("got SIGINT")
    loop = watcher.loop
    # optional - stop all watchers
    if loop.data:
        print("stopping watchers: {0}".format(loop.data))
        while loop.data:
            loop.data.pop().stop()
    # unloop all nested loop
    print("stopping the loop: {0}".format(loop))
    loop.stop(pyev.EVBREAK_ALL)

def timer_cb(watcher, revents):
    watcher.data += 1
    print("timer.data: {0}".format(watcher.data))
    print("timer.loop.iteration: {0}".format(watcher.loop.iteration))
    print("timer.loop.now(): {0}".format(watcher.loop.now()))

if __name__ == "__main__":
    loop = pyev.default_loop()
    # initialise and start a repeating timer
    timer = loop.timer(0, 2, timer_cb, 0)
    timer.start()
    # initialise and start a Signal watcher
    sig = loop.signal(signal.SIGINT, sig_cb)
    sig.start()
    loop.data = [timer, sig] # optional
    # now wait for events to arrive
    loop.start()
