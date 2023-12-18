struct VSOut
{
    float3 color : Color;
    float4 pos : SV_Position;
};

cbuffer CBuf
{
    // the type here MUST match the type defined in the code (See func. DrawTestTriangle in SnGraphics.cpp)
    matrix transform; // the keyword matrix indicates a 4x4 float (array). 
};

VSOut main(float2 pos : Position, float3 color : Color)
{
    VSOut vso;
    // mul is an hlsl function for matrix multiplication (remember matrix multiplication is right-multiplied, so matrix goes on right!)
    vso.pos = mul(float4(pos.x, pos.y, 0.0f, 1.0f), transform); // you can use the "transform" name like a global and it works! As opposed to "CBuf.transform" or "CBuf::transform"
    vso.color = color;
    return vso;
}