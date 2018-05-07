import abc


class Action(object):
    __metaclass__ = abc.ABCMeta

    @abc.abstractmethod
    def get_man_conclusion(self, p):
        pass

    @abc.abstractmethod
    def get_woman_conclusion(self, p):
        pass


class Success(Action):
    def get_man_conclusion(self, p):
        print "man success"

    def get_woman_conclusion(self, p):
        print "woman success"


class Failure(Action):
    def get_man_conclusion(self, p):
        print "man failure"

    def get_woman_conclusion(self, p):
        print "woman failure"


class Person(object):
    __metaclass__ = abc.ABCMeta

    @abc.abstractmethod
    def accept(self, a):
        pass


class Man(Person):
    def accept(self, a):
        a.get_man_conclusion(self)


class Woman(Person):
    def accept(self, a):
        a.get_woman_conclusion(self)


class ObjectStructure(object):
    def __init__(self):
        self.people = []

    def attach(self, p):
        self.people.append(p)

    def detach(self, p):
        self.people.remove(p)

    def display(self, a):
        for person in self.people:
            person.accept(a)


if __name__ == "__main__":
    man = Man()
    woman = Woman()
    object_structure = ObjectStructure()
    object_structure.attach(man)
    object_structure.attach(woman)
    object_structure.display(Success())
    object_structure.display(Failure())
