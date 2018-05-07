import threading


class Singleton(object):
    instance = None
    lock = threading.RLock()

    @classmethod
    def __new__(cls):
        if cls.instance is None:
            cls.lock.acquire()
            if cls.instance is None:
                cls.instance = super(Singleton, cls).__new__(cls)
            cls.lock.release()
        return cls.instance


if __name__ == "__main__":
    instance1 = Singleton()
    instance2 = Singleton()
    print id(instance1) == id(instance2)
