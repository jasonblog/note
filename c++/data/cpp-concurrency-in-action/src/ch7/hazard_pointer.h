#include <memory>
#include <thread>
#include <cstdio>

unsigned const max_hazard_pointers = 100;
struct hazard_pointer
{
    std::atomic<std::thread::id> id;
    std::atomic<void*> pointer;
};
hazard_pointer hazard_pointers[max_hazard_pointers];

class hp_owner
{
    hazard_pointer* hp;

  public:
    hp_owner(hp_owner const&)=delete;
    hp_owner operator=(hp_owner const&)=delete;
    hp_owner()
    : hp(nullptr)
    {
        for (unsigned i = 0; i < max_hazard_pointers; ++i) {
            std::thread::id old_id;
            // try to claim ownership of a hazard pointer
            if (hazard_pointers[i].id.compare_exchange_strong(
                old_id, std::this_thread::get_id())) {
                // successfully claimed the entry for the current thread,
                // store it and stop the search
                hp = &hazard_pointers[i];
                printf("hp: %ld\n", hp);
                break;
            }
            // another threads owns this entry, move on to the next
        }
        // if you get to the end of the list without finding a free entry,
        // there are too many threads using hazard pointers, so throw an
        // exception
        if (!hp) {
            throw std::runtime_error("No hazard pointers available");
        }
    }

    std::atomic<void*>& get_pointer()
    {
        return hp->pointer;
    }

    ~hp_owner()
    {
        // when each thread exits, if an instance of hp_owner was created
        // for the thread, then it's destryoed. The destructor then resets
        // the actual pointer to nullptr before setting the owner ID to 
        // std::thread::id(), allowing another thread to reuse the entry later.
        // TODO: this causes crash
        //hp->pointer.store(nullptr);
        //hp->id.store(std::thread::id());
    }
};




