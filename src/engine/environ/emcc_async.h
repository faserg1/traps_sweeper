#ifdef EMSCRIPTEN

#include <type_traits>
#include <tuple>
#include <future>
#include <functional>
#include <emscripten.h>

namespace engine::env {
	template<class Func, class ... Args>
	auto async(Func func, Args ... args) -> std::shared_future<typename std::result_of<Func(Args...)>::type> {
		static_assert( std::is_invocable<Func, Args...>::value);
		using ResultT = typename std::result_of<Func(Args...)>::type;
		struct FData {
			Func f; 
			std::tuple<Args...> args;
			std::promise<ResultT> result;
		};
		auto fdata = new FData {func, {args ...}, {}};
		void (*c_callback)(void *) = [](void *fdata_arg) {
			FData *fdata = static_cast<FData *>(fdata_arg);
			if constexpr (std::is_void_v<ResultT>)
			{
				std::apply(fdata->f, fdata->args);
				fdata->result.set_value();
			}
			else
				fdata->result.set_value(std::apply(fdata->f, fdata->args));
			delete fdata;
		};
		emscripten_async_call(c_callback, fdata, -1);
		return fdata->result.get_future().share();
	}
}

#endif