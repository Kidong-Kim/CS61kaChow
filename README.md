# CS61kaChow
Efficient Convolution via HW/SW level Parallelism

In the project, the implementation utilizes SIMD instructions to achieve a maximum speedup of 8.05x compared to the serial implementation. SIMD instructions allow for parallel processing of multiple data elements in a single instruction, resulting in improved performance. By leveraging the capabilities of SIMD, the dot product computation is accelerated.

Additionally, the integration of MPI enables parallel computing across multiple processes or nodes, leading to a speedup of around 5.30x compared to the serial implementation. MPI allows for distributing the workload among multiple processors, facilitating concurrent execution and efficient utilization of available resources.

The combined effect of SIMD instructions and MPI results in significant performance enhancements for the matrix convolution task, demonstrating the benefits of parallelization and utilizing hardware-level optimizations
