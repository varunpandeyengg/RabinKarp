/*
 * rabin_karp.cpp
 * 
 * Created by Varun Pandey on 05-07-2015.
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

/* Cleanup rutine before successfull termination of this program
 * */
void OnExit (void) 
{
	std::cout << std::endl << "Terminating Now..." << std::endl;
}

/* Assuing that the bytes is in the msb position, removes 
 * the byte.
 * returns 0 on success
 * */
int remove_byte(uint64_t& current_digest, BYTE from_digest)
{
	int ret_val = 0;

	current_digest = current_digest - (msb_multiplier * from_digest); //shift the byte
	//current_digest %= PRIME;
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
int insert_byte(uint64_t& current_digest, BYTE to_digest)
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
int insert_bytes(uint64_t& current_digest, BYTE* data, size_t size)
{
	int ret_val = 0;
	for (auto i = (size_t)0; i < size; i++){
		insert_byte (current_digest, data[i]);		
	}

	return ret_val;
}

int main (int argc, char ** argv)
{
	auto found = false;
	auto i = (size_t)0;
	uint64_t text_digest = 0;
	uint64_t pattern_hash = 0;
	size_t msb_pos_window = 0;
		
	BYTE* read_buf = NULL;
	BYTE* window = NULL; 

	std::ifstream file_stream;
	std::streampos stream_pos;
	
	if (argc != 3) {
		std::cout << std::endl << "Illegal number of arguments.";
		return -1;
	}
	
	atexit (OnExit);	

	try {
		window_size = strlen(argv[2]); //Pattern size = window size
		for (i = 1; i < window_size; i++)
			msb_multiplier = (msb_multiplier*RADIX)%PRIME;

		read_buf = new BYTE [BUF_SIZE];
		if (!read_buf)
			THROW("Bad allocation");

		window = new BYTE [window_size];
                if (!window)
                        THROW("Bad allocation");

		// Check if file exist		
		file_stream.open (argv[1], std::ios::in | std::ios::binary);
		if (file_stream.bad() || file_stream.fail())
			THROW("Please check the file path and try again");
	
		ZERO(read_buf, BUF_SIZE);
        
		//Fill the window and get the 
	        file_stream.read(read_buf, window_size);
		memcpy (window, read_buf, window_size);
		stream_pos = file_stream.tellg();		
		insert_bytes (text_digest, read_buf, window_size);
		insert_bytes (pattern_hash, argv[2], window_size);
		
#ifdef DEBUG	/*For Dedugging*/
		std::cout << std::endl << "Pattern hash: " << pattern_hash << std::endl;
		std::cout << std::endl << std::string(window) << ":" << (text_digest) << std::endl;
#endif

		//We will be accessing only the hashed value of the pattern...

		if ((text_digest) == pattern_hash)
                        found = true;

		while (!found && !file_stream.eof()) {
			ZERO(read_buf, BUF_SIZE);
			file_stream.read(read_buf, BUF_SIZE);
			stream_pos = file_stream.tellg();
		
			for (i = 0; i < BUF_SIZE; i++) {	
				remove_byte (text_digest, window[msb_pos_window]);
				window[msb_pos_window] = *(read_buf+i);
				insert_byte (text_digest, *(read_buf+i));

#ifdef DEBUG			/*For Dedugging*/
				std::cout << std::endl << std::string(window) << ":" << text_digest << std::endl;
#endif				
				if ((++msb_pos_window) == window_size)
					 msb_pos_window = 0;
				
				if (text_digest == pattern_hash) {
					found = true; break;
				}	
			}
		}
		
	} catch (const std::exception& e) {
		std::cout << std::endl << "Error reported: " <<e.what() << std::endl;
	}

	/*Display Result*/
	if (found) 
		std::cout << std::endl << "Pattern found!!!" << std::endl;
	else
		std::cout << std::endl << "Pattern not found..." << std::endl;
	
	/*Cleanup*/
	if (read_buf)	
		delete [] read_buf, read_buf = NULL;
	if (window)
		delete [] window, window = NULL;
	file_stream.close();
	
	return 0;
}
