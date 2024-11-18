#include "function.hpp"

long Ack(long m, long n)
{
    if (!m) return n + 1;
    if (!n) return Ack(m - 1, 1);
    return Ack(m - 1, Ack(m, n - 1));
}