struct Material
{
	float4 ambient;
	float4 diffuse;
	float4 specular;
	float4 Reflect;
};

struct DirectionLight
{
	float4 ambient;
	float4 diffuse;
	float4 specular;
	float3 direction;
	float waste;         // 填充位
};

cbuffer LightConstantBuffer : register(b0)
{
	Material		material;
	DirectionLight	light;
	float3			eyePos;
	float			waste;
};

// 通过像素着色器传递的每个像素的颜色数据。
struct PixelShaderInput
{
	float4 pos :	SV_POSITION;
	float3 posW:	POSITION;
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
	// 准备N E单位化向量
	float3 normal = normalize(input.normal);
	float3 toEye = normalize(eyePos - input.posW);

	// 初始化
	float4 ambient, diffuse, specular;
	ambient = diffuse = specular = float4(0.0f, 0.0f, 0.0f, 0.0f);

	//// 光向量与照射方向相反
	//float3 lightVec = -light.direction;

	//// 计算环境光
	//ambient =light.ambient;

	//// 计算漫反射光影响因子
	//float diffuseFactor = dot(lightVec, input.normal);

	//// 展开，避免动态分支
	//[flatten]
	//if (diffuseFactor > 0.0f)
	//{
	//	float3 v = reflect(-lightVec, input.normal);
	//	float specFactor = pow(max(dot(v, toEye), 0.0f), 5.f);

	//	diffuse = diffuseFactor * material.diffuse * light.diffuse;
	//	spec = specFactor * material.specular * light.specular;
	//}

	 //计算环境光
	ambient = material.ambient * light.ambient;

	//计算漫反射光
	float3 L = normalize(-light.direction);
	float diffuseLight = max(dot(normal, L), 0);
	diffuse = material.diffuse * diffuseLight;

	//计算高光
	float3 H = normalize(L + toEye);
	float specularLight = pow(max(dot(normal, H), 0), 256);

	if (diffuseLight <= 0)
		specularLight = 0;
	specular = material.specular * specularLight;

	float4 texColor = g_tex.Sample(samTex,input.tex);
	float4 lightColor = texColor + ambient + diffuse + specular;

	lightColor.a = material.diffuse.a * texColor.a;

	return lightColor;
}
