
//
// include files
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include <vector>
#include <helper_cuda.h>
#include <sys/time.h>
#include <library_daq.h>

// CUDA = Computer Device Unified Architecture

__global__ void kernel_correct_times(unsigned int *ct);




//
// main code
//

int main(int argc, const char **argv)
{


  /////////////////////
  // initialise card //
  /////////////////////
  findCudaDevice(argc, argv);


  // initialise CUDA timing
  bool use_timing = true;
  if( use_timing ){
    cudaEventCreate(&start);
    cudaEventCreate(&stop);
  }
  cudaEventCreate(&total_start);
  cudaEventCreate(&total_stop);
  float elapsed_parameters = 0, elapsed_pmts = 0, elapsed_detector = 0, elapsed_verteces = 0,
    elapsed_threads = 0, elapsed_tof = 0, elapsed_memory_tofs_dev = 0, elapsed_memory_candidates_host = 0, elapsed_tofs_copy_dev = 0,
    elapsed_input = 0, elapsed_memory_dev = 0, elapsed_copy_dev = 0, elapsed_kernel = 0, 
    elapsed_threads_candidates = 0, elapsed_candidates_memory_dev = 0, elapsed_candidates_kernel = 0,
    elapsed_candidates_copy_host = 0, choose_candidates = 0, elapsed_coalesce = 0, elapsed_gates = 0, elapsed_free = 0, elapsed_total = 0,
    elapsed_tofs_free = 0, elapsed_reset = 0, elapsed_write_output = 0;
  use_verbose = true;


  ////////////////////
  // inspect device //
  ////////////////////
  // set: max_n_threads_per_block, max_n_blocks
  print_gpu_properties();




  ////////////////
  // read PMTs  //
  ////////////////
  // set: n_PMTs, PMT_x, PMT_y, PMT_z
  if( use_timing )
    start_c_clock();
  event_file_base = "all_hits_";
  event_file_suffix = ".txt";
  detector_file = "detector.txt";
  pmts_file = "all_pmts.txt";
  output_file_base = "all_hits_emerald_threshold_";
  if( !read_the_pmts() ) return 0;
  if( use_timing )
    elapsed_pmts = stop_c_clock();


  ///////////////////////
  // define parameters //
  ///////////////////////
  if( use_timing )
    start_c_clock();
  read_user_parameters();
  set_output_file();
  if( use_verbose ){
    printf(" --- user parameters \n");
    printf(" distance between test verteces = %f cm \n", distance_between_verteces);
    printf(" time step size = %d ns \n", time_step_size);
    printf(" threshold_number_of_pmts = %d \n", threshold_number_of_pmts);
    printf(" coalesce_time = %f ns \n", coalesce_time);
    printf(" trigger_gate_up = %f ns \n", trigger_gate_up);
    printf(" trigger_gate_down = %f ns \n", trigger_gate_down);
  }
  if( use_timing )
    elapsed_parameters = stop_c_clock();




  /////////////////////
  // read detector ////
  /////////////////////
  // set: detector_height, detector_radius, pmt_radius
  if( use_timing )
    start_c_clock();
  if( !read_the_detector() ) return 0;
  if( use_timing )
    elapsed_detector = stop_c_clock();




  ////////////////////////
  // make test verteces //
  ////////////////////////
  // set: n_test_verteces, vertex_x, vertex_y, vertex_z
  // use: detector_height, detector_radius
  if( use_timing )
    start_c_clock();
  make_test_verteces();
  if( use_timing )
    elapsed_verteces = stop_c_clock();



  //////////////////////////////
  // table of times_of_flight //
  //////////////////////////////
  // set: host_times_of_flight, time_offset
  // use: n_test_verteces, vertex_x, vertex_y, vertex_z, n_PMTs, PMT_x, PMT_y, PMT_z
  // malloc: host_times_of_flight
  if( use_timing )
    start_c_clock();
  make_table_of_tofs();
  if( use_timing )
    elapsed_tof = stop_c_clock();



  ////////////////////////////////////
  // allocate tofs memory on device //
  ////////////////////////////////////
  // use: n_test_verteces, n_PMTs
  // cudamalloc: device_times_of_flight
  if( use_timing )
    start_cuda_clock();
  allocate_tofs_memory_on_device();
  if( use_timing )
    elapsed_memory_tofs_dev = stop_cuda_clock();


  ////////////////////////////////
  // fill tofs memory on device //
  ////////////////////////////////
  // use: n_test_verteces, n_PMTs
  // memcpy: device_times_of_flight, constant_time_step_size, constant_n_test_verteces, constant_n_PMTs
  // texture: tex_times_of_flight
  if( use_timing )
    start_cuda_clock();
  fill_tofs_memory_on_device();
  if( use_timing )
    elapsed_tofs_copy_dev = stop_cuda_clock();


  ///////////////////////
  // initialize output //
  ///////////////////////
  initialize_output();



  start_total_cuda_clock();

  int n_events = 0;

  while( set_input_file_for_event(n_events) ){

    printf(" ------ analyzing event %d \n", n_events+1);

    ////////////////
    // read input //
    ////////////////
    // set: n_hits, host_ids, host_times, time_offset, n_time_bins
    // use: time_offset, n_test_verteces
    // memcpy: constant_n_time_bins, constant_n_hits
    if( use_timing )
      start_c_clock();
    if( !read_the_input() ) return 0;
    if( use_timing )
      elapsed_input += stop_c_clock();
  


    ////////////////////////////////////////
    // allocate candidates memory on host //
    ////////////////////////////////////////
    // use: n_time_bins, n_hits
    // malloc: host_max_number_of_pmts_in_time_bin, host_vertex_with_max_n_pmts
    if( use_timing )
      start_cuda_clock();
    allocate_candidates_memory_on_host();
    if( use_timing )
      elapsed_memory_candidates_host += stop_cuda_clock();


    ////////////////////////////////////////////////
    // set number of blocks and threads per block //
    ////////////////////////////////////////////////
    // set: number_of_kernel_blocks, number_of_threads_per_block
    // use: n_test_verteces, n_hits
    if( use_timing )
      start_c_clock();
    if( !setup_threads_for_tof_2d() ) return 0;
    if( use_timing )
      elapsed_threads += stop_c_clock();



    ///////////////////////////////////////
    // allocate correct memory on device //
    ///////////////////////////////////////
    // use: n_test_verteces, n_hits, n_time_bins
    // cudamalloc: device_ids, device_times, device_n_pmts_per_time_bin
    if( use_timing )
      start_cuda_clock();
    allocate_correct_memory_on_device();
    if( use_timing )
      elapsed_memory_dev += stop_cuda_clock();


    //////////////////////////////////////
    // copy input into device variables //
    //////////////////////////////////////
    // use: n_hits
    // memcpy: device_ids, device_times, constant_time_offset
    // texture: tex_ids, tex_times
    if( use_timing )
      start_cuda_clock();
    fill_correct_memory_on_device();
    if( use_timing )
      elapsed_copy_dev += stop_cuda_clock();



    ////////////////////
    // execute kernel //
    ////////////////////
    if( use_timing )
      start_cuda_clock();
    printf(" --- execute kernel \n");
    kernel_correct_times<<<number_of_kernel_blocks_3d,number_of_threads_per_block_3d>>>(device_n_pmts_per_time_bin);
    getLastCudaError("correct_kernel execution failed\n");
    if( use_timing )
      elapsed_kernel += stop_cuda_clock();



    //////////////////////////////////
    // setup threads for candidates //
    //////////////////////////////////
    // set: number_of_kernel_blocks, number_of_threads_per_block
    // use: n_time_bins
    if( use_timing )
      start_c_clock();
    if( !setup_threads_to_find_candidates() ) return 0;
    if( use_timing )
      elapsed_threads_candidates += stop_c_clock();



    //////////////////////////////////////////
    // allocate candidates memory on device //
    //////////////////////////////////////////
    // use: n_time_bins
    // cudamalloc: device_max_number_of_pmts_in_time_bin, device_vertex_with_max_n_pmts
    if( use_timing )
      start_cuda_clock();
    allocate_candidates_memory_on_device();
    if( use_timing )
      elapsed_candidates_memory_dev += stop_cuda_clock();



    /////////////////////////////////////
    // find candidates above threshold //
    /////////////////////////////////////
    if( use_timing )
      start_cuda_clock();
    if( use_verbose )
      printf(" --- execute candidates kernel \n");
    kernel_find_vertex_with_max_npmts_in_timebin<<<number_of_kernel_blocks,number_of_threads_per_block>>>(device_n_pmts_per_time_bin, device_max_number_of_pmts_in_time_bin, device_vertex_with_max_n_pmts);
    getLastCudaError("candidates_kernel execution failed\n");
    if( use_timing )
      elapsed_candidates_kernel += stop_cuda_clock();




    /////////////////////////////////////////
    // copy candidates from device to host //
    /////////////////////////////////////////
    // use: n_time_bins
    // memcpy: host_max_number_of_pmts_in_time_bin, host_vertex_with_max_n_pmts
    if( use_timing )
      start_cuda_clock();
    if( use_verbose )
      printf(" --- copy candidates from device to host \n");
    copy_candidates_from_device_to_host();
    if( use_timing )
      elapsed_candidates_copy_host += stop_cuda_clock();



    ///////////////////////////////////////
    // choose candidates above threshold //
    ///////////////////////////////////////
    if( use_timing )
      start_cuda_clock();
    if( use_verbose )
      printf(" --- choose candidates above threshold \n");
    choose_candidates_above_threshold();
    if( use_timing )
      choose_candidates = stop_cuda_clock();



    ///////////////////////
    // coalesce triggers //
    ///////////////////////
    if( use_timing )
      start_cuda_clock();
    coalesce_triggers();
    if( use_timing )
      elapsed_coalesce += stop_cuda_clock();




    //////////////////////////////////
    // separate triggers into gates //
    //////////////////////////////////
    if( use_timing )
      start_cuda_clock();
    separate_triggers_into_gates();
    if( use_timing )
      elapsed_gates += stop_cuda_clock();



    //////////////////
    // write output //
    //////////////////
    if( use_timing )
      start_cuda_clock();
    write_output();
    if( use_timing )
      elapsed_write_output += stop_cuda_clock();

    /////////////////////////////
    // deallocate event memory //
    /////////////////////////////
    if( use_timing )
      start_cuda_clock();
    if( use_verbose )
      printf(" --- deallocate memory \n");
    free_event_memories();
    if( use_timing )
      elapsed_free += stop_cuda_clock();

    n_events ++;

  }

  elapsed_total += stop_total_cuda_clock();


  printf(" ------ analyzed %d events \n", n_events);

  ///////////////////////
  // normalize timings //
  ///////////////////////
  if( use_timing ){
    elapsed_input /= n_events;
    elapsed_memory_candidates_host /= n_events;
    elapsed_threads /= n_events;
    elapsed_memory_dev /= n_events;
    elapsed_copy_dev /= n_events;
    elapsed_kernel /= n_events;
    elapsed_threads_candidates /= n_events;
    elapsed_candidates_memory_dev /= n_events;
    elapsed_candidates_kernel /= n_events;
    elapsed_candidates_copy_host /= n_events;
    elapsed_coalesce /= n_events;
    elapsed_gates /= n_events;
    elapsed_write_output /= n_events;
    elapsed_free /= n_events;
  }
  elapsed_total /= n_events;



  //////////////////////////////
  // deallocate global memory //
  //////////////////////////////
  if( use_timing )
    start_cuda_clock();
  if( use_verbose )
    printf(" --- deallocate tofs memory \n");
  free_global_memories();
  if( use_timing )
    elapsed_tofs_free = stop_cuda_clock();



  //////////////////
  // reset device //
  //////////////////
  // -- needed to flush the buffer which holds printf from each thread
  if( use_timing )
    start_cuda_clock();
  if( use_verbose )
    printf(" --- reset device \n");
  //  cudaDeviceReset();
  if( use_timing )
    elapsed_reset = stop_cuda_clock();



  //////////////////
  // print timing //
  //////////////////
  if( use_timing ){
    printf(" user parameters time : %f ms \n", elapsed_parameters);
    printf(" read pmts execution time : %f ms \n", elapsed_pmts);
    printf(" read detector execution time : %f ms \n", elapsed_detector);
    printf(" make test verteces execution time : %f ms \n", elapsed_verteces);
    printf(" setup threads candidates execution time : %f ms \n", elapsed_threads_candidates);
    printf(" make table of tofs execution time : %f ms \n", elapsed_tof);
    printf(" allocate tofs memory on device execution time : %f ms \n", elapsed_memory_tofs_dev);
    printf(" fill tofs memory on device execution time : %f ms \n", elapsed_tofs_copy_dev);
    printf(" deallocate tofs memory execution time : %f ms \n", elapsed_tofs_free);
    printf(" device reset execution time : %f ms \n", elapsed_reset);
    printf(" read input execution time : %f ms (%f) \n", elapsed_input, elapsed_input/elapsed_total);
    printf(" allocate candidates memory on host execution time : %f ms (%f) \n", elapsed_memory_candidates_host, elapsed_memory_candidates_host/elapsed_total);
    printf(" setup threads execution time : %f ms (%f) \n", elapsed_threads, elapsed_threads/elapsed_total);
    printf(" allocate memory on device execution time : %f ms (%f) \n", elapsed_memory_dev, elapsed_memory_dev/elapsed_total);
    printf(" fill memory on device execution time : %f ms (%f) \n", elapsed_copy_dev, elapsed_copy_dev/elapsed_total);
    printf(" correct kernel execution time : %f ms (%f) \n", elapsed_kernel, elapsed_kernel/elapsed_total);
    printf(" allocate candidates memory on device execution time : %f ms (%f) \n", elapsed_candidates_memory_dev, elapsed_candidates_memory_dev/elapsed_total);
    printf(" copy candidates to host execution time : %f ms (%f) \n", elapsed_candidates_copy_host, elapsed_candidates_copy_host/elapsed_total);
    printf(" choose candidates execution time : %f ms (%f) \n", choose_candidates, choose_candidates/elapsed_total);
    printf(" candidates kernel execution time : %f ms (%f) \n", elapsed_candidates_kernel, elapsed_candidates_kernel/elapsed_total);
    printf(" coalesce triggers execution time : %f ms (%f) \n", elapsed_coalesce, elapsed_coalesce/elapsed_total);
    printf(" separate triggers into gates execution time : %f ms (%f) \n", elapsed_gates, elapsed_gates/elapsed_total);
    printf(" write output execution time : %f ms (%f) \n", elapsed_write_output, elapsed_write_output/elapsed_total);
    printf(" deallocate memory execution time : %f ms (%f) \n", elapsed_free, elapsed_free/elapsed_total);
  }
  printf(" total execution time : %f ms \n", elapsed_total);


  return 1;
}



