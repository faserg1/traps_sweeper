#ifdef EMSCRIPTEN

#include <iostream>
#include <memory>
#include <thread>
#include <emscripten.h>
#include "emcc_wget.h"
#include "emcc_async.h"

std::shared_future<std::tuple<const void *, size_t>> engine::env::loadFileAsync(const char *url)
{
	using tuple_t = std::tuple<const void *, size_t>;
	auto promise = std::make_shared<std::promise<tuple_t>>();
	
	void (*onload)(void *, void *, int) = [](void *userArg, void *data, int size) {
		std::promise<tuple_t> *promise = static_cast<std::promise<tuple_t> *>(userArg);
		std::cout << "File downloaded" << std::endl;
		promise->set_value(tuple_t{static_cast<const void *>(data), static_cast<size_t>(size)});
	};
	void (*onerror)(void *) = [](void *userArg) {
		std::promise<tuple_t> *promise = static_cast<std::promise<tuple_t> *>(userArg);
		std::cerr << "File download failed" << std::endl;
		promise->set_exception({});
	};
	std::async(std::launch::async, [url, promise, onload, onerror](){
		emscripten_async_wget_data(url, promise.get(), onload, onerror);
	});
	return promise->get_future().share();
}

std::shared_future<std::tuple<const void *, size_t>> engine::env::loadFileAsyncJs(const char *url)
{
	using tuple_t = std::tuple<const void *, size_t>;
	auto promise = std::make_shared<std::promise<tuple_t>>();
	
	void (*onload)(void *, void *, int) = [](void *userArg, void *data, int size) {
		std::promise<tuple_t> *promise = static_cast<std::promise<tuple_t> *>(userArg);
		std::cout << "File downloaded" << std::endl;
		promise->set_value(tuple_t{static_cast<const void *>(data), static_cast<size_t>(size)});
	};
	void (*onerror)(void *) = [](void *userArg) {
		std::promise<tuple_t> *promise = static_cast<std::promise<tuple_t> *>(userArg);
		std::cerr << "File download failed" << std::endl;
		promise->set_exception({});
	};
	
	async([url, promise, onload, onerror](){
		emscripten_async_wget_data(url, promise.get(), onload, onerror);
	});
	
	return promise->get_future().share();
}

std::shared_ptr<std::tuple<void *, size_t>> engine::env::loadFile(const char *url)
{
	using tuple_t = std::tuple<void *, size_t>;
	void *buffer;
	int size, errCode;
	emscripten_wget_data(url, &buffer, &size, &errCode);
	if (errCode) {
		std::cerr << "File download failed" << std::endl;
		throw;
	}
	std::cout << "File downloaded" << std::endl;
	return std::shared_ptr<tuple_t>(new tuple_t{static_cast<void *>(buffer), static_cast<size_t>(size)}, [](tuple_t *t) {
		void *buffer_ptr = std::get<0>(*t);
		free(buffer_ptr);
		delete t;
	});
}

#endif