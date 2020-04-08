#include <ArborX.hpp>

#include <Kokkos_Core.hpp>

#include <array>
#include <iostream>
#include <numeric>

struct PointCloud
{
  float *d_x;
  float *d_y;
  float *d_z;
  int N;
};

struct Spheres
{
  float *d_x;
  float *d_y;
  float *d_z;
  float *d_r;
  int N;
};

namespace ArborX
{
namespace Traits
{
template <>
struct Access<PointCloud, PrimitivesTag>
{
  inline static std::size_t size(PointCloud const &cloud) { return cloud.N; }
  KOKKOS_INLINE_FUNCTION static Point get(PointCloud const &cloud,
                                          std::size_t i)
  {
    return {{cloud.d_x[i], cloud.d_y[i], cloud.d_z[i]}};
  }
  using memory_space = Kokkos::CudaSpace;
};

template <>
struct Access<Spheres, PredicatesTag>
{
  inline static std::size_t size(Spheres const &d) { return d.N; }
  KOKKOS_INLINE_FUNCTION static auto get(Spheres const &d, std::size_t i)
  {
    return intersects(Sphere{{{d.d_x[i], d.d_y[i], d.d_z[i]}}, d.d_r[i]});
  }
  using memory_space = Kokkos::CudaSpace;
};
} // namespace Traits
} // namespace ArborX

int main(int argc, char *argv[])
{
  Kokkos::initialize(argc, argv);
  {

    constexpr std::size_t N = 10;
    std::array<float, N> a;

    float *d_a;
    cudaMalloc(&d_a, sizeof(a));

    std::iota(std::begin(a), std::end(a), 1.0);

    cudaMemcpy(d_a, a.data(), sizeof(a), cudaMemcpyHostToDevice);

    using MemorySpace = Kokkos::CudaSpace;
    using ExecutionSpace = Kokkos::Cuda;
    ArborX::BVH<MemorySpace> bvh{ExecutionSpace{},
                                 PointCloud{d_a, d_a, d_a, N}};

    Kokkos::View<int *, ExecutionSpace> indices("indices", 0);
    Kokkos::View<int *, ExecutionSpace> offset("offset", 0);
    bvh.query(ExecutionSpace{}, Spheres{d_a, d_a, d_a, d_a, N}, indices,
              offset);

    Kokkos::parallel_for(N, KOKKOS_LAMBDA(int i) {
      for (int j = offset(i); j < offset(i + 1); ++j)
      {
        printf("%i %i\n", i, indices(j));
      }
    });
  }
  Kokkos::finalize();

  return 0;
}
