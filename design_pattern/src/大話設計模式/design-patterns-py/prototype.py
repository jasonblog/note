import copy


class WorkExperience:
    def __init__(self):
        self.company = ""
        self.time_area = ""


class Resume:
    def __init__(self, n):
        self.name = n
        self.work_experience = WorkExperience()

    def set_personal_info(self, s, a):
        self.sex = s
        self.age = a

    def set_work_experience(self, c, ta):
        self.work_experience.company = c
        self.work_experience.time_area = ta

    def print_resume(self):
        print self.name + ", " + self.sex + ", " + self.age + ", " \
              + self.work_experience.company + " : " + self.work_experience.time_area

    def clone(self):
        new_resume = copy.deepcopy(self)
        new_resume.work_experience = copy.deepcopy(self.work_experience)
        return new_resume


if __name__ == "__main__":
    resume1 = Resume("Bob")
    resume1.set_personal_info("M", "24")
    resume1.set_work_experience("Google", "2015")
    resume2 = resume1
    resume2.set_personal_info("F", "22")
    resume2.set_work_experience("Twitter", "2015")
    resume1.print_resume()
    resume2.print_resume()

    resume1 = Resume("Bob")
    resume1.set_personal_info("M", "24")
    resume1.set_work_experience("Google", "2015")
    resume2 = copy.copy(resume1)
    resume2.set_personal_info("F", "22")
    resume2.set_work_experience("Twitter", "2015")
    resume1.print_resume()
    resume2.print_resume()

    resume1 = Resume("Bob")
    resume1.set_personal_info("M", "24")
    resume1.set_work_experience("Google", "2015")
    resume2 = copy.deepcopy(resume1)
    resume2.set_personal_info("F", "22")
    resume2.set_work_experience("Twitter", "2015")
    resume1.print_resume()
    resume2.print_resume()

    resume1 = Resume("Bob")
    resume1.set_personal_info("M", "24")
    resume1.set_work_experience("Google", "2015")
    resume2 = resume1.clone()
    resume2.set_personal_info("F", "22")
    resume2.set_work_experience("Twitter", "2015")
    resume1.print_resume()
    resume2.print_resume()
