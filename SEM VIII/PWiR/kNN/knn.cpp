/* ============================================================

Copyright (c) 2009 Advanced Micro Devices, Inc.  All rights reserved.
 
Redistribution and use of this material is permitted under the following 
conditions:
 
Redistributions must retain the above copyright notice and all terms of this 
license.
 
In no event shall anyone redistributing or accessing or using this material 
commence or participate in any arbitration or legal action relating to this 
material against Advanced Micro Devices, Inc. or any copyright holders or 
contributors. The foregoing shall survive any expiration or termination of 
this license or any agreement or access or use related to this material. 

ANY BREACH OF ANY TERM OF THIS LICENSE SHALL RESULT IN THE IMMEDIATE REVOCATION 
OF ALL RIGHTS TO REDISTRIBUTE, ACCESS OR USE THIS MATERIAL.

THIS MATERIAL IS PROVIDED BY ADVANCED MICRO DEVICES, INC. AND ANY COPYRIGHT 
HOLDERS AND CONTRIBUTORS "AS IS" IN ITS CURRENT CONDITION AND WITHOUT ANY 
REPRESENTATIONS, GUARANTEE, OR WARRANTY OF ANY KIND OR IN ANY WAY RELATED TO 
SUPPORT, INDEMNITY, ERROR FREE OR UNINTERRUPTED OPERA TION, OR THAT IT IS FREE 
FROM DEFECTS OR VIRUSES.  ALL OBLIGATIONS ARE HEREBY DISCLAIMED - WHETHER 
EXPRESS, IMPLIED, OR STATUTORY - INCLUDING, BUT NOT LIMITED TO, ANY IMPLIED 
WARRANTIES OF TITLE, MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, 
ACCURACY, COMPLETENESS, OPERABILITY, QUALITY OF SERVICE, OR NON-INFRINGEMENT. 
IN NO EVENT SHALL ADVANCED MICRO DEVICES, INC. OR ANY COPYRIGHT HOLDERS OR 
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, PUNITIVE,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, REVENUE, DATA, OR PROFITS; OR 
BUSINESS INTERRUPTION) HOWEVER CAUSED OR BASED ON ANY THEORY OF LIABILITY 
ARISING IN ANY WAY RELATED TO THIS MATERIAL, EVEN IF ADVISED OF THE POSSIBILITY 
OF SUCH DAMAGE. THE ENTIRE AND AGGREGATE LIABILITY OF ADVANCED MICRO DEVICES, 
INC. AND ANY COPYRIGHT HOLDERS AND CONTRIBUTORS SHALL NOT EXCEED TEN DOLLARS 
(US $10.00). ANYONE REDISTRIBUTING OR ACCESSING OR USING THIS MATERIAL ACCEPTS 
THIS ALLOCATION OF RISK AND AGREES TO RELEASE ADVANCED MICRO DEVICES, INC. AND 
ANY COPYRIGHT HOLDERS AND CONTRIBUTORS FROM ANY AND ALL LIABILITIES, 
OBLIGATIONS, CLAIMS, OR DEMANDS IN EXCESS OF TEN DOLLARS (US $10.00). THE 
FOREGOING ARE ESSENTIAL TERMS OF THIS LICENSE AND, IF ANY OF THESE TERMS ARE 
CONSTRUED AS UNENFORCEABLE, FAIL IN ESSENTIAL PURPOSE, OR BECOME VOID OR 
DETRIMENTAL TO ADVANCED MICRO DEVICES, INC. OR ANY COPYRIGHT HOLDERS OR 
CONTRIBUTORS FOR ANY REASON, THEN ALL RIGHTS TO REDISTRIBUTE, ACCESS OR USE 
THIS MATERIAL SHALL TERMINATE IMMEDIATELY. MOREOVER, THE FOREGOING SHALL 
SURVIVE ANY EXPIRATION OR TERMINATION OF THIS LICENSE OR ANY AGREEMENT OR 
ACCESS OR USE RELATED TO THIS MATERIAL.

NOTICE IS HEREBY PROVIDED, AND BY REDISTRIBUTING OR ACCESSING OR USING THIS 
MATERIAL SUCH NOTICE IS ACKNOWLEDGED, THAT THIS MATERIAL MAY BE SUBJECT TO 
RESTRICTIONS UNDER THE LAWS AND REGULATIONS OF THE UNITED STATES OR OTHER 
COUNTRIES, WHICH INCLUDE BUT ARE NOT LIMITED TO, U.S. EXPORT CONTROL LAWS SUCH 
AS THE EXPORT ADMINISTRATION REGULATIONS AND NATIONAL SECURITY CONTROLS AS 
DEFINED THEREUNDER, AS WELL AS STATE DEPARTMENT CONTROLS UNDER THE U.S. 
MUNITIONS LIST. THIS MATERIAL MAY NOT BE USED, RELEASED, TRANSFERRED, IMPORTED,
EXPORTED AND/OR RE-EXPORTED IN ANY MANNER PROHIBITED UNDER ANY APPLICABLE LAWS, 
INCLUDING U.S. EXPORT CONTROL LAWS REGARDING SPECIFICALLY DESIGNATED PERSONS, 
COUNTRIES AND NATIONALS OF COUNTRIES SUBJECT TO NATIONAL SECURITY CONTROLS. 
MOREOVER, THE FOREGOING SHALL SURVIVE ANY EXPIRATION OR TERMINATION OF ANY 
LICENSE OR AGREEMENT OR ACCESS OR USE RELATED TO THIS MATERIAL.

NOTICE REGARDING THE U.S. GOVERNMENT AND DOD AGENCIES: This material is 
provided with "RESTRICTED RIGHTS" and/or "LIMITED RIGHTS" as applicable to 
computer software and technical data, respectively. Use, duplication, 
distribution or disclosure by the U.S. Government and/or DOD agencies is 
subject to the full extent of restrictions in all applicable regulations, 
including those found at FAR52.227 and DFARS252.227 et seq. and any successor 
regulations thereof. Use of this material by the U.S. Government and/or DOD 
agencies is acknowledgment of the proprietary rights of any copyright holders 
and contributors, including those of Advanced Micro Devices, Inc., as well as 
the provisions of FAR52.227-14 through 23 regarding privately developed and/or 
commercial computer software.

This license forms the entire agreement regarding the subject matter hereof and 
supersedes all proposals and prior discussions and writings between the parties 
with respect thereto. This license does not affect any ownership, rights, title,
or interest in, or relating to, this material. No terms of this license can be 
modified or waived, and no breach of this license can be excused, unless done 
so in a writing signed by all affected parties. Each term of this license is 
separately enforceable. If any term of this license is determined to be or 
becomes unenforceable or illegal, such term shall be reformed to the minimum 
extent necessary in order for this license to remain in effect in accordance 
with its terms as modified by such reformation. This license shall be governed 
by and construed in accordance with the laws of the State of Texas without 
regard to rules on conflicts of law of any state or jurisdiction or the United 
Nations Convention on the International Sale of Goods. All disputes arising out 
of this license shall be subject to the jurisdiction of the federal and state 
courts in Austin, Texas, and all defenses are hereby waived concerning personal 
jurisdiction and venue of these courts.

============================================================ */


