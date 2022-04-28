#pragma once
#include <xb2at/core.h>
#include <algorithm>

namespace xb2at::core {

	namespace detail {
		template<class ByteType>
		struct basic_ivstream : public std::istream {

			// TODO: This really should be using spans (as we should assume we don't own the memory and don't try allocating)
			class vector_streambuf : public std::streambuf {
				using size_type = typename std::vector<ByteType>::size_type;

				size_type read_pos;

				/**
			 	 * Underlying buffer
			 	 */
				std::vector<ByteType>& buffer;

			   public:
				explicit vector_streambuf(std::vector<ByteType>& buffer_)
					: read_pos(0),
					  buffer(buffer_) {
					// code worked fine without this but I'm doing it here for correctness
					setg(reinterpret_cast<char*>(&buffer_[0]), reinterpret_cast<char*>(&buffer_[1]), reinterpret_cast<char*>(&buffer_[buffer_.size()]));
				}

				pos_type seekoff(off_type off, std::ios_base::seekdir dir, std::ios_base::openmode which = std::ios_base::in) override {
					if(dir == std::ios_base::cur)
						gbump((std::int32_t)off);
					else if(dir == std::ios_base::end)
						setg(eback(), egptr() + off, egptr());
					else if(dir == std::ios_base::beg)
						setg(eback(), eback() + off, egptr());

					read_pos = gptr() - eback();
					return read_pos;
				}

				pos_type seekpos(pos_type sp, std::ios_base::openmode which) override {
					return seekoff(sp - pos_type(off_type(0)), std::ios_base::beg, which);
				}

				int_type underflow() override {
					if(read_pos < buffer.size())
						return static_cast<unsigned char>(buffer[read_pos]);
					else
						return EOF;
				}

				int_type uflow() override {
					if(read_pos < buffer.size())
						return static_cast<unsigned char>(buffer[read_pos++]);
					else
						return EOF;
				}

				int_type pbackfail(int_type c) override {
					// if they are trying to push back a character that they didn't read last
					// that is an error
					const unsigned long prev = (unsigned long)read_pos - 1;
					if(c != EOF && prev < buffer.size() &&
					   c != static_cast<unsigned char>(buffer[prev])) {
						return EOF;
					}

					read_pos = prev;
					return 1;
				}

				std::streamsize xsgetn(char* s, std::streamsize n) override {
					if(read_pos < buffer.size()) {
						const size_type num = std::min<size_type>(n, buffer.size() - read_pos);
						memcpy(s, &buffer[read_pos], num);
						read_pos += num;
						return num;
					}
					return 0;
				}
			};

			/**
		 	 * Constructor
		 	 *
		 	 * \param[in] buffer Reference to buffer to use for this ivstream
		 	 */
			explicit basic_ivstream(std::vector<ByteType>& buffer)
				: std::istream(&buf),
				  buf(buffer) {
			}

		   private:
			vector_streambuf buf;
		};
	}

	/**
	 * A istream Implementation allowing a vector<char> to be used as a data buffer for the stream.
	 * Essentially, like .NET's MemoryStream, but with some zero copy guarantees.
	 */
	using ivstream = detail::basic_ivstream<std::uint8_t>;
	using signed_ivstream = detail::basic_ivstream<std::int8_t>;

} // namespace xb2at::core