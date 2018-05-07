import abc


class User:
    def __init__(self, n):
        self.name = n


class Website:
    __metaclass__ = abc.ABCMeta

    @abc.abstractmethod
    def use(self, u):
        """users use website"""


class ConcreteWebsite(Website):
    def __init__(self, wn):
        self.website_name = wn

    def use(self, u):
        print u.name + " use " + self.website_name


class WebsiteFactory:
    flyweights = {}

    def get_website_category(self, n):
        if self.flyweights.has_key(n) is False:
            self.flyweights[n] = ConcreteWebsite(n)
        return self.flyweights[n]

    def get_website_count(self):
        print len(self.flyweights.keys())


if __name__ == "__main__":
    website_factory = WebsiteFactory()
    alice = User("Alice")
    bob = User("Bob")

    website = website_factory.get_website_category("bbs")
    website.use(alice)
    website.use(bob)
    website_factory.get_website_count()

    website = website_factory.get_website_category("blog")
    website.use(alice)
    website.use(bob)
    website_factory.get_website_count()

    website = website_factory.get_website_category("bbs")
    website.use(alice)
    website.use(bob)
    website_factory.get_website_count()
