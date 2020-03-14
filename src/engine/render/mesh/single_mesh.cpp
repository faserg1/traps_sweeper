#include "single_mesh.h"

#include "src/engine/utils/compile_time/compile_time.h"
#include <vector>
namespace engine::render
{
	class TypeBase {

	};

	template <typename T, i32 Elems>
	class TypeImpl : TypeBase {
	public:
		constexpr static i32 size() { return sizeof(T) * Elems; }
		using Type = T;
		static i32 count() { return Elems; }
	};

	template <typename... Ts>
	class TypeAggregate {
	public:
		static i64 totalSize(i32 count) {
			return (Ts::size() + ...) * count;
		}
		template <typename Target>
		static i64 getOffset(i32 count) {
			static_assert(compile_time::contains<Target, Ts...>(), "Error: Type not found.");
			static_assert(compile_time::countSame<Target, Ts...>() == 1, "Error: sequense has same types.");
			return getOffsetImpl() * count;
		}
	private:
		template <typename Target>
		constexpr static i64 getOffsetImpl() {
			struct {
				i64 size;
				bool isTarget;
			} allSizes[sizeof...(Ts)] = {{Ts::size(), std::is_same_v<Target, Ts>}...};
			i64 offset = 0;
			for (auto &size : allSizes) {
				if (size.isTarget)
					break;
				offset += size.size;
			}
			return offset;
		}
	};

	class Pos : public TypeImpl<f32, 3> {};
	class Normal : public TypeImpl<f32, 3> {};
	class UV : public TypeImpl<f32, 2> {};
	class Color : public TypeImpl<u8, 4> {};
	class Index : public TypeImpl<u16, 1> {};

	using MeshVertices = TypeAggregate<Pos, Normal, UV, Color>;
	using MeshIndices = TypeAggregate<Index>;

	MeshData::MeshData(i32 vertices, i32 indices)
		: _data(MeshVertices::totalSize(vertices) + MeshIndices::totalSize(indices)),
		_vertexCount(vertices), _indexCount(indices)
	{
	}

	template <typename T>
	ArrayView<typename T::Type> getDataViewHelper(RawMemory& buffer, i32 vCount, i32 iCount) {
		auto offset = MeshVertices::getOffset<T>(vCount) + MeshIndices::totalSize(iCount);
		return buffer.view<typename T::Type>(offset, T::count()*vCount);
	}

	ArrayView<f32> MeshData::positions() 
	{
		return getDataViewHelper<Pos>(_data, _vertexCount, _indexCount);
	}
	ArrayView<f32> MeshData::normals() 
	{
		return getDataViewHelper<Normal>(_data, _vertexCount, _indexCount);
	}
	ArrayView<f32> MeshData::uvs() 
	{
		return getDataViewHelper<UV>(_data, _vertexCount, _indexCount);
	}
	ArrayView<u8> MeshData::colors() 
	{
		return getDataViewHelper<Color>(_data, _vertexCount, _indexCount);
	}
	ArrayView<u16> MeshData::indices() 
	{
		return _data.view<Index::Type>(0, _indexCount);
	}
	i32 MeshData::vCount() const
	{
		return _vertexCount;
	}
	i32 MeshData::iCount() const
	{
		return _indexCount;
	}
}