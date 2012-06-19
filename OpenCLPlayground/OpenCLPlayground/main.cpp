#include <stdio.h>
#include <stdlib.h>

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

#include <math.h>
#include <mach/mach_time.h>
#include <stdint.h>

#define MAX_SOURCE_SIZE (0x100000)

typedef struct 
{
    cl_float3 vertices[3];
} Triangle;

const int REPEAT_COUNT = 100000;

static Triangle _icosahedronTriangles[20 * REPEAT_COUNT];

void makeIcosahedron()
{
    const float X = 0.525731112119133606f;
    const float Z = 0.850650808352039932f;
    
    static GLfloat vdata[12][3] = 
    {    
        {-X, 0.0, Z}, {X, 0.0, Z}, {-X, 0.0, -Z}, {X, 0.0, -Z},    
        {0.0, Z, X}, {0.0, Z, -X}, {0.0, -Z, X}, {0.0, -Z, -X},    
        {Z, X, 0.0}, {-Z, X, 0.0}, {Z, -X, 0.0}, {-Z, -X, 0.0} 
    };
    
    static GLint tindices[20][3] = 
    { 
        {0,4,1}, {0,9,4}, {9,5,4}, {4,5,8}, {4,8,1},    
        {8,10,1}, {8,3,10}, {5,3,8}, {5,2,3}, {2,7,3},    
        {7,10,3}, {7,6,10}, {7,11,6}, {11,0,6}, {0,1,6}, 
        {6,1,10}, {9,0,11}, {9,11,2}, {9,2,5}, {7,2,11} 
    };
    
    for (int r = 0; r < REPEAT_COUNT; r++)
    {
        for (int i = 0; i < 20; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                for (int k = 0; k < 3; k++)
                {
                    _icosahedronTriangles[i + 20 * r].vertices[j].s[k] = vdata[tindices[i][j]][k];
                }
            }
        }
    }
}

cl_float3 sub(cl_float3 a, cl_float3 b)
{
    cl_float3 v;
    v.x = a.x - b.x;
    v.y = a.y - b.y;
    v.z = a.z - b.z;
    return v;
}

cl_float3 cross(cl_float3 a, cl_float3 b)
{
    cl_float3 n;
    n.x = (a.y * b.z) - (a.z * b.y);
    n.y = (a.z * b.x) - (a.x * b.z);
    n.z = (a.x * b.y) - (a.y * b.x);
    return n;
}

cl_float3 normalize(cl_float3 v)
{
    float length = v.x * v.x + v.y * v.y + v.z * v.z;
    float oneOverLength = 1.0f / sqrtf(length);
    v.x *= oneOverLength;
    v.y *= oneOverLength;
    v.z *= oneOverLength;
    return v;
}

typedef void(^action)();

double measureBlockInMilliseconds(action block)
{
    static double ticksToNanoseconds = 0.0;
    
    uint64_t startTime = mach_absolute_time( );
    
    // Do some stuff you want to time here
    block();
    
    uint64_t endTime = mach_absolute_time( );
    
    // Elapsed time in mach time units
    uint64_t elapsedTime = endTime - startTime;
    
    // The first time we get here, ask the system
    // how to convert mach time units to nanoseconds
    if (0.0 == ticksToNanoseconds) 
    {
        mach_timebase_info_data_t timebase;
        // to be completely pedantic, check the return code of this next call.
        mach_timebase_info(&timebase);
        ticksToNanoseconds = (double)timebase.numer / timebase.denom;
    }
    
    double elapsedTimeInNanoseconds = elapsedTime * ticksToNanoseconds;
    double elapsedTimeInMilliseconds = elapsedTimeInNanoseconds / 1000000.0;
    return elapsedTimeInMilliseconds;
}

double computeNormalsReference(Triangle *triangles, cl_float3 *normals, int trianglesCount)
{
    return measureBlockInMilliseconds(^
    {
        for (int i = 0; i < trianglesCount; i++)
        {
            cl_float3 u = sub(triangles[i].vertices[0], triangles[i].vertices[1]);
            cl_float3 v = sub(triangles[i].vertices[1], triangles[i].vertices[2]);
            normals[i] = normalize(cross(u, v));
        };
    });
}

