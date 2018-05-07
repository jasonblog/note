import abc


class Observer:
    __metaclass__ = abc.ABCMeta

    def __init__(self, n, nr):
        self.name = n
        self.notifier = nr

    @abc.abstractmethod
    def update(self):
        """updated by notifier"""


class NbaObserver(Observer):
    def update(self):
        print self.name + ", " + self.notifier.state + ", close NBA"


class StockObserver(Observer):
    def update(self):
        print self.name + ", " + self.notifier.state + ", close stock"


class Notifier:
    def __init__(self):
        self.observers = []
        self.state = ""

    def attach(self, o):
        self.observers.append(o)

    def detach(self, o):
        self.observers.remove(o)

    def notify(self):
        for observer in self.observers:
            observer.update()


class Boss(Notifier):
    pass


class Secretary(Notifier):
    pass


if __name__ == "__main__":
    boss = Boss()
    nba_observer = NbaObserver("Bob", boss)
    boss.attach(nba_observer)
    stock_observer = StockObserver("Alice", boss)
    boss.attach(stock_observer)
    boss.state = "boss's back himself"
    boss.notify()
