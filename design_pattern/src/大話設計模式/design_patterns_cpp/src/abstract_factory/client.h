#ifndef design_patterns_abstract_factory_client_h
#define design_patterns_abstract_factory_client_h

#include "abstract_product_a.h"
#include "abstract_product_b.h"
#include "factory1.h"
#include "factory2.h"

void client()
{
    User user;
    Department department;
    // Use SQL Server
    Factory1 db;
    AbstractProductA* user_inf = db.CreateProductA();
    AbstractProductB* department_inf = db.CreateProductB();
    user_inf->Insert(user);
    user_inf->GetUser(user.id);
    department_inf->Insert(department);
    department_inf->GetDepartment(department.id);
    // Use Access
    Factory2 db2;
    AbstractProductA* user_inf2 = db2.CreateProductA();
    AbstractProductB* department_inf2 = db2.CreateProductB();
    user_inf2->Insert(user);
    user_inf2->GetUser(user.id);
    department_inf2->Insert(department);
    department_inf2->GetDepartment(department.id);
}

#endif