#include "knn.hpp"

int initializeHost(const char* file) {
  FILE* input_file = fopen(file, "r");

  // Read bound vars
  fscanf(input_file, "%d %d %d %d %d", &n, &d, &l, &q, &k);

  global_threads = ALIGNED(q, local_threads);

  // Allocate memory used by host
  input_points_size = sizeof(cl_float) * (d + 1) * n;
  input_points = (cl_float*) malloc(input_points_size);
  if (input_points == NULL)
    std::cout << "Error: `input_points` malloc\n";

  input_query_size = sizeof(cl_float) * d * global_threads;
  input_query = (cl_float*) malloc(input_query_size);
  if (input_query == NULL)
    std::cout << "Error: `input_query` malloc\n";

  output_query_size = sizeof(cl_int) * global_threads;
  output_query = (cl_int*) malloc(output_query_size);
  if (output_query == NULL)
    std::cout << "Error: `output_query` malloc\n";

  // Read input data into memory
  for (cl_uint i = 0; i < n; i++) {
    cl_uint offset = i * (d + 1);
    fscanf(input_file, "%d", (cl_int*) &input_points[offset + d]);
    for (cl_uint j = 0; j < d; j++) {
      fscanf(input_file, "%f", &input_points[offset + j]);
    }
  }

  for (cl_uint i = 0; i < q; i++) {
    for (cl_uint j = 0; j < d; j++) {
      fscanf(input_file, "%f", &input_query[i * d + j]);
    }
  }

  fclose(input_file);

  return 0;
}


