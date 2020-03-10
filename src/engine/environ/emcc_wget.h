#ifdef EMSCRIPTEN

#include <tuple>
#include <future>

namespace engine::env {
	std::shared_future<std::tuple<const void *, size_t>> loadFileAsyncJs(const char *url);
	std::shared_future<std::tuple<const void *, size_t>> loadFileAsync(const char *url);
	std::shared_ptr<std::tuple<void *, size_t>> loadFile(const char *url);
}

#endif