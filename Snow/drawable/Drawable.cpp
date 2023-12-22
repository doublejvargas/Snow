#include "Drawable.h"
#include "bindable/IndexBuffer.h"

// lib
#include <cassert>
#include <typeinfo>

// exception macros
#include "error/GraphicsThrowMacros.h"

void Drawable::Draw(SnGraphics& gfx) const noexcept(!IS_DEBUG)
{
	for (auto& b : _binds)
	{
		b->Bind(gfx);
	}

	gfx.DrawIndexed(_pIndexBuffer->GetCount());
}

void Drawable::AddBind(std::unique_ptr<Bindable> bind) noexcept(!IS_DEBUG)
{
	assert("*Must* use AddIndexBuffer to bind index buffer" && typeid(*bind) != typeid(IndexBuffer));
	_binds.push_back(std::move(bind));  //TODO: look into how std::move works
}

void Drawable::AddIndexBuffer(std::unique_ptr<IndexBuffer> ibuf) noexcept
{
	assert("Attempting to add index buffer a second time" && _pIndexBuffer == nullptr);
	_pIndexBuffer = ibuf.get();
	_binds.push_back(std::move(ibuf));
}