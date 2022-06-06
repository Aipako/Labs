// compileTime.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <stdarg.h>


// dot, coef1, coef2, coef3...
template<typename T, typename Arg1, typename... Args>
class polynomClass
{
public:
    const unsigned long long value;
    //unsigned long long value; 
    template<typename T, typename Arg1, typename... Args>
    constexpr  polynomClass(const T dot, const Arg1& arg1, const Args&... args) : value(get_polynom(dot, arg1, args...))
    {
       //return get_polynom(dot, arg1, args...);
       //auto arguments = args;
    }
    //constexpr unsigned long long value = polynomGet();
    template<typename T, typename Arg1, typename... Args>
    constexpr unsigned long long get_polynom(const T dot, const Arg1& arg1, const Args&... args) const
    {

        return  arg1 + get_polynom2(dot, dot,  args...);
        //return result;
    }

    template<typename T, typename Arg1, typename... Args>
    constexpr unsigned long long get_polynom2(const T dotOrig, const T dot, const Arg1& arg1, const Args&... args) const
    {

        return (dot * arg1) + get_polynom2(dotOrig, dot * dotOrig, args...);
        //return result;
    }
    template<typename T>
    constexpr unsigned long long get_polynom2(const T dotOrig,const T dot) const
    {
        return 0;
    }
    template<typename T, typename Arg1, typename... Args>
    constexpr unsigned long long polynomGet(const T dot, const Arg1& arg1, const Args&... args) const
    {
        return get_polynom(dot, arg1,  args...);
    }
    //Args arguments;
};



template<typename T, typename Arg1, typename... Args>
std::ostream& operator<< (std::ostream& os,const polynomClass<T, Arg1, Args...>& pol)
{
    return os << pol.value;
}

int main()
{
    constexpr unsigned long long dot = 2;
    constexpr unsigned long long first = 2;
    constexpr unsigned long long second = 3;
    constexpr unsigned long long third = 0;
    constexpr unsigned long long fourth = 10;
    
    constexpr polynomClass<int, int> pol(3, 3, 5, 6);
    constexpr polynomClass<int, int> pol2(1, 2094, 23, 1);
    constexpr unsigned long long n = pol.polynomGet(2, 3, 5);
    constexpr int n2 = pol.value;
    std::cout << pol;
}