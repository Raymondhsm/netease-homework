// 通过像素着色器传递的每个像素的颜色数据。
struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD;
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