std::string convertToString(const char *filename) {
  size_t size;
  char*  str;
  std::string s;

  std::fstream f(filename, (std::fstream::in | std::fstream::binary));

  if(f.is_open()) {
    size_t fileSize;
    f.seekg(0, std::fstream::end);
    size = fileSize = (size_t)f.tellg();
    f.seekg(0, std::fstream::beg);

    str = new char[size+1];
    if (!str) {
      f.close();
      return NULL;
    }

    f.read(str, fileSize);
    f.close();
    str[size] = '\0';
  
    s = str;
    delete[] str;
    return s;
  } else {
    std::cout << "\nFile containg the kernel code(\".cl\") not found. " <<
        "Please copy the required file in the folder containg the executable.\n";
    exit(1);
  }
  return NULL;
}


int initializeCL(void) {
  cl_int status = 0;
  size_t deviceListSize;

  /*
   * Have a look at the available platforms and pick either
   * the AMD one if available or a reasonable default.
   */

  cl_uint numPlatforms;
  cl_platform_id platform = NULL;
  status = clGetPlatformIDs(0, NULL, &numPlatforms);
  if (status != CL_SUCCESS) {
    std::cout << "Error: Getting Platforms. (clGetPlatformsIDs)\n";
    return 1;
  }
  
  if(numPlatforms > 0) {
    cl_platform_id* platforms = new cl_platform_id[numPlatforms];
    status = clGetPlatformIDs(numPlatforms, platforms, NULL);
    if (status != CL_SUCCESS) {
      std::cout << "Error: Getting Platform Ids. (clGetPlatformsIDs)\n";
      return 1;
    }
    for (unsigned int i=0; i < numPlatforms; ++i) {
      char pbuff[100];
      status = clGetPlatformInfo(platforms[i], CL_PLATFORM_VENDOR,
          sizeof(pbuff), pbuff, NULL);
      if (status != CL_SUCCESS) {
        std::cout << "Error: Getting Platform Info.(clGetPlatformInfo)\n";
        return 1;
      }
      platform = platforms[i];
      if (!strcmp(pbuff, "NVIDIA Corporation")) {
         break;
      }
    }
    delete platforms;
  }

  if (NULL == platform) {
    std::cout << "NULL platform found so Exiting Application." << std::endl;
    return 1;
  }

  /*
   * If we could find our platform, use it. Otherwise use just available platform.
   */
  cl_context_properties cps[3] = { CL_CONTEXT_PLATFORM, (cl_context_properties)platform, 0 };

  /////////////////////////////////////////////////////////////////
  // Create an OpenCL context
  /////////////////////////////////////////////////////////////////
  context = clCreateContextFromType(cps, CL_DEVICE_TYPE_GPU, NULL, NULL,
      &status);
  if(status != CL_SUCCESS) {  
    std::cout << "Error: Creating Context. (clCreateContextFromType)\n";
    return 1; 
  }

  /* First, get the size of device list data */
  status = clGetContextInfo(context, CL_CONTEXT_DEVICES, 0, NULL,
      &deviceListSize);
  if (status != CL_SUCCESS) {  
    std::cout << "Error: Getting Context Info \
        (device list size, clGetContextInfo)\n";
    return 1;
  }

  /////////////////////////////////////////////////////////////////
  // Detect OpenCL devices
  /////////////////////////////////////////////////////////////////
  devices = (cl_device_id *) malloc(deviceListSize);
  if(devices == 0) {
    std::cout << "Error: No devices found.\n";
    return 1;
  }

  /* Now, get the device list data */
  status = clGetContextInfo(context, CL_CONTEXT_DEVICES, deviceListSize, devices, 
      NULL);
  if (status != CL_SUCCESS) { 
    std::cout << "Error: Getting Context Info \
        (device list, clGetContextInfo)\n";
    return 1;
  }

  /////////////////////////////////////////////////////////////////
  // Create an OpenCL command queue
  /////////////////////////////////////////////////////////////////
  commandQueue = clCreateCommandQueue(context, devices[0], 
      CL_QUEUE_PROFILING_ENABLE, &status);
  if (status != CL_SUCCESS) { 
    std::cout << "Creating Command Queue. (clCreateCommandQueue)\n";
    return 1;
  }

  /////////////////////////////////////////////////////////////////
  // Create OpenCL memory buffers
  /////////////////////////////////////////////////////////////////

  input_points_buffer = clCreateBuffer(context, CL_MEM_READ_WRITE,
      input_points_size, NULL, &status);
  if (status != CL_SUCCESS)
    std::cout << "Error: clCreateBuffer (input_points_buffer)";

  input_query_buffer = clCreateBuffer(context, CL_MEM_READ_WRITE,
      input_query_size, NULL, &status);
  if (status != CL_SUCCESS)
    std::cout << "Error: clCreateBuffer (input_query_buffer)";

  output_query_buffer = clCreateBuffer(context, CL_MEM_READ_WRITE,
      output_query_size, NULL, &status);
  if (status != CL_SUCCESS)
    std::cout << "Error: clCreateBuffer (output_query_buffer)";
  

  /////////////////////////////////////////////////////////////////
  // Load CL file, build CL program object, create CL kernel object
  /////////////////////////////////////////////////////////////////
  const char * filename  = "knn_kernels.cl";
  std::string  sourceStr = convertToString(filename);
  const char * source    = sourceStr.c_str();
  size_t sourceSize[]    = { strlen(source) };

  program = clCreateProgramWithSource(context, 1, &source, sourceSize, &status);
  if (status != CL_SUCCESS) { 
    std::cout << "Error: Loading Binary into cl_program \
         (clCreateProgramWithBinary)\n";
    return 1;
  }

  /* create a cl program executable for all the devices specified */
  std::stringstream ss;
  ss << "-D ARG_N=" << n << " ";
  ss << "-D ARG_K=" << k << " ";
  ss << "-D ARG_D=" << d << " ";
  std::string flags = ss.str();
  status = clBuildProgram(program, 1, devices, flags.c_str(), NULL, NULL);
  if (status != CL_SUCCESS) { 
    // check build error and build status first
    cl_build_status build_status;
    char* program_log;
    size_t log_size;

    clGetProgramBuildInfo(program, devices[0], CL_PROGRAM_BUILD_STATUS,
      sizeof(cl_build_status), &build_status, NULL);

    // check build log
    clGetProgramBuildInfo(program, devices[0],
	CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
    program_log = (char*) calloc (log_size + 1, sizeof(char));
    clGetProgramBuildInfo(program, devices[0],
	CL_PROGRAM_BUILD_LOG, log_size + 1, program_log, NULL);
    printf("Build failed; error=%d, status=%d, programLog:\n\n%s",
	status, build_status, program_log);
    free(program_log);

    return 1; 
  }

  /* get a kernel object handle for a kernel with the given name */
  kernel = clCreateKernel(program, kernel_name, &status);
  if (status != CL_SUCCESS) {  
    std::cout << "Error: Creating Kernel from program. (clCreateKernel)\n";
    return 1;
  }

  return 0;
}


int runCLKernels(void) {
  cl_int   status;
  cl_uint maxDims;
  cl_event events[4];
  size_t maxWorkGroupSize;
  size_t maxWorkItemSizes[3];
  cl_uint addressBits;


  /**
  * Query device capabilities. Maximum 
  * work item dimensions and the maximmum
  * work item sizes
  */ 
  status = clGetDeviceInfo(devices[0],
      CL_DEVICE_MAX_WORK_GROUP_SIZE,  // maximum number of work items in a work group
      sizeof(size_t), (void*) &maxWorkGroupSize, NULL);
  if (status != CL_SUCCESS) {  
    std::cout << "Error: Getting Device Info. (clGetDeviceInfo)\n";
    return 1;
  }
  
  status = clGetDeviceInfo(devices[0], 
      CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, // maximum number of dimensions
      sizeof(cl_uint), (void*) &maxDims,  NULL);
  if (status != CL_SUCCESS) {  
    std::cout << "Error: Getting Device Info. (clGetDeviceInfo)\n";
    return 1;
  }

  status = clGetDeviceInfo(devices[0], 
      CL_DEVICE_MAX_WORK_ITEM_SIZES, // maximum number of work items in each dimension of a WG
      sizeof(size_t) * maxDims, (void*) maxWorkItemSizes, NULL);
  if (status != CL_SUCCESS) {  
    std::cout << "Error: Getting Device Info. (clGetDeviceInfo)\n";
    return 1;
  }

    
  status = clGetDeviceInfo(devices[0],
      CL_DEVICE_ADDRESS_BITS, //maximum number of WI is bounded by 2^CL_DEVICE_ADDRESS_BITS
      sizeof(cl_uint), &addressBits, NULL);
  if (status != CL_SUCCESS) {
    std::cout << "Error: Getting Device Info. (clGetDeviceInfo)" << std::endl;
    return 1;
  }

  if (global_threads > ((unsigned long) 2 << addressBits)) {
    std::cout << "Unsupported: Device does not support requested number of " <<
        "global work items." << std::endl;
    return 1;
  }
  if (local_threads > maxWorkGroupSize || local_threads > maxWorkItemSizes[0]) {
    std::cout << "Unsupported: Device does not support requested number of " <<
        "work items in a work group." <<
        std::endl;
    return 1;
  }


  // load data
  status = clEnqueueWriteBuffer(commandQueue, input_query_buffer, CL_TRUE,
      0, input_query_size, input_query, 0, NULL, &events[2]);
  if (status != CL_SUCCESS) { 
      std::cout << "Error: clEnqueueWriteBuffer failed.\n";
      return 1;
  }
  
  status = clWaitForEvents(1, &events[2]);
  if (status != CL_SUCCESS) { 
    std::cout << "Error: Waiting for read buffer call to finish.\n";
    return 1;
  }
  clGetEventProfilingInfo(events[2], CL_PROFILING_COMMAND_START, sizeof(cl_ulong),
      &start_time_all, NULL);


  status = clReleaseEvent(events[2]);
  if (status != CL_SUCCESS) { 
    std::cout << "Error: Release event object.\n";
    return 1;
  }


  status = clEnqueueWriteBuffer(commandQueue, input_points_buffer, CL_TRUE,
      0, input_points_size, input_points, 0, NULL, &events[3]);
  if (status != CL_SUCCESS) { 
      std::cout << "Error: clEnqueueWriteBuffer failed.\n";
      return 1;
  }
  
  status = clWaitForEvents(1, &events[3]);
  if (status != CL_SUCCESS) { 
    std::cout << "Error: Waiting for read buffer call to finish.\n";
    return 1;
  }

  status = clReleaseEvent(events[3]);
  if (status != CL_SUCCESS) { 
    std::cout << "Error: Release event object.\n";
    return 1;
  }




  // Kernel Args
  status = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void*) &input_points_buffer);
  CHECK(status, "Setting kernel argument. (input_points)");

  status = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void*) &input_query_buffer);
  CHECK(status, "Setting kernel argument. (input_query)");

  status = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void*) &output_query_buffer);
  CHECK(status, "Setting kernel argument. (output_query)");


