#include <iostream>
#include <immintrin.h>
#include <emmintrin.h> // especially for storing 128 bit registers
#include <iomanip>
#include <chrono>
#include <random>
using namespace std;
/*

COMPUTE DOT PRODUCT OF TWO VECTORS
INPUTS :n => dimension of vectors
        vec1 => the first vector
        vec2 => the second vector

OUTPUT:dotProduct = vec1.vec2

dotProduct of two vectors is defined as summation_{i=0}^{i=n} [vec1[i]*vec2[i]];


The goal of this file is to help y'all with syntax of SIMD instructions

NOTE : Do not use SIMD instructions without the 'u' (eg: Use _mm256_loadu_ps instead of _mm256_load_ps), the 'u' stands for unaligned 
and while it is technically slower, it is a lot less verbose in terms of syntax. Also only use SIMD instructions with floats, 
although they can work with integers as well, working with integers requires alignment which while faster, is a bit more complicated
Use above note only for load and store instructions

The format is as follows:
<Instruction/Code> | <Input parameters/types, NA for none> | <Output parameters/types, NA for none> | <Usage> | <Explanation> | <Additional Comments, if any>
__m256 | NA | NA | __m256 <Name> | Declares a 256 bit register with the name of <Name>
__m128 | NA | NA | __m128 <Name> | Declares a 128 bit register with the name of <Name>
_mm256_loadu_ps | float * | __m256 | __m256 <Name> = _mm256_loadu_ps(vec) | Assuming vec is a float*, loads vec[0],vec[1],vec[2]..vec[7] into the 256 bit register by name of <Name>
_mm256_add_ps | __m256,__m256 | __m256 | __m256 <Name> = _mm256_add_ps(a,b) | Performs <Name>[i] = a[i] + b[i] for i ranging from 0 to 7 | Similar for _mm256_sub_ps, _mm256_mul_ps, _mm256_div_ps, _mm256_max_ps, _mm256_min_ps
_mm256_storeu_ps | float *,__m256 | NA | _mm256_storeu_ps(a,b) | Performs b[i] = a[i] where a is a 256 bit register whereas b is float* from i = 0 to 7
_mm256_extractf128_ps | __m256,int | __m128 | __m128 <Name> = _mm256_extractf128_ps(a,0) OR __m128 <Name> = _mm256_extractf128_ps(a,1) | if int is 1, extracts the upper 128 bits and store into <Name>, if int is 0, extract lower 128 bits and store into <Name>
_mm_hadd_ps | __m128,__m128 | __m128 | __m128 <Name> = _mm_hadd_ps(a,b) | <Name>[0] = a[0] + a[1], <Name>[1] = a[2] + a[3], <Name>[2] = b[0] + b[1], <Name>[3] = b[2] + b[3] | Why might this be useful?
_mm_cvtss_f32 | __m128 | float | float <Name> = _mm_cvtss_f32(a) | <Name> = a[0]. May not be available on all machines

Those are the instructions that I believe will be relevant to this part. Note there are couple of other interesting instructions as well,
such as the _mm_mask instructions, I highly recommend you google search or chatGPT them to understand them.

Here is a hint for computing dot product using SIMD instructions.
Multiply normally
Extract upper and lower 128 bits into two registers and then use the hadd instruction to obtain a[0] + a[1] + a[2] + a[3] given a

*/

int getRandomNumber(int min, int max) {
    static std::random_device rd; // Initialize random device
    static std::mt19937 gen(rd()); // Initialize Mersenne Twister pseudo-random number generator
    std::uniform_int_distribution<int> dist(min, max); // Define distribution for integers in [min, max]
    return dist(gen); // Generate and return a random number
}


int main () {

    __m256 v1,v2,v3;
    __m128 a,b,c;
    //__m128 xmm_data;
    float buffer[4];

    int n;
    cin>>n;
    float vec1[n];
    float vec2[n];
    for (int i=0; i<n; i++) {
        vec1[i]=getRandomNumber(1,20);
        vec2[i]=getRandomNumber(1,20);
    }
    // for (int i=0; i<n; i++) {
    //     cin>>vec2[i];
    // }
    float naiveTimeTaken = 0;
    float SIMDTimeTaken = 0;
    float dotProduct = 0;
    for (int i=0; i<1000; i++) {
        dotProduct = 0;
        auto start = chrono::high_resolution_clock::now();
        //NAIVE IMPLEMENTATION FOR COMPARISION PURPOSES
        for (int i=0; i<n; i++) {
            dotProduct += vec1[i]*vec2[i];
        }
        //NAIVE IMPLEMENTATION ENDS
        auto end = chrono::high_resolution_clock::now();
        auto elapsed = chrono::duration_cast<chrono::duration<double>>(end - start);
        naiveTimeTaken += (elapsed.count())/1000;
    }

    cout<<dotProduct<<" "<<naiveTimeTaken<<"\n";
    
    for (int i=0; i<1000; i++) {
        dotProduct = 0;
        auto start2 = chrono::high_resolution_clock::now();
        //STUDENT CODE BEGINS HERE
        //cout<<"STUDENT CODE NOT IMPLEMENTED!\n";
        //exit(-1);
        if (n<=8){
            for (int i=0; i<n; i++) {
                dotProduct += vec1[i]*vec2[i];
            }
        }

        else {
            for (int i = 0 ; i < n - 7 ; i+=8){
                v1 = _mm256_loadu_ps(vec1+i); 
                v2 = _mm256_loadu_ps(vec2+i); 
                v3 = _mm256_mul_ps(v1,v2); 
                
                a = _mm256_extractf128_ps(v3,0);
                b = _mm256_extractf128_ps(v3,1);
                c = _mm_hadd_ps(a,b);
                
                //xmm_data = _mm_load_si128(c);
                _mm_store_ps(buffer, c);
            
                for (int j = 0; j<4 ; j++){
                    // dotProduct += ((_mm_cvtss_f32(c >> 32*j)) & 0xFFFFFFFF)  
                    dotProduct += buffer[j];
                }
            }

            for (int k = n - (n % 8) ; k<n; k++) {
                dotProduct += vec1[k]*vec2[k];
            }
        }
        //END OF STUDENT CODE
        auto end2 = chrono::high_resolution_clock::now();
        auto elapsed2 = chrono::duration_cast<chrono::duration<double>>(end2 - start2);
        SIMDTimeTaken += (elapsed2.count())/1000;
    }
    cout<<dotProduct<<" "<<SIMDTimeTaken<<"\n";
}

// THIS CODE WORKS WELL AND I AM GETTING ~ 2X SPEED UP FOR HIGHER(~10^3) VALUES OF n.
// Eg: 
// n = 1000
// 111788 5.16501e-06    -> Naive
// 111788 2.53909e-06    -> SIMD

// n = 1200
// 132075 2.9147e-06
// 132075 1.46058e-06

// n = 1500
// 161951 3.44119e-06
// 161951 1.8622e-06

// n = 2000
// 227056 8.02998e-06
// 227056 2.65337e-06

// n = 5000
// 547216 1.82669e-05
// 547216 5.23737e-06

// n = 10000
// 1.10708e+06 2.34858e-05
// 1.10708e+06 9.94992e-06