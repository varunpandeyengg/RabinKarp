/*
 * constant.h
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


#ifndef CONSTANT
#define CONSTANT

#define BUF_SIZE 10
#define BYTE char
#define RADIX 128//3//128//256
#define PRIME 3355439 //Prime Number

#define THROW(err) {                                                                            \
                        std::ostringstream err_msg;                                             \
                        err_msg <<  __FILE__ << ":" << __LINE__ << " Error msg : " << err;      \
                        throw std::runtime_error(err_msg.str());                                \
                }

#define ZERO(s, size) memset(s, 0, size);
#define PATTERN_REPORT(found, pos){ \
					if (found)\
						std::cout << std::endl << "Pattern found!!! at position " << pos << std::endl; \
					else \
						std::cout << std::endl << "Pattern not found..." << std::endl;\
				}

#define REPORT(e) std::cout << std::endl << "Error reported: " << e.what() << std::endl;
#define USAGE_EXIT(process) {\
                                        printf ("\t Illegal Usage. Expected two arg - text path and pattern. Correct usage for the this utility is:\n"); \
                                        printf ("\t %s  <file_path aka text> <pattern text>\n", process); \
                                        printf ("\t \t \t @Rabin Karp utility...\n"); \
                                        exit(-1); \
}

size_t window_size = 5; //By default, will change with 
uint64_t msb_multiplier = 1;

#endif //CONSTANT
