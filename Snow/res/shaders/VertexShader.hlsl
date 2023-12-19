cbuffer CBuf
{
    // the type here MUST match the type defined in the code (See func. DrawTestTriangle in SnGraphics.cpp)
    matrix transform; // the keyword matrix indicates a 4x4 float (array). 
};

float4 main( float3 pos : Position ) : SV_Position
{
    return mul( float4(pos, 1.0f), transform ); // you can use the "transform" name like a global and it works! As opposed to "CBuf.transform" or "CBuf::transform";
}