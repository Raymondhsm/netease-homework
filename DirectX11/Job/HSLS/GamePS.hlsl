struct Material
{
	float4 ambient;
	float4 diffuse;
	float4 specular;
	float4 reflect;
};

cbuffer LightConstantBuffer : register(b0)
{
	Material	g_material;
	float3		eyePos;
};

// 通过像素着色器传递的每个像素的颜色数据。
struct PixelShaderInput
{
	float4 pos :	SV_POSITION;
	float3 normal:	NORMAL;
	float2 tex :	TEXCOORD;
};


//设置过滤方式
SamplerState samTex
{
	Filter = MIN_MAG_MIP_LINEAR;
};

Texture2D g_tex;


// (内插)颜色数据的传递函数。
float4 main(PixelShaderInput input) : SV_TARGET
{
	float4 texColor = g_tex.Sample(samTex,input.tex);
	return texColor;
}
