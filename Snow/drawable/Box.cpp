#include "Box.h"
#include "bindable/BindableBase.h"

// exception macros
#include "error/GraphicsThrowMacros.h"

Box::Box(SnGraphics& gfx,
	std::mt19937& rng,
	std::uniform_real_distribution<float>& adist,
	std::uniform_real_distribution<float>& ddist,
	std::uniform_real_distribution<float>& odist,
	std::uniform_real_distribution<float>& rdist)
	:
	_r(rdist(rng)),
	_droll(ddist(rng)),
	_dpitch(ddist(rng)),
	_dyaw(ddist(rng)),
	_dphi(odist(rng)),
	_dtheta(odist(rng)),
	_dchi(odist(rng)),
	_chi(adist(rng)),
	_theta(adist(rng)),
	_phi(adist(rng))
{
	struct Vertex
	{
		struct
		{
			float x;
			float y;
			float z;
		} pos;
	};

	const std::vector<Vertex> vertices =
	{
		{ -1.0f, -1.0f, -1.0f },
		{  1.0f, -1.0f, -1.0f },
		{ -1.0f,  1.0f, -1.0f },
		{  1.0f,  1.0f, -1.0f },
		{ -1.0f, -1.0f,	 1.0f },
		{  1.0f, -1.0f,	 1.0f },
		{ -1.0f,  1.0f,	 1.0f },
		{  1.0f,  1.0f,	 1.0f },
	};

	AddBind(std::make_unique<VertexBuffer>(gfx, vertices));

	auto pvs = std::make_unique<VertexShader>(gfx, L"VertexShader.cso");
	auto pvsbc = pvs->GetBytecode();	// this is the blob, it will be needed later for creating the input layout
	AddBind(std::move(pvs)); //TODO: look into how std::move works.

	AddBind(std::make_unique<PixelShader>(gfx, L"PixelShader.cso"));

	const std::vector<unsigned short> indices =
	{
		0,2,1, 2,3,1,
		1,3,5, 3,7,5,
		2,6,3, 3,6,7,
		4,5,7, 4,7,6,
		0,4,2, 2,4,6,
		0,1,4, 1,5,4
	};

	AddIndexBuffer(std::make_unique<IndexBuffer>(gfx, indices));

	struct ConstantBuffer2
	{
		struct
		{
			float r;
			float g;
			float b;
			float a;
		} face_colors[6];
	};

	const ConstantBuffer2 cb2 =
	{
		{
			{ 1.0f,	 0.0f,	1.0f },
			{ 1.0f,	 0.0f,	0.0f },
			{ 0.0f,	 1.0f,	0.0f },
			{ 0.0f,	 0.0f,	1.0f },
			{ 1.0f,	 1.0f,	0.0f },
			{ 0.0f,	 1.0f,	1.0f },
		}
	};
	AddBind(std::make_unique<PixelConstantBuffer<ConstantBuffer2>>(gfx, cb2));

	const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
	{
		/*semanticName: must match with the element name specified in vertex shader
		  semanticIndex: the index (i.e., layout location) for the element, also must match with shader (TODO: REVIEW)
		  format: specifies the format of the data, for vertex positions this would be 2 32-bit floats (r32g32)
		  input slot: "always 0" according to chili
		  AlignedByteOffset: the offset from the beginning of vertex structure data to this specific element
		  InputSlotClass: vertex vs index? for now we'll use vertex
		  InstanceDataStepRate: we're not working with instances yet, so 0*/
		{ "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
	};

	AddBind(std::make_unique<InputLayout>(gfx, ied, pvsbc)); // we create the input layout here with the blob from the vertex shader

	AddBind(std::make_unique<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

	AddBind(std::make_unique<TransformCbuf>(gfx, *this));
}

void Box::Update(float dt) noexcept
{
	_roll	+= _droll	* dt;
	_pitch	+= _dpitch	* dt;
	_yaw	+= _dyaw	* dt;
	_theta	+= _dtheta	* dt;
	_phi	+= _dphi	* dt;
	_chi	+= _dchi	* dt;
}

DirectX::XMMATRIX Box::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixRotationRollPitchYaw(_pitch, _yaw, _roll) * // rotate about the box's center
		DirectX::XMMatrixTranslation(_r, 0.0f, 0.0f) *					// translate box away from space's origin
		DirectX::XMMatrixRotationRollPitchYaw(_theta, _phi, _chi) *		// rotate about the origin
		DirectX::XMMatrixTranslation(0.0f, 0.0f, 20.0f);				// translate the box away from the camera for better visibility
}