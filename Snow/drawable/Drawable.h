#pragma once
#include "graphics/SnGraphics.h"

// lib
#include <DirectXMath.h>

class Bindable;

class Drawable
{
public:
	Drawable() = default;
	Drawable(const Drawable&) = delete;

	virtual ~Drawable() = default;

	virtual DirectX::XMMATRIX GetTransformXM() const noexcept = 0;
	void Draw(SnGraphics& gfx) const noexcept(!IS_DEBUG);
	virtual void Update(float dt) noexcept = 0;
	void AddBind(std::unique_ptr<Bindable> bind) noexcept(!IS_DEBUG);
	void AddIndexBuffer(std::unique_ptr<class IndexBuffer> ibuf) noexcept;

private:
	const IndexBuffer* _pIndexBuffer = nullptr; // reference to index buffer that will be needed for context->DrawIndexed call
	std::vector<std::unique_ptr<Bindable>> _binds; // we store the bindables as unique<Bindable> ptrs because we will need to use polymorphism in the child classes of Drawable
};