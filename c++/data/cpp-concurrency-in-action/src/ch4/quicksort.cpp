#include <iostream>
#include <cstdio>
#include <list>
#include <thread>
#include <memory>
#include <algorithm>
#include <future>

std::ostream& operator<<(std::ostream& ostr, const std::list<int>& list)
{
    for (auto& i : list) {
        ostr << " " << i;
    }

    return ostr;
}

template<typename T>
std::list<T> sequential_quick_sort(std::list<T> input)
{
    if (input.empty()) {
        return input;
    }

    std::list<T> result;
    result.splice(result.begin(), input, input.begin());
    T const& pivot = *result.begin();
    auto divide_point = std::partition(input.begin(), input.end(),
    [&](T const & t) {
        return t < pivot;
    });
    std::list<T> lower_part;
    lower_part.splice(lower_part.end(), input, input.begin(),
                      divide_point);
    auto new_lower(
        sequential_quick_sort(std::move(lower_part)));
    auto new_higher(
        sequential_quick_sort(std::move(input)));
    result.splice(result.end(), new_higher);
    result.splice(result.begin(), new_lower);
    return result;
}

template<typename T>
std::list<T> parallel_quick_sort(std::list<T> input)
{
    if (input.empty()) {
        return input;
    }

    std::list<T> result;
    result.splice(result.begin(), input, input.begin());
    T const& pivot = *result.begin();
    auto divide_point = std::partition(input.begin(), input.end(),
    [&](T const & t) {
        return t < pivot;
    });
    std::list<T> lower_part;
    lower_part.splice(lower_part.end(), input, input.begin(),
                      divide_point);
    std::future<std::list<T> > new_lower(
        std::async(&parallel_quick_sort<T>, std::move(lower_part)));
    auto new_higher(
        parallel_quick_sort(std::move(input)));
    result.splice(result.end(), new_higher);
    result.splice(result.begin(), new_lower.get());
    return result;
}

int main()
{
    std::list<int> list1 = {4, 2, 1, 5, 8, 9, 7};
    std::list<int> sorted1 = sequential_quick_sort(list1);
    std::list<int> sorted2 = parallel_quick_sort(list1);
    std::cout << "sorted1: " << sorted1 << std::endl;
    std::cout << "sorted2: " << sorted2 << std::endl;
    return 0;
}