double computeNormalsOpenCL(Triangle *triangles, cl_float3 *normals, int trianglesCount)
{
    cl_device_id device_id = NULL;
    cl_context context = NULL;
    cl_command_queue commands = NULL;
    cl_program program = NULL;
    cl_kernel kernel = NULL;
    cl_platform_id platform_id = NULL;
    cl_uint num_devices;
    cl_uint num_platforms;
    
    FILE *fp;
    char fileName[] = "./kernel.cl";
    char *source_str;
    size_t source_size;
    
    fp = fopen(fileName, "r");
    if (!fp) 
    {
        fprintf(stderr, "Failed to load kernel.\n");
        exit(1);
    }
    
    source_str = (char*)malloc(MAX_SOURCE_SIZE);
    source_size = fread(source_str, 1, MAX_SOURCE_SIZE, fp);
    fclose(fp);
    
    clGetPlatformIDs(1, &platform_id, &num_platforms);
    clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, 1, &device_id, &num_devices);
    
    context = clCreateContext(NULL, 1, &device_id, NULL, NULL, NULL);
    commands = clCreateCommandQueue(context, device_id, 0, NULL);
    
    program = clCreateProgramWithSource(context, 1, (const char **)&source_str, (const size_t *)&source_size, NULL);
    
    clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);
    kernel = clCreateKernel(program, "computeNormals", NULL);
    
    cl_mem memTriangles = clCreateBuffer(context,  CL_MEM_READ_ONLY,  sizeof(Triangle) * trianglesCount, NULL, NULL);
    cl_mem memNormals = clCreateBuffer(context,  CL_MEM_WRITE_ONLY,  sizeof(cl_float3) * trianglesCount, NULL, NULL);
    
    clSetKernelArg(kernel, 0, sizeof(cl_mem), &memTriangles);
    clSetKernelArg(kernel, 1, sizeof(cl_mem), &memNormals);
    
    size_t global = trianglesCount;
    
    double elapsedMs = measureBlockInMilliseconds(^
    {
        clEnqueueWriteBuffer(commands, memTriangles, CL_TRUE, 0, sizeof(Triangle) * trianglesCount, triangles, 0, NULL, NULL);
        clEnqueueNDRangeKernel(commands, kernel, 1, NULL, &global, NULL, 0, NULL, NULL);
        clFinish(commands);
        clEnqueueReadBuffer(commands, memNormals, CL_TRUE, 0, sizeof(cl_float3) * trianglesCount, normals, 0, NULL, NULL);
    });
    
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseMemObject(memTriangles);
    clReleaseMemObject(memNormals);
    clReleaseCommandQueue(commands);
    clReleaseContext(context);
    
    free(source_str);
    
    return elapsedMs;
}

void printNormals(cl_float3 *normals, int trianglesCount)
{
    for (int i = 0; i < trianglesCount; i++)
    {
        printf("normals[%i] = { x = %.3f\t\ty = %.3f\t\tz = %.3f }\n", i, normals[i].x, normals[i].y, normals[i].z);
    }
}

static cl_float3 _normalsReference[20 * REPEAT_COUNT];
static cl_float3 _normalsOpenCL[20 * REPEAT_COUNT];

int main()
{
    makeIcosahedron();
        
    double elapsedReference = computeNormalsReference(_icosahedronTriangles, _normalsReference, 20 * REPEAT_COUNT);
    double elapsedOpenCL = computeNormalsOpenCL(_icosahedronTriangles, _normalsOpenCL, 20 * REPEAT_COUNT);
    
    printf("--- Reference: %.3f ms ---\n", elapsedReference);
    printNormals(_normalsReference, 20);
    printf("\n***  OpenCL: %.3f ms ***\n", elapsedOpenCL);
    printNormals(_normalsOpenCL, 20);
    
    return 0;
}

