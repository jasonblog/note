import abc


class User:
    """class user"""


class IUser:
    __metaclass__ = abc.ABCMeta

    @abc.abstractmethod
    def insert_user(self, user):
        """insert user"""

    @abc.abstractmethod
    def get_user(self):
        """get user"""


class SqlserverUser(IUser):
    def insert_user(self, user):
        print "Insert user into Sqlserver"

    def get_user(self):
        print "Get user from Sqlserver"


class AccessUser(IUser):
    def insert_user(self, user):
        print "Insert user into Access"

    def get_user(self):
        print "Get user from Access"


class Department:
    """class department"""


class IDepartment:
    __metaclass__ = abc.ABCMeta

    @abc.abstractmethod
    def insert_department(self, department):
        """insert department"""

    def get_department(self):
        """get department"""


class SqlserverDepartment(IDepartment):
    def insert_department(self, department):
        print "Insert department into Sqlserver"

    def get_department(self):
        print "Get department from Sqlserver"


class AccessDepartment(IDepartment):
    def insert_department(self, department):
        print "Insert department into Access"

    def get_department(self):
        print "Get department from Access"


class IFactory:
    __metaclass__ = abc.ABCMeta

    @abc.abstractmethod
    def create_user(self):
        """create user"""

    @abc.abstractmethod
    def create_department(self):
        """create deparment"""


class SqlserverFactory(IFactory):
    def create_user(self):
        return SqlserverUser()

    def create_department(self):
        return SqlserverDepartment()


class AccessFactory(IFactory):
    def create_user(self):
        return AccessUser()

    def create_department(self):
        return AccessDepartment()


if __name__ == "__main__":
    # use sqlserver to insert/get user/department
    i_factory = SqlserverFactory()
    i_user = i_factory.create_user()
    i_user.insert_user(User())
    i_user.get_user()
    i_department = i_factory.create_department()
    i_department.insert_department(Department())
    i_department.get_department()

    # use access to insert/get user/department
    i_factory = AccessFactory()  # just replace SqlserverFactory with AccessFactory
    i_user = i_factory.create_user()
    i_user.insert_user(User())
    i_user.get_user()
    i_department = i_factory.create_department()
    i_department.insert_department(Department())
    i_department.get_department()
