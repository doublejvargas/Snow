#pragma once
#include "ConstantBuffers.h"
#include "drawable/Drawable.h"

// lib
#include <DirectXMath.h>

class TransformCbuf : public Bindable  // inheritance
{
public:
	TransformCbuf(SnGraphics& gfx, const Drawable& parent);
	void Bind(SnGraphics& gfx) noexcept override;

private:
	// Making the transform bindable as static means we only have 1 resource and each instance of a drawable will update this
	// transform constant buffer with its transformation data each frame. This saves us memory space compared to having a transform buffer for each instance.
	static std::unique_ptr<VertexConstantBuffer<DirectX::XMMATRIX>> _pVcbuf;  //composition

	// reference to the "parent" drawable object which will contain the object's transformation matrix
	//  which we can retrieve, and then use to build a vertex constant buffer (field above this one) and update
	//  the drawables in real time.
	//
	// Drawable is not directly the parent of this TransformCbuf object, rather TransformCbuf is a child of Bindable, and Drawable contains (composition)
	//  all the bindables. Therefore, this reference contains a bindable object which is the parent of this current TransformCbuf.
	const Drawable& _parent;
};