#ifdef PASS_CONSTANTS
  status = clSetKernelArg(kernel, 3, sizeof(cl_uint), (void*) &n);
  CHECK(status, "Setting kernel argument. (n)");

  status = clSetKernelArg(kernel, 4, sizeof(cl_uint), (void*) &d);
  CHECK(status, "Setting kernel argument. (d)");

  status = clSetKernelArg(kernel, 5, sizeof(cl_uint), (void*) &l);
  CHECK(status, "Setting kernel argument. (l)");

  status = clSetKernelArg(kernel, 6, sizeof(cl_uint), (void*) &k);
  CHECK(status, "Setting kernel argument. (k)");
#endif


  /* run kernel */
  status = clEnqueueNDRangeKernel(commandQueue, kernel, 1, // number of dimensions
      NULL, &global_threads, &local_threads, 0, NULL, &events[0]);
  if (status != CL_SUCCESS) {
    std::cout << "Error: Enqueueing kernel onto command queue. \
        (clEnqueueNDRangeKernel)\n";
    return 1;
  }


  /* wait for the kernel call to finish execution */
  status = clWaitForEvents(1, &events[0]);
  if (status != CL_SUCCESS) { 
    std::cout<< "Error: Waiting for kernel run to finish. \
        (clWaitForEvents)\n";
    return 1;
  }
  clGetEventProfilingInfo(events[0], CL_PROFILING_COMMAND_START, sizeof(cl_ulong),
      &start_time_kernel, NULL);
  clGetEventProfilingInfo(events[0], CL_PROFILING_COMMAND_END, sizeof(cl_ulong),
      &end_time_kernel, NULL);

  status = clReleaseEvent(events[0]);
  if (status != CL_SUCCESS) { 
    std::cout << "Error: Release event object. \
        (clReleaseEvent)\n";
    return 1;
  }

  /* Enqueue readBuffer*/
  status = clEnqueueReadBuffer(commandQueue, output_query_buffer, CL_TRUE,
      0, output_query_size, output_query, 0, NULL, &events[1]);
  if (status != CL_SUCCESS) { 
      std::cout << "Error: clEnqueueReadBuffer failed.\n";
      return 1;
  }
  
  /* Wait for the read buffer to finish execution */
  status = clWaitForEvents(1, &events[1]);
  if (status != CL_SUCCESS) { 
    std::cout << "Error: Waiting for read buffer call to finish.\n";
    return 1;
  }
  clGetEventProfilingInfo(events[1], CL_PROFILING_COMMAND_END, sizeof(cl_ulong),
      &end_time_all, NULL);

  
  status = clReleaseEvent(events[1]);
  if (status != CL_SUCCESS) { 
    std::cout << "Error: Release event object.\n";
    return 1;
  }


  std::cerr << (end_time_kernel - start_time_kernel) / 1000000 << std::endl;
  std::cerr << (end_time_all - start_time_all) / 1000000 << std::endl;

  return 0;
}


