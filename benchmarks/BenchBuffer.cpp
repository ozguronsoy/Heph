#include <benchmark/benchmark.h>
#include <vector>
#include "Heph/Buffers/Buffer.h"

using namespace Heph;
using test_data_t = int;
static constexpr benchmark::TimeUnit TIME_UNIT = benchmark::kMillisecond;

template<
    size_t NDimensions,
    template<typename, size_t> typename TIterator = BufferIterator
>
    requires (NDimensions > 0 && NDimensions <= 2)
class TestBuffer : public Buffer<test_data_t, NDimensions, TIterator>
{
public:
    using Base = Buffer<test_data_t, NDimensions, TIterator>;
    using typename Base::iterator;
    using typename Base::const_iterator;
    using typename Base::buffer_size_t;

    using Base::Base;
    using Base::operator=;

public:
    template<typename... Args>
    TestBuffer(Args... args) : Base(std::forward<Args>(args)...) {}

    TestBuffer operator<<(size_t n) const
    {
        TestBuffer result = *this;
        return result <<= n;
    }

    TestBuffer& operator<<=(size_t n)
    {
        Base::ShiftLeft(*this, n);
        return *this;
    }

    TestBuffer operator>>(size_t n) const
    {
        TestBuffer result = *this;
        return result >>= n;
    }

    TestBuffer& operator>>=(size_t n)
    {
        Base::ShiftRight(*this, n);
        return *this;
    }

    TestBuffer SubBuffer(size_t index, size_t size)
    {
        TestBuffer result;
        Base::SubBuffer(*this, result, index, size);
        return result;
    }

    void Prepend(const TestBuffer& b)
    {
        Base::Prepend(*this, b);
    }

    void Append(const TestBuffer& b)
    {
        Base::Append(*this, b);
    }

    void Insert(const TestBuffer& b, size_t index)
    {
        Base::Insert(*this, b, index);
    }

    void Cut(size_t index, size_t size)
    {
        Base::Cut(*this, index, size);
    }

    void Replace(const TestBuffer& b, size_t index, size_t bIndex, size_t size)
    {
        Base::Replace(*this, b, index, bIndex, size);
    }

    void Transpose(Enum<TransposeMode> mode, auto... perm)
    {
        static_assert(sizeof...(perm) == NDimensions, "Invalid number of perm parameters.");
        static_assert((std::is_convertible_v<decltype(perm), size_t> && ...), "Invalid type for perm parameters, must be convertible to size_t.");

        const buffer_size_t permArray = { std::forward<size_t>(static_cast<size_t>(perm))... };
        this->Transpose(mode, permArray);
    }

    void Transpose(Enum<TransposeMode> mode, const buffer_size_t& perm)
    {
        Base::Transpose(*this, *this, perm, mode);
    }

    void Resize(auto... newSize)
    {
        static_assert(sizeof...(newSize) > 0 && sizeof...(newSize) <= NDimensions, "Invalid number of newSize parametrs");
        static_assert((std::is_convertible_v<decltype(newSize), size_t> && ...), "Invalid type for newSize parameters, must be convertible to size_t.");

        const buffer_size_t ns = { std::forward<size_t>(static_cast<size_t>(newSize))... };
        Base::Resize(*this, ns);
    }

    void Reverse(size_t dim)
    {
        Base::Reverse(*this, dim);
    }
};


