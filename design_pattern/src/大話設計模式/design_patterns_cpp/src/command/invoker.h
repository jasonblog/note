#ifndef design_patterns_command_invoker_h
#define design_patterns_command_invoker_h

#include "command.h"
#include <list>
using std::list;

class Waiter
{
private:
    list<Command*> orders_;
public:
    Waiter(): orders_{} {}
    void AddOrder(Command* command)
    {
        switch (command->id()) {
        case 2:
            printf("No order %d.\n", command->id());
            break;

        default:
            orders_.push_back(command);
            printf("Add order %d\n", command->id());
        }
    }
    void RemoveOrder(Command* command)
    {
        for (auto it = orders_.begin(); it != orders_.end(); ++it) {
            if (*it == command) {
                printf("remove order %d\n", command->id());
                orders_.erase(it);
                break;
            }
        }
    }
    void Notify()
    {
        for (auto order : orders_) {
            order->ExecuteCommand();
        }
    }
};

#endif