/*
 * \brief Release OpenCL resources (Context, Memory etc.) 
 */
int cleanupCL(void) {
  cl_int status;

  status = clReleaseKernel(kernel);
  if(status != CL_SUCCESS) {
    std::cout << "Error: In clReleaseKernel \n";
    return 1; 
  }

  status = clReleaseProgram(program);
  if(status != CL_SUCCESS) {
    std::cout << "Error: In clReleaseProgram\n";
    return 1; 
  }

  status = clReleaseMemObject(input_points_buffer);
  if(status != CL_SUCCESS) {
    std::cout << "Error: In clReleaseMemObject (input_points_buffer)\n";
    return 1; 
  }

  status = clReleaseMemObject(input_query_buffer);
  if(status != CL_SUCCESS) {
    std::cout << "Error: In clReleaseMemObject (input_query_buffer)\n";
    return 1; 
  }

  status = clReleaseMemObject(output_query_buffer);
  if(status != CL_SUCCESS) {
    std::cout << "Error: In clReleaseMemObject (output_query_buffer)\n";
    return 1; 
  }

  status = clReleaseCommandQueue(commandQueue);
  if(status != CL_SUCCESS) {
    std::cout << "Error: In clReleaseCommandQueue\n";
    return 1;
  }

  status = clReleaseContext(context);
  if(status != CL_SUCCESS) {
    std::cout << "Error: In clReleaseContext\n";
    return 1;
  }

  return 0;
}


void cleanupHost(void) {
  if (input_points != NULL) {
    free(input_points);
    input_points = NULL;
  }

  if (input_query != NULL) {
    free(input_query);
    input_query = NULL;
  }

  if (output_query != NULL) {
    free(output_query);
    output_query = NULL;
  }
}


void printOutput(const char* file) {
  FILE* fp = fopen(file, "w+");
  for (cl_uint i = 0; i < q; i++) {
    fprintf(fp, "%d\n", output_query[i]);
  }
  fclose(fp);
}


int main(int argc, char * argv[]) {
  if (argc < 3) {
    printf("Usage: %s Input Output", argv[0]);
    return 0;
  }

  // Initialize Host application 
  if (initializeHost(argv[1]) == 1)
    return 1;

  // Initialize OpenCL resources
  if (initializeCL() == 1)
    return 1;

  // Run the CL program
  if (runCLKernels() == 1)
    return 1;

  // Releases OpenCL resources 
  if (cleanupCL() == 1)
    return 1;

  // Print output
  printOutput(argv[2]);

  // Release host resources
  cleanupHost();

  return 0;
}