//
// kernel routine
// 

// __global__ identifier says it's a kernel function
__global__ void kernel_correct_times(unsigned int *ct){

  // get unique id for each thread in each block
  unsigned int tid_x = threadIdx.x + blockDim.x*blockIdx.x;
  unsigned int tid_y = threadIdx.y + blockDim.y*blockIdx.y;

  // map the two 2D indices to a single linear, 1D index
  int tid = tid_y * gridDim.x * blockDim.x + tid_x;

  // tid runs from 0 to n_test_verteces * n_hits:
  //      vertex 0           vertex 1       ...     vertex m
  // (hit 0, ..., hit n; hit 0, ..., hit n; ...; hit 0, ..., hit n);

  unsigned int vertex_index = (int)(tid/constant_n_hits);
  unsigned int hit_index = tid % constant_n_hits;

  //  printf(" tid %d tidx %d tidy %d v %d h %d \n", tid, tid_x, tid_y, vertex_index, hit_index);

  //    printf( " threadi %d blockdim %d blockid %d, tid %d, vertex_index %d, hit %d \n",
  //  	  threadIdx.x, blockDim.x, blockIdx.x, tid,
  //	  vertex_index, hit_index);

  // skip if thread is assigned to nonexistent vertex
  if( vertex_index >= constant_n_test_verteces ) return;

  // skip if thread is assigned to nonexistent hit
  if( hit_index >= constant_n_hits ) return;

  unsigned int vertex_block = constant_n_time_bins*vertex_index;

  unsigned int vertex_block2 = constant_n_PMTs*vertex_index;

  atomicAdd(&
	    ct[
	       device_get_time_index(
				     int(floor(
					       (tex1Dfetch(tex_times,hit_index)
						- tex1Dfetch(tex_times_of_flight,
							     device_get_distance_index(
										       tex1Dfetch(tex_ids,hit_index),
										       vertex_block2
										       )
							     )
						+ constant_time_offset)/constant_time_step_size
					       )
					 ),
				     vertex_block
				     )
	       ]
	    ,1);

  //  printf( " hit %d (nh %d) id %d t %d; vertex %d (nv %d) tof %f  %d \n", hit_index, constant_n_hits, ids[hit_index], t[hit_index], vertex_index, constant_n_test_verteces, tof, ct[time_index]);

  return;

}


