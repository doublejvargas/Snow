#pragma once
#include "graphics/SnGraphics.h"

// lib
#include <DirectXMath.h>

class Bindable;

class Drawable
{
	template<class T>
	friend class DrawableBase;

public:
	Drawable() = default;
	Drawable(const Drawable&) = delete;

	virtual ~Drawable() = default;

	virtual DirectX::XMMATRIX GetTransformXM() const noexcept = 0;
	void Draw(SnGraphics& gfx) const noexcept(!IS_DEBUG);
	virtual void Update(float dt) noexcept = 0;

protected:
	void AddBind(std::unique_ptr<Bindable> bind) noexcept(!IS_DEBUG);
	void AddIndexBuffer(std::unique_ptr<class IndexBuffer> ibuf) noexcept(!IS_DEBUG); // interesting class declaration inside the smart pointer brackets

private:
	// pure virtual function
	// Drawable needs access to the static bindables that will be declared by its children
	// It is bridging a gap between "static templated" world and "dynamic virtual" world
	virtual const std::vector<std::unique_ptr<Bindable>>& GetStaticBinds() const noexcept = 0;

private:
	// why the class declaration here before the type??
	const class IndexBuffer* _pIndexBuffer = nullptr; // reference to index buffer that will be needed for context->DrawIndexed call
	std::vector<std::unique_ptr<Bindable>> _binds; // we store the bindables as unique<Bindable> ptrs because we will need to use polymorphism in the child classes of Drawable
};