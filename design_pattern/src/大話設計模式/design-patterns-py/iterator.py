import abc


class Aggregate:
    __metaclass__ = abc.ABCMeta

    @abc.abstractmethod
    def create_iterator(self):
        """create iterator"""


class List(Aggregate):
    def __init__(self):
        self.items = []

    def create_iterator(self):
        return ListIterator(self)

    def count(self):
        return len(self.items)

    def __setitem__(self, key, value):
        if key >= len(self.items):
            self.items.append(value)
        else:
            self.items[key] = value

    def __getitem__(self, i):
        return self.items[i]


class Iterator:
    __metaclass__ = abc.ABCMeta

    @abc.abstractmethod
    def first_item(self):
        """"""

    @abc.abstractmethod
    def next_item(self):
        """"""

    @abc.abstractmethod
    def is_done(self):
        """"""

    @abc.abstractmethod
    def current_item(self):
        """"""

class ListIterator(Iterator):
    def __init__(self, a):
        self.current = 0
        self.aggregate = a

    def first_item(self):
        return self.aggregate[0]

    def next_item(self):
        result = None
        self.current += 1
        if self.current < self.aggregate.count():
            result = self.aggregate[self.current]
        return result

    def is_done(self):
        return self.current >= self.aggregate.count()

    def current_item(self):
        return self.aggregate[self.current]


if __name__ == "__main__":
    list = List()
    list[0] = 1
    list[1] = 2
    list[2] = 3
    list_iterator = list.create_iterator()
    print list_iterator.first_item()
    print list_iterator.current_item()
    print list_iterator.next_item()
    print list_iterator.next_item()
    print list_iterator.is_done()
    print list_iterator.next_item()
    print list_iterator.is_done()



