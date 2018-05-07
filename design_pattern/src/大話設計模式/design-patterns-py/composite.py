import abc


class Company:
    __metaclass__ = abc.ABCMeta

    def __init__(self, n):
        self.name = n

    @abc.abstractmethod
    def add(self, c):
        """add"""

    @abc.abstractmethod
    def display(self, d):
        """display"""

    @abc.abstractmethod
    def line_of_duty(self):
        """duty line"""


class HrDepartment(Company):
    def __init__(self, n):
        Company.__init__(self, n)

    def add(self, c):
        print "leaf can't add component"

    def display(self, d):
        print "--" * d + self.name

    def line_of_duty(self):
        print self.name + " : human resources"


class FinanceDepartment(Company):
    def __init__(self, n):
        Company.__init__(self, n)

    def add(self, c):
        print "leaf can't add component"

    def display(self, d):
        print "--" * d + self.name

    def line_of_duty(self):
        print self.name + " : finance analysis"


class ConcreteCompany(Company):
    def __init__(self, n):
        Company.__init__(self, n)
        self.companies = []

    def add(self, c):
        self.companies.append(c)

    def display(self, d):
        print "--" * d + self.name
        for company in self.companies:
            company.display(d + 1)

    def line_of_duty(self):
        for company in self.companies:
            company.line_of_duty()


if __name__ == "__main__":
    beijing_head_office = ConcreteCompany("Beijing Head Office")
    beijing_head_office.add(HrDepartment("Beijing HR"))
    beijing_head_office.add(FinanceDepartment("Beijing Finance"))

    huadong_branch_office = ConcreteCompany("Huadong Branch Office")
    huadong_branch_office.add(HrDepartment("Huadong HR"))
    huadong_branch_office.add(FinanceDepartment("Huadong Finance"))
    beijing_head_office.add(huadong_branch_office)

    nanjing_office = ConcreteCompany("Nanjing Office")
    nanjing_office.add(HrDepartment("Nanjing HR"))
    nanjing_office.add(FinanceDepartment("Nanjing Finance"))
    huadong_branch_office.add(nanjing_office)

    hangzhou_office = ConcreteCompany("Hangzhou Office")
    hangzhou_office.add(HrDepartment("Hangzhou HR"))
    hangzhou_office.add(FinanceDepartment("Hangzhou Finance"))
    huadong_branch_office.add(hangzhou_office)

    beijing_head_office.display(0)

    beijing_head_office.line_of_duty()
