#include "Pyramid.h"
#include "bindable/BindableBase.h"
#include "error/GraphicsThrowMacros.h"
#include "primitives/Cone.h"


Pyramid::Pyramid(SnGraphics& gfx,
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
	namespace dx = DirectX;

	if (!IsStaticInitialized())
	{
		struct Vertex
		{
			dx::XMFLOAT3 pos;
			struct
			{
				unsigned char r;
				unsigned char g;
				unsigned char b;
				unsigned char a;
			} color;
		};
		auto model = Cone::MakeTesselated<Vertex>(4);
		// set vertex colors for mesh
		model.vertices[0].color = { 255,255,0 };
		model.vertices[1].color = { 255,255,0 };
		model.vertices[2].color = { 255,255,0 };
		model.vertices[3].color = { 255,255,0 };
		model.vertices[4].color = { 255,255,80 };
		model.vertices[5].color = { 255,10,0 };
		// deform mesh linearly
		model.Transform(dx::XMMatrixScaling(1.0f, 1.0f, 0.7f));

		AddStaticBind(std::make_unique<VertexBuffer>(gfx, model.vertices));

		auto pvs = std::make_unique<VertexShader>(gfx, L"ColorBlendVS.cso");
		auto pvsbc = pvs->GetBytecode();
		AddStaticBind(std::move(pvs));

		AddStaticBind(std::make_unique<PixelShader>(gfx, L"ColorBlendPS.cso"));

		AddStaticIndexBuffer(std::make_unique<IndexBuffer>(gfx, model.indices));

		const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
		{
			{ "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
			{ "Color",0,DXGI_FORMAT_R8G8B8A8_UNORM,0,12,D3D11_INPUT_PER_VERTEX_DATA,0 },
		};
		AddStaticBind(std::make_unique<InputLayout>(gfx, ied, pvsbc));

		AddStaticBind(std::make_unique<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	}
	else
	{
		SetIndexFromStatic();
	}

	AddBind(std::make_unique<TransformCbuf>(gfx, *this));
}

void Pyramid::Update(float dt) noexcept
{
	_roll	+= _droll	* dt;
	_pitch	+= _dpitch	* dt;
	_yaw	+= _dyaw	* dt;
	_theta	+= _dtheta	* dt;
	_phi	+= _dphi	* dt;
	_chi	+= _dchi	* dt;
}

DirectX::XMMATRIX Pyramid::GetTransformXM() const noexcept
{
	namespace dx = DirectX;
	return dx::XMMatrixRotationRollPitchYaw(_pitch, _yaw, _roll) *
		dx::XMMatrixTranslation(_r, 0.0f, 0.0f) *
		dx::XMMatrixRotationRollPitchYaw(_theta, _phi, _chi) *
		dx::XMMatrixTranslation(0.0f, 0.0f, 20.0f);
}