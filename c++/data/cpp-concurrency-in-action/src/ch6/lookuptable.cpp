#include <iostream>
#include <string>
#include <thread>
#include <boost/thread/shared_mutex.hpp>
#include <memory>
#include <list>
#include <vector>

template <typename Key, typename Value, typename Hash = std::hash<Key> >
class lookup_table
{
private:
    class bucket_type
    {
    private:
        typedef std::pair<Key, Value> bucket_value;
        typedef std::list<bucket_value> bucket_data;
        typedef typename bucket_data::iterator bucket_iterator;

        bucket_data data;
        mutable boost::shared_mutex mutex;

        bucket_iterator find_entry_for(Key const& key) const
        {
            return std::find_if(data.begin(), data.end(),
            [&](bucket_value const & item) {
                return item.first == key;
            });
        }

    public:
        Value value_for(Key const& key, Value const& default_value) const
        {
            boost::shared_lock<boost::shared_mutex> lock(mutex);
            bucket_iterator const found_entry = find_entry_for(key);
            return (found_entry == data.end()) ?
                   default_value : found_entry->second;
        }

        void add_or_update_mapping(Key const& key, Value const& value)
        {
            std::unique_lock<boost::shared_mutex> lock(mutex);
            bucket_iterator const found_entry = find_entry_for(key);

            if (found_entry == data.end()) {
                data.push_back(bucket_value(key, value));
            } else {
                found_entry->second = value;
            }
        }

        void remove_mapping(Key const& key)
        {
            std::unique_lock<boost::shared_mutex> lock(mutex);
            bucket_iterator const found_entry = find_entry_for(key);

            if (found_entry != data.end()) {
                data.erase(found_entry);
            }
        }
    };

    std::vector<std::unique_ptr<bucket_type> > buckets;
    Hash hasher;

    bucket_type& get_bucket(Key const& key) const
    {
        const std::size_t bucket_index = hasher(key) % buckets.size();
        return *buckets[bucket_index];
    }

public:
    typedef Key key_type;
    typedef Value mapped_type;
    typedef Hash hash_type;

    lookup_table(unsigned num_buckets = 19, const Hash& hasher_ = Hash())
        : buckets(num_buckets)
        , hasher(hasher_)
    {
        for (unsigned i = 0; i < num_buckets; ++i) {
            buckets[i].reset(new bucket_type);
        }
    }

    lookup_table(const lookup_table& other) = delete;
    lookup_table& operator=(const lookup_table& other) = delete;

    Value value_for(Key const& key,
                    Value const& default_value = Value()) const
    {
        return get_bucket(key).value_for(key, default_value);
    }

    void add_or_update_mapping(const Key& key, const Value& value)
    {
        get_bucket(key).add_or_update_mapping(key, value);
    }

    void remove_mapping(const Key& key)
    {
        get_bucket(key).remove_mapping(key);
    }

    std::map<Key, Value> get_map() const
    {
        std::vector<boost::unique_lock<boost::shared_mutex> > locks;

        for (unsigned i = 0;  i < buckets.size(); ++i) {
            locks.push_back(boost::unique_lock<boost::shared_mutex>(
                                buckets[i].mutex));
        }

        std::map<Key, Value> res;

        for (unsigned i = 0; i < buckets.size(); ++i) {
            for (typename bucket_type::bucket_iterator it = buckets[i].data.begin();
                 it != buckets[i].data.end();
                 ++it) {
                res.insert(*it);
            }
        }

        return res;
    }
};

int main()
{
    lookup_table<int, std::string> t;
    t.add_or_update_mapping(1, "hello");
    std::cout << "value for 1 is " << t.value_for(1) << std::endl;
    return 0;
}

