#include "upload.h"

#include "src/engine/render/chunk/attr_chunk.h"
#include "src/engine/render/model/draw_batch.h"

#include <iostream>

namespace engine::render {
	void upload(AttrChunk& chunk)
	{
		// std::cout << "Chunk upload" << std::endl;
		chunk.upload();
	}
	void upload(DrawBatch& batch)
	{
		// std::cout << "Batch upload" << std::endl;
		batch.upload();
	}
}