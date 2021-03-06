#include "gl_buffer.h"

#define GLEW_STATIC 
#include <GL/glew.h>
#include <utility>
#include <iostream>
#include <vector>
#include "error_handling.h"
#include "gl_context_info.h"

//#include "src/engine/utils/console.h";


namespace engine::render::opengl
{
	struct GPUBuffer::Impl {
		i32 _size = 0;
		GLuint _bufferId = 0;
		GLuint _type = 0;
		// Used for webgl1 buffer copy
		std::vector<byte> _memBuffer;
		const int _majorVer;

		Impl() : _majorVer(getMajorVersion()) {};
		~Impl() {
			glDeleteBuffers(1, &_bufferId);
		}
	};

	GPUBuffer::GPUBuffer(i64 size, EnumT type)
		: _impl(std::make_unique<Impl>())
	{
		_impl->_type = type;
		resize(size);
	}

	GPUBuffer::~GPUBuffer() = default;
	GPUBuffer::GPUBuffer(GPUBuffer &&) noexcept = default;
	GPUBuffer& GPUBuffer::operator=(GPUBuffer &&) noexcept = default;

	void GPUBuffer::use() const
	{
		glBindBuffer(_impl->_type, _impl->_bufferId);
	}

	void GPUBuffer::resize(i64 newSize) {
		if (newSize <= _impl->_size)
			return;
		if (_impl->_size == 0) {
			glGenBuffers(1, &(_impl->_bufferId));
			glBindBuffer(_impl->_type, _impl->_bufferId);
			glBufferData(_impl->_type, newSize, nullptr, GL_STATIC_DRAW);
			checkError();
			if (_impl->_majorVer == 2) {
				_impl->_memBuffer.resize(newSize);
			}
		}
		else {
			GLuint newBuffer = 0;
			glGenBuffers(1, &newBuffer);
			glBindBuffer(_impl->_type, newBuffer);
			glBufferData(_impl->_type, newSize, nullptr, GL_STATIC_DRAW);
			if (_impl->_majorVer > 2) {
				glBindBuffer(GL_COPY_WRITE_BUFFER, newBuffer);
				glBufferData(GL_COPY_WRITE_BUFFER, newSize, nullptr, GL_STATIC_DRAW);
				glBindBuffer(GL_COPY_READ_BUFFER, _impl->_bufferId);
				glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, _impl->_size);
			} else {
				glBufferSubData(_impl->_type, 0, _impl->_memBuffer.size(), _impl->_memBuffer.data());
				_impl->_memBuffer.resize(newSize);
			}
			glDeleteBuffers(1, &(_impl->_bufferId));
			checkError();
			_impl->_bufferId = newBuffer;
		}
		_impl->_size = newSize;
	}

	void GPUBuffer::write(void * data, i64 offset, i64 bytes) {
		glBindBuffer(_impl->_type, _impl->_bufferId);
		glBufferSubData(_impl->_type, offset, bytes, data);
		// checkError();
		auto err = glGetError();
		if (err) {
			std::cout << "Tried to write to GPU buffer with: \noffset = " << offset << "\nsize = " << bytes << std::endl;
			std::cerr << "GL error code: 0x" << std::hex << err << std::dec << std::endl;
			throw;
		}
		if (_impl->_majorVer == 2) {	
			memcpy(_impl->_memBuffer.data() + offset, data, bytes);
		}
	}

	void GPUBuffer::read(void * output, i64 offset, i64 bytes) {
		// Should not used in release
		if (getMajorVersion() == 2)
			throw;
		glBindBuffer(_impl->_type, _impl->_bufferId);
		glGetBufferSubData(_impl->_type, offset, bytes, output);
		checkError();
	}

	GPUBuffer::EnumT GPUBuffer::VERTEX_BUFFER = GL_ARRAY_BUFFER;
	GPUBuffer::EnumT GPUBuffer::INDEX_BUFFER = GL_ELEMENT_ARRAY_BUFFER;
}