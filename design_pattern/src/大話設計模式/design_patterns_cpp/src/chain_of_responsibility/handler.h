#ifndef design_patterns_chain_of_responsiblity_handler_h
#define design_patterns_chain_of_responsiblity_handler_h

#include "request.h"

class Handler
{
protected:
    Handler* successor_;
public:
    Handler(): successor_{nullptr} {}
    virtual ~Handler() {}
    Handler(const Handler&) = delete;
    Handler& operator=(const Handler&) = delete;
    void set_successor(Handler* successor)
    {
        successor_ = successor;
    }
    virtual void HandleRequest(Request&) {}
};

class ConcreteHandler1 : public Handler
{
public:
    void HandleRequest(Request& request)
    {
        if (request.type() <= 1) {
            printf("Handler 1 permit %s.\n", request.content().c_str());
        } else if (successor_ != nullptr) {
            successor_->HandleRequest(request);
        }
    }
};

class ConcreteHandler2 : public Handler
{
public:
    void HandleRequest(Request& request)
    {
        if (request.type() <= 2) {
            printf("Handler 2 permit %s.\n", request.content().c_str());
        } else if (successor_ != nullptr) {
            successor_->HandleRequest(request);
        }
    }
};

class ConcreteHandler3 : public Handler
{
public:
    void HandleRequest(Request& request)
    {
        if (request.type() <= 3) {
            printf("Handler 3 permit %s.\n", request.content().c_str());
        } else if (successor_ != nullptr) {
            successor_->HandleRequest(request);
        }
    }
};

#endif
