#pragma once
#include <core.h>
#include <algorithm>

namespace xb2at {
namespace core {

	/**
	 * A istream allowing a vector<char> to be used as a data buffer without copying the data multiple times 
	 * (speeding things up slightly and greatly lowering memory usage).
	 * Essentially, like a .NET MemoryStream, but with less memory copying.
	 */
	struct ivstream : public std::istream {

		class vector_streambuf : public std::streambuf {
			typedef std::vector<char>::size_type size_type;

			size_type read_pos;
		public:
			std::vector<char>& buffer;

			vector_streambuf(std::vector<char>& buffer_) 
				: read_pos(0),
				buffer(buffer_) {
			}

			pos_type seekoff(off_type off, std::ios_base::seekdir dir, std::ios_base::openmode which = std::ios_base::in) override {
				if (dir == std::ios_base::cur)
					gbump(off);
				else if (dir == std::ios_base::end)
					setg(eback(), egptr() + off, egptr());
				else if (dir == std::ios_base::beg)
					setg(eback(), eback() + off, egptr());

				read_pos = gptr() - eback();
				return read_pos;
			}

			pos_type seekpos(pos_type sp, std::ios_base::openmode which) override {
				return seekoff(sp - pos_type(off_type(0)), std::ios_base::beg, which);
			}

			int_type underflow() {
				if (read_pos < buffer.size())
					return static_cast<unsigned char>(buffer[read_pos]);
				else
					return EOF;
			}

			int_type uflow() {   
				if (read_pos < buffer.size())
					return static_cast<unsigned char>(buffer[read_pos++]);
				else
					return EOF;
			}

			int_type pbackfail(int_type c) {
				// if they are trying to push back a character that they didn't read last
				// that is an error
				const unsigned long prev = read_pos-1;
				if (c != EOF && prev < buffer.size() && 
					c != static_cast<unsigned char>(buffer[prev])) {
					return EOF;
				}

				read_pos = prev;
				return 1;
			}

			std::streamsize xsgetn(char* s, std::streamsize n) { 
				if (read_pos < buffer.size()) {
					const size_type num = std::min<size_type>(n, buffer.size()-read_pos);
					memcpy(s, &buffer[read_pos], num);
					read_pos += num;
					return num;
				}
				return 0;
			}

		};

		ivstream(std::vector<char>& buffer) 
			: std::istream(&buf),
			buf(buffer) {
		
		}

	private:
		vector_streambuf buf;
	};

}
}