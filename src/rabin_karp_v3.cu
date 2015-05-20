/*
 * rabin_karp_v3.cpp
 * 
 * Created by Varun Pandey on 05-19-2015.
 * 
 * Copyright (c) 2015 Varun Pandey
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 
 * Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 * 
 * redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 
 * Neither the name of the project's author nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include "../include/common_headers.h"
#include "../include/constant.h"

__device__ bool d_found;

__device__
int remove_byte(int64_t& current_digest, BYTE from_digest, size_t msb_pos_window);
__device__
int insert_byte(int64_t& current_digest, BYTE from_digest);
/* Cleanup rutine before successfull termination of this program
 * */
void OnExit (void) 
{
	std::cout << std::endl << "Terminating Now..." << std::endl;
}

/*
 * Cuda Kernel that will be used run independently on 2*(size of pattern)
 * to find the patern.
 * Returns true on success.
 * */
__global__ 
void find_pattern (BYTE* data, int64_t pattern_fingerprint, size_t window_size, size_t msb_multiplier)
{
	int msb_pos_window = 0;
	int64_t text_digest = 0;
	int start_pos = threadIdx.x;
	BYTE *window = new BYTE [2*window_size];
	if (!window)
		return;

	if (start_pos % (window_size*2))
	{
		/*
 		* start with adding window_size bytes to the digest
 		* and try to matching the fingerprint for window_size+1 shifts
 		* */
		int i = 0;
		for (; i < window_size; i++) {
			insert_byte (text_digest, *(data+start_pos+i));
			window[i] = *(data+start_pos+i);

		}
			
		for (++i; i < (2*window_size); i++)
		{
			if (text_digest == pattern_fingerprint) {
				d_found = true; break;
			}
			
			remove_byte (text_digest, window[msb_pos_window++], msb_multiplier);
			window[i] = *(data+start_pos+i);
			insert_byte (text_digest, *(data+start_pos+i));
		}
	}
}
/* Assuing that the bytes is in the msb position, removes 
 * the byte.
 * returns 0 on success
 * */
__device__
int remove_byte(int64_t& current_digest, BYTE from_digest, size_t msb_multiplier)
{
	int ret_val = 0;

	current_digest = current_digest - (msb_multiplier * from_digest); //shift the byte
	/*After much head banging, I am adding this code. Underflowing hash, damn it!*/
	while (current_digest < 0)
		current_digest += PRIME;
	
	return ret_val;
}

/* Insert single byte into the digest 
 * Note: - The first byte will goto msb position. So data[0]
 * is at the highest order of the polynomial. Subsequently,
 * this is the first byte to be kicked out of our window.
 * Window size = pattern size (for now). For large patterns
 * proportional to text, window size can be controlled.
 * returns 0 on success
 *
 * */
__device__
int insert_byte(int64_t& current_digest, BYTE to_digest)
{
	int ret_val = 0;

	current_digest *= RADIX; //shift the byte
	current_digest += to_digest;
	current_digest %= PRIME;
	
	return ret_val;
}

int insert_byte_host(int64_t& current_digest, BYTE to_digest, size_t msb_pos_window)
{
        int ret_val = 0;

        current_digest *= RADIX; //shift the byte
        current_digest += to_digest;
        current_digest %= PRIME;

        return ret_val;
}

/* Process $size bytes and add it into the into the digest
 * returns 0 on success
 * */
int insert_bytes(int64_t& current_digest, BYTE* data, size_t size)
{
	int ret_val = 0;
	for (int i = (size_t)0; i < size; i++){
		insert_byte_host (current_digest, data[i], msb_multiplier);		
	}

	return ret_val;
}

int main (int argc, char ** argv)
{
	typeof(d_found) found = false;
	size_t i = 0;
	int64_t pattern_hash = 0;
		
	BYTE* read_buf = NULL;
	BYTE* window = NULL; 

	std::ifstream file_stream;
	std::streampos stream_pos;
	
	if (argc != 3)
		USAGE_EXIT(argv[0]);
	
	atexit (OnExit);	

	try {
		window_size = strlen(argv[2]); //Pattern size = window size
		for (i = 1; i < window_size; i++)
			msb_multiplier = (msb_multiplier*RADIX)%PRIME;

		// Check if file exist		
		file_stream.open (argv[1], std::ios::in | std::ios::binary);
		if (file_stream.bad() || file_stream.fail())
			THROW("Please check the file path and try again");
	
		//Get File size
       		size_t fsize = file_stream.tellg();
       		file_stream.seekg (0, std::ios::end);
	    	fsize = file_stream.tellg() - (std::streampos)fsize;
		if (fsize % window_size) 
			fsize = fsize + (window_size - (fsize % window_size)); //create extra room for padding if needed
		
		//reset stream
		file_stream.clear();                 // clear fail and eof bits
		file_stream.seekg(0, std::ios::beg); // back to the start!

		read_buf = new BYTE [fsize];
                if (!read_buf)
                        THROW("Bad allocation");
		memset (read_buf, '@', fsize); // Pad the data with @
	
		//Fill the window and get the 
	        file_stream.read(read_buf, fsize);
		insert_bytes (pattern_hash, argv[2], window_size);
		
#ifdef DEBUG	/*For Dedugging*/
		std::cout << std::endl << "Pattern hash: " << pattern_hash << std::endl;
#endif
			
		BYTE* d_text;
		cudaMalloc ((void**)&d_text, fsize); 
		cudaMemcpy(d_text, read_buf, fsize, cudaMemcpyHostToDevice );
		
		find_pattern<<<1, 1024>>>(d_text, pattern_hash, window_size, msb_multiplier);
		
		cudaMemcpyFromSymbol (&found, d_found, sizeof(found), cudaMemcpyDeviceToHost);
		cudaFree(d_text);
	} catch (const std::exception& e) {
		REPORT(e);
	}

	/*Display Result*/
	PATTERN_REPORT(found, file_stream.tellg());
	
	/*Cleanup*/
	if (read_buf)	
		delete [] read_buf, read_buf = NULL;
	if (window)
		delete [] window, window = NULL;

	file_stream.close();
	
	return 0;
}
