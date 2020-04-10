#include "gl_texture.h"
#include "gl_context_info.h"
#include "defines/gl_type_enums.h"

#define GLEW_STATIC 
#include <GL/glew.h>
#include "error_handling.h"
#include <cassert>
#include <iostream>

//#include "src/engine/utils/console.h"

namespace engine::render::opengl
{
	GLenum toGLEnum(TextureParams::Format format);
	GLint toFilterEnum(TextureParams::Filter filter);
	GLint toWrapEnum(TextureParams::Wrap warp);
	GLint getInternalFormatES2(DataType type, TextureParams::Format format);
	GLint getInternalFormatES3(DataType type, TextureParams::Format format);

	struct GPUTexture::Impl {
		GLuint _texture = 0;
		TextureParams _params = {};
		i32 _w = 0;
		i32 _h = 0;
		bool _dirty = true;
		GLint (*const getInternalFormat)(DataType type, TextureParams::Format format);

		Impl(): getInternalFormat(getMajorVersion() == 3 ? &getInternalFormatES3 : &getInternalFormatES2) {
			glGenTextures(1, &_texture);
			//console::out(std::to_string(_texture));
		}
		~Impl() {
			if (_texture != 0) {// spec ignores 0s so this isnt technically neccessary, but why the heck not
				glDeleteTextures(1, &_texture); 
				_texture = 0;
			}
		}
	};
	GPUTexture::GPUTexture()
		: _impl(std::make_unique<Impl>())
	{
	}
	GPUTexture::~GPUTexture() = default;
	GPUTexture::GPUTexture(GPUTexture &&) noexcept = default;
	GPUTexture & GPUTexture::operator=(GPUTexture &&) noexcept = default;

	bool is_power_of_2(int x) {
		return x > 0 && !(x & (x - 1));
	}

	GLenum toGLEnum(TextureParams::Format format) {
		switch (format) {
		case TextureParams::Format::RGBA:
			return GL_RGBA;
		default:
			std::cerr << "What format?\n";
			throw;
		}
	}

	GLint toFilterEnum(TextureParams::Filter filter) {
		switch (filter) {
		case TextureParams::Filter::NEAREST:
			return GL_NEAREST;
		case TextureParams::Filter::LINEAR:
			return GL_LINEAR;
		case TextureParams::Filter::NEAREST_MIPMAP_NEAREST:
			return GL_NEAREST_MIPMAP_NEAREST;
		case TextureParams::Filter::LINEAR_MIPMAP_NEAREST:
			return GL_LINEAR_MIPMAP_NEAREST;
		case TextureParams::Filter::NEAREST_MIPMAP_LINEAR:
			return GL_NEAREST_MIPMAP_LINEAR;
		case TextureParams::Filter::LINEAR_MIPMAP_LINEAR:
			return GL_LINEAR_MIPMAP_LINEAR;
		default:
			throw;
		}
	}

	GLint toWrapEnum(TextureParams::Wrap warp) {
		switch (warp) {
		case TextureParams::Wrap::CLAMP_TO_EDGE:
			return GL_CLAMP_TO_EDGE;
		case TextureParams::Wrap::MIRRORED_REPEAT:
			return GL_MIRRORED_REPEAT;
		case TextureParams::Wrap::REPEAT:
			return GL_REPEAT;
		default:
			throw;
		}
	}

	GLint getInternalFormatES2(DataType type, TextureParams::Format format) {
		switch (format) {
		case TextureParams::Format::RGBA:
			return GL_RGBA;
		default:
			std::cerr << "What internal format?\n";
			throw;
		}
	}

	GLint getInternalFormatES3(DataType type, TextureParams::Format format) {
		switch (format) {
		case TextureParams::Format::RGBA:
		{
			switch (type.toInt()) {
			case DataType::F32:
				return GL_RGBA32F;
			case DataType::U8:
				return GL_RGBA8;
			default:
				std::cerr << "What type?\n";
				throw;
			}
		}
		break;
		default:
			std::cerr << "What internal format?\n";
			throw;
		}
	}

	const TextureParams& GPUTexture::params() const
	{
		return _impl->_params;
	}

	TextureParams& GPUTexture::params(bool)
	{
		_impl->_dirty = true;
		return _impl->_params;
	}

	void GPUTexture::use()
	{
		checkError(1);
		glBindTexture(GL_TEXTURE_2D, _impl->_texture);
		checkError(2);
		if (_impl->_dirty) {
			auto& params = _impl->_params;
			if (params.magFilter == params.LINEAR_MIPMAP_LINEAR
				|| params.magFilter == params.LINEAR_MIPMAP_NEAREST
				|| params.magFilter == params.NEAREST_MIPMAP_LINEAR
				|| params.magFilter == params.NEAREST_MIPMAP_NEAREST
			) {
				params.magFilter = params.LINEAR;
				// magFilter can only be NEAREST OR LINEAR!
			}
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, toFilterEnum(params.minFilter));
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, toFilterEnum(params.magFilter));
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, toWrapEnum(params.wrapS));
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, toWrapEnum(params.wrapT));

			checkError(3);

			if (params.minFilter != params.LINEAR && params.minFilter != params.NEAREST
				&& is_power_of_2(_impl->_w) && is_power_of_2(_impl->_h)) {
				glGenerateMipmap(GL_TEXTURE_2D);
			}
			checkError(4);
			_impl->_dirty = false;
		}
	}

	void GPUTexture::resizeAndClear(i32 w, i32 h) // clears data!
	{
		_impl->_w = w;
		_impl->_h = h;
		auto& params = _impl->_params;
		this->use();

		auto internalFormat = _impl->getInternalFormat(params.dataType, params.format);
		auto format = toGLEnum(params.format);
		auto type = toGLEnum(params.dataType);
		
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			internalFormat,
			w,
			h,
			0,
			format,
			type,
			nullptr
		);
		if (params.minFilter != params.LINEAR && params.minFilter != params.NEAREST
		 && is_power_of_2(w) && is_power_of_2(h)) {
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		checkError();
	}
	void GPUTexture::write(const byte* bytes)
	{
		write(bytes, 0, 0, _impl->_w, _impl->_h);
	}
	void GPUTexture::write(const byte* bytes, i32 x, i32 y, i32 w, i32 h)
	{
		assert(x >= 0 && y >= 0 && x + w <= _impl->_w && y + h <= _impl->_h);
		checkError();
		this->use();
		auto& params = _impl->_params;

		auto internalFormat = _impl->getInternalFormat(params.dataType, params.format);
		auto format = toGLEnum(params.format);
		auto type = toGLEnum(params.dataType);

		glTexSubImage2D(
			GL_TEXTURE_2D,
			0,
			x,
			y,
			w,
			h,
			toGLEnum(params.format),
			toGLEnum(params.dataType),
			bytes
		);
		if (params.minFilter != params.LINEAR && params.minFilter != params.NEAREST
			&& is_power_of_2(w) && is_power_of_2(h)) {
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		checkError();
	}
	i32 GPUTexture::byteSize()
	{
		auto& params = _impl->_params;
		auto pixels = _impl->_w * _impl->_h;
		auto channels = params.format == TextureParams::Format::RGBA ? 4 : 0;
		auto bytesPerChannel = params.dataType.bytesPerElem();
		return pixels * channels * bytesPerChannel;
	}
	i32 GPUTexture::w() const
	{
		return _impl->_w;
	}
	i32 GPUTexture::h() const
	{
		return _impl->_h;
	}
	bool GPUTexture::operator==(const GPUTexture& other) const
	{
		return this == &other;
	}
}