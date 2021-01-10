#pragma once
#include <stdio.h>
#include <stdint.h>
#include "cuda_runtime.h"


const uint32_t k[64] = {
0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee ,
0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501 ,
0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be ,
0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821 ,
0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa ,
0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8 ,
0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed ,
0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a ,
0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c ,
0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70 ,
0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05 ,
0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665 ,
0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039 ,
0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1 ,
0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1 ,
0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391 };

const uint32_t r[64] = { 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
					  5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20,
					  4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
					  6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21 };


#define posLen 62
const unsigned char possible[posLen] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 
								'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 
								'y', 'z', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 
								'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 
								'W', 'X', 'Y', 'Z', '0', '1', '2', '3', '4', '5', '6', '7', 
								'8', '9' };

__constant__ unsigned char d_possible[posLen];
__constant__ uint32_t d_k[64];
__constant__ uint32_t d_r[64];

void md5_init() {
	cudaMemcpyToSymbol(d_k, k, sizeof(uint32_t) * 64, 0, cudaMemcpyHostToDevice);
	cudaMemcpyToSymbol(d_r, r, sizeof(uint32_t) * 64, 0, cudaMemcpyHostToDevice);
	cudaMemcpyToSymbol(d_possible, possible, sizeof(unsigned char) * posLen, 0, cudaMemcpyHostToDevice);
}

// leftrotate function definition
#define LEFTROTATE(x, c) (((x) << (c)) | ((x) >> (32 - (c))))

#define pwdbit 64

__device__ void encode(unsigned char output[], const unsigned int input[], unsigned int len)
{
    for (unsigned int i = 0, j = 0; j < len; i++, j += 4)
    {
        output[j] = (unsigned char)(input[i] & 0xff);
        output[j + 1] = (unsigned char)((input[i] >> 8) & 0xff);
        output[j + 2] = (unsigned char)((input[i] >> 16) & 0xff);
        output[j + 3] = (unsigned char)((input[i] >> 24) & 0xff);
    }
}

__device__ void to_bytes(unsigned char output[], const unsigned int input) {
	output[0] = (unsigned char)input;
	output[1] = (unsigned char)(input >> 8);
	output[2] = (unsigned char)(input >> 16);
	output[3] = (unsigned char)(input >> 24);
}

__device__ void decode(unsigned int output[], const unsigned char input[], unsigned int len) {
	for (int i = 0, j = 0; j < len; i++, j += 4) {
        output[i] = ((unsigned int)input[j]) | (((unsigned int)input[j + 1]) << 8) |
            (((unsigned int)input[j + 2]) << 16) | (((unsigned int)input[j + 3]) << 24);
    }
	output[14] = pwdbit;
}

extern "C" __global__ void Parallel_Hash(unsigned char* input, unsigned char* output, unsigned char* hit, unsigned char* pattern, int patternLen)
{
	int id = blockIdx.x * 32 * 32 + threadIdx.x;
	hit[id] = 0;

	uint32_t h0, h1, h2, h3;

	uint32_t w[16];
	uint32_t a, b, c, d, i, f, g, temp;
	
	unsigned char res[16];
	unsigned char h;

	unsigned char my_in[12];
	memcpy(my_in, &input[id*8], sizeof(unsigned char) * 8);
	my_in[8] = 0x80;
	my_in[9] = 0;
	my_in[10] = 0;
	my_in[11] = 0;

	memset(w, 0, sizeof(unsigned int) * 16);
	decode(w, my_in, 12);


	for (int ch = 0; ch < posLen; ch++) {
		my_in[7] = d_possible[ch];
		decode(w, my_in, 12);

		h0 = 0x67452301;
		h1 = 0xefcdab89;
		h2 = 0x98badcfe;
		h3 = 0x10325476;

		a = h0;
		b = h1;
		c = h2;
		d = h3;

		// Main loop:
		for (i = 0; i < 64; i++) {

			if (i < 16) {
				f = (b & c) | ((~b) & d);
				g = i;
			}
			else if (i < 32) {
				f = (d & b) | ((~d) & c);
				g = (5 * i + 1) % 16;
			}
			else if (i < 48) {
				f = b ^ c ^ d;
				g = (3 * i + 5) % 16;
			}
			else {
				f = c ^ (b | (~d));
				g = (7 * i) % 16;
			}

			temp = d;
			d = c;
			c = b;
			b = b + LEFTROTATE((a + f + d_k[i] + w[g]), d_r[i]);
			a = temp;
		}

		h0 += a;
		h1 += b;
		h2 += c;
		h3 += d;
		to_bytes(res, h0);
		to_bytes(res + 4, h1);
		to_bytes(res + 8, h2);
		to_bytes(res + 12, h3);

		h = 1;
		for (int j = 0; j < patternLen; j++) {
			if (res[j] != pattern[j]) {
				h = 0;
			}
		}
		if (h == 1) {
			hit[id] = 1;
			for (int j = 0; j < 8; j++) {
				output[j] = my_in[j];
			}
		}

	}
}