static void BM_BufferCreation1D(benchmark::State& state)
{
    for (auto _ : state)
    {
        TestBuffer<1> b(state.range(0));
        benchmark::DoNotOptimize(b);
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_BufferCreation1D)->Unit(TIME_UNIT)->Arg(1e6);

static void BM_VectorCreation1D(benchmark::State& state)
{
    for (auto _ : state)
    {
        std::vector<test_data_t> v(state.range(0));
        benchmark::DoNotOptimize(v);
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_VectorCreation1D)->Unit(TIME_UNIT)->Arg(1e6);

static void BM_BufferCopy1D(benchmark::State& state)
{
    TestBuffer<1> b1(state.range(0));
    TestBuffer<1> b2;

    benchmark::DoNotOptimize(b1);
    benchmark::DoNotOptimize(b2);

    for (auto _ : state)
    {
        b2 = b1;
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_BufferCopy1D)->Unit(TIME_UNIT)->Arg(1e6);

static void BM_VectorCopy1D(benchmark::State& state)
{
    std::vector<test_data_t> v1(state.range(0));
    std::vector<test_data_t> v2;

    benchmark::DoNotOptimize(v1);
    benchmark::DoNotOptimize(v2);

    for (auto _ : state)
    {
        v2 = v1;
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_VectorCopy1D)->Unit(TIME_UNIT)->Arg(1e6);

static void BM_BufferMove1D(benchmark::State& state)
{
    TestBuffer<1> b1(state.range(0));
    TestBuffer<1> b2;

    benchmark::DoNotOptimize(b1);
    benchmark::DoNotOptimize(b2);

    for (auto _ : state)
    {
        b2 = std::move(b1);
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_BufferMove1D)->Unit(TIME_UNIT)->Arg(1e6);

static void BM_VectorMove1D(benchmark::State& state)
{
    std::vector<test_data_t> v1(state.range(0));
    std::vector<test_data_t> v2;

    benchmark::DoNotOptimize(v1);
    benchmark::DoNotOptimize(v2);

    for (auto _ : state)
    {
        v2 = std::move(v1);
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_VectorMove1D)->Unit(TIME_UNIT)->Arg(1e6);

static void BM_BufferAccess1D(benchmark::State& state)
{
    TestBuffer<1> b(state.range(0));
    size_t size = b.Size();

    benchmark::DoNotOptimize(b);
    benchmark::DoNotOptimize(size);

    for (auto _ : state)
    {
        for (size_t i = 0; i < size; ++i)
        {
            benchmark::DoNotOptimize(b[i]);
            benchmark::ClobberMemory();
        }
    }
}
BENCHMARK(BM_BufferAccess1D)->Unit(TIME_UNIT)->Arg(1e6);

static void BM_VectorAccess1D(benchmark::State& state)
{
    std::vector<test_data_t> v(state.range(0));
    size_t size = v.size();

    benchmark::DoNotOptimize(v);
    benchmark::DoNotOptimize(size);

    for (auto _ : state)
    {
        for (size_t i = 0; i < size; ++i)
        {
            benchmark::DoNotOptimize(v[i]);
            benchmark::ClobberMemory();
        }
    }
}
BENCHMARK(BM_VectorAccess1D)->Unit(TIME_UNIT)->Arg(1e6);

static void BM_BufferAtAccess1D(benchmark::State& state)
{
    TestBuffer<1> b(state.range(0));
    size_t size = b.Size();

    benchmark::DoNotOptimize(b);
    benchmark::DoNotOptimize(size);

    for (auto _ : state)
    {
        for (size_t i = 0; i < size; ++i)
        {
            benchmark::DoNotOptimize(b.At(i));
            benchmark::ClobberMemory();
        }
    }
}
BENCHMARK(BM_BufferAtAccess1D)->Unit(TIME_UNIT)->Arg(1e6);

static void BM_VectorAtAccess1D(benchmark::State& state)
{
    std::vector<test_data_t> v(state.range(0));
    size_t size = v.size();

    benchmark::DoNotOptimize(v);
    benchmark::DoNotOptimize(size);

    for (auto _ : state)
    {
        for (size_t i = 0; i < size; ++i)
        {
            benchmark::DoNotOptimize(v.at(i));
            benchmark::ClobberMemory();
        }
    }
}
BENCHMARK(BM_VectorAtAccess1D)->Unit(TIME_UNIT)->Arg(1e6);

static void BM_BufferItAccess1D(benchmark::State& state)
{
    TestBuffer<1> b(state.range(0));
    auto itBegin = b.begin();
    auto itEnd = b.end();

    benchmark::DoNotOptimize(b);
    benchmark::DoNotOptimize(itBegin);
    benchmark::DoNotOptimize(itEnd);

    for (auto _ : state)
    {
        for (auto it = itBegin; it != itEnd; ++it)
        {
            benchmark::DoNotOptimize(*it);
            benchmark::ClobberMemory();
        }
    }
}
BENCHMARK(BM_BufferItAccess1D)->Unit(TIME_UNIT)->Arg(1e6);

static void BM_VectorItAccess1D(benchmark::State& state)
{
    std::vector<test_data_t> v(state.range(0));
    auto itBegin = v.begin();
    auto itEnd = v.end();

    benchmark::DoNotOptimize(v);
    benchmark::DoNotOptimize(itBegin);
    benchmark::DoNotOptimize(itEnd);

    for (auto _ : state)
    {
        for (auto it = itBegin; it != itEnd; ++it)
        {
            benchmark::DoNotOptimize(*it);
            benchmark::ClobberMemory();
        }
    }
}
BENCHMARK(BM_VectorItAccess1D)->Unit(TIME_UNIT)->Arg(1e6);

static void BM_BufferShiftLeft(benchmark::State& state)
{
    TestBuffer<1> b(state.range(0));

    benchmark::DoNotOptimize(b);

    for (auto _ : state)
    {
        TestBuffer<1> b2 = b << 100;
        benchmark::DoNotOptimize(b2);
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_BufferShiftLeft)->Unit(TIME_UNIT)->Arg(1e6);

static void BM_VectorShiftLeft(benchmark::State& state)
{
    std::vector<test_data_t> v(state.range(0));

    benchmark::DoNotOptimize(v);

    for (auto _ : state)
    {
        std::vector<test_data_t> v2(v.size());
        std::copy(v.begin() + 100, v.end(), v2.begin());
        benchmark::DoNotOptimize(v2);
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_VectorShiftLeft)->Unit(TIME_UNIT)->Arg(1e6);

static void BM_BufferShiftRight(benchmark::State& state)
{
    TestBuffer<1> b(state.range(0));

    benchmark::DoNotOptimize(b);

    for (auto _ : state)
    {
        TestBuffer<1> b2 = b >> 100;
        benchmark::DoNotOptimize(b2);
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_BufferShiftRight)->Unit(TIME_UNIT)->Arg(1e6);

static void BM_VectorShiftRight(benchmark::State& state)
{
    std::vector<test_data_t> v(state.range(0));

    benchmark::DoNotOptimize(v);

    for (auto _ : state)
    {
        std::vector<test_data_t> v2(v.size());
        std::copy(v.begin(), v.end() - 100, v2.begin() + 100);
        benchmark::DoNotOptimize(v2);
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_VectorShiftRight)->Unit(TIME_UNIT)->Arg(1e6);

static void BM_BufferSubBuffer(benchmark::State& state)
{
    TestBuffer<1> b(state.range(0));

    benchmark::DoNotOptimize(b);

    for (auto _ : state)
    {
        TestBuffer<1> sb = b.SubBuffer(100, state.range(0) - 100);
        benchmark::DoNotOptimize(sb);
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_BufferSubBuffer)->Unit(TIME_UNIT)->Arg(1e6);

static void BM_VectorSubVector(benchmark::State& state)
{
    std::vector<test_data_t> v(state.range(0));

    benchmark::DoNotOptimize(v);

    for (auto _ : state)
    {
        std::vector<test_data_t> sv(v.begin() + 100, v.begin() + (state.range(0) - 100));
        benchmark::DoNotOptimize(sv);
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_VectorSubVector)->Unit(TIME_UNIT)->Arg(1e6);

static void BM_BufferPrepend(benchmark::State& state)
{
    TestBuffer<1> b1(state.range(0));

    benchmark::DoNotOptimize(b1);

    for (auto _ : state)
    {
        TestBuffer<1> b2(state.range(0));
        b2.Prepend(b1);
        benchmark::DoNotOptimize(b2);
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_BufferPrepend)->Unit(TIME_UNIT)->Arg(1e6);

static void BM_VectorPrepend(benchmark::State& state)
{
    std::vector<test_data_t> v1(state.range(0));

    benchmark::DoNotOptimize(v1);

    for (auto _ : state)
    {
        std::vector<test_data_t> v2(state.range(0));
        v2.insert(v2.begin(), v1.begin(), v1.end());
        benchmark::DoNotOptimize(v2);
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_VectorPrepend)->Unit(TIME_UNIT)->Arg(1e6);

static void BM_BufferAppend(benchmark::State& state)
{
    TestBuffer<1> b1(state.range(0));

    benchmark::DoNotOptimize(b1);

    for (auto _ : state)
    {
        TestBuffer<1> b2(state.range(0));
        b2.Append(b1);
        benchmark::DoNotOptimize(b2);
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_BufferAppend)->Unit(TIME_UNIT)->Arg(1e6);

static void BM_VectorAppend(benchmark::State& state)
{
    std::vector<test_data_t> v1(state.range(0));

    benchmark::DoNotOptimize(v1);

    for (auto _ : state)
    {
        std::vector<test_data_t> v2(state.range(0));
        v2.insert(v2.end(), v1.begin(), v1.end());
        benchmark::DoNotOptimize(v2);
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_VectorAppend)->Unit(TIME_UNIT)->Arg(1e6);

static void BM_BufferInsert(benchmark::State& state)
{
    TestBuffer<1> b1(state.range(0));

    benchmark::DoNotOptimize(b1);

    for (auto _ : state)
    {
        TestBuffer<1> b2(state.range(0));
        b2.Insert(b1, 100);
        benchmark::DoNotOptimize(b2);
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_BufferInsert)->Unit(TIME_UNIT)->Arg(1e6);

static void BM_VectorInsert(benchmark::State& state)
{
    std::vector<test_data_t> v1(state.range(0));

    benchmark::DoNotOptimize(v1);

    for (auto _ : state)
    {
        std::vector<test_data_t> v2(state.range(0));
        v2.insert(v2.begin() + 100, v1.begin(), v1.end());
        benchmark::DoNotOptimize(v2);
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_VectorInsert)->Unit(TIME_UNIT)->Arg(1e6);