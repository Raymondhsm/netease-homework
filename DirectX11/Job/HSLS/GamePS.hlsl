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
	float4 pos :		SV_POSITION;
	float3 posW:		POSITION;
	float4 shadowPos:	POSITION1;
	float3 normal:		NORMAL;
	float2 tex :		TEXCOORD;
};


//设置过滤方式
SamplerState samTex
{
	Filter = MIN_MAG_MIP_LINEAR;
};

Texture2D BaseTexture	:	register(t0);
Texture2D ShadowMap		:	register(t1);


// (内插)颜色数据的传递函数。
float4 main(PixelShaderInput input) : SV_TARGET
{
	// 准备N E单位化向量
	float3 normal = normalize(input.normal);
	float3 toEye = normalize(eyePos - input.posW);

	// 初始化
	float4 ambient, diffuse, specular;
	ambient = diffuse = specular = float4(0.0f, 0.0f, 0.0f, 0.0f);

	 //计算环境光
	ambient = material.ambient * light.ambient;

	// 计算阴影纹理坐标
	float2 shadowTex;
	shadowTex.x = (input.shadowPos.x / input.shadowPos.w)*0.5f + 0.5f;
	shadowTex.y = (input.shadowPos.y / input.shadowPos.w)*(-0.5f) + 0.5f;

	float bias = 0.001f;
	float ShadowMapDepth,Depth;
	if (saturate(shadowTex.x) == shadowTex.x && saturate(shadowTex.y) == shadowTex.y)
	{
		ShadowMapDepth = ShadowMap.Sample(samTex, shadowTex).r;
		Depth = input.shadowPos.z / input.shadowPos.w;
		Depth = Depth - bias;

		if (ShadowMapDepth >= Depth)
		{
			//计算漫反射光
			float3 L = normalize(-light.direction);
			float diffuseLight = max(dot(normal, L), 0);
			diffuse = material.diffuse * light.diffuse * diffuseLight;

			//计算高光
			float3 H = normalize(L + toEye);
			float specularLight = pow(max(dot(normal, H), 0), 256);

			if (diffuseLight <= 0)
				specularLight = 0;
			specular = material.specular * light.specular * specularLight;
		}
	}

	float4 texColor = BaseTexture.Sample(samTex,input.tex);
	float4 lightColor = texColor * (ambient + diffuse) + specular;

	lightColor.a = material.diffuse.a * texColor.a;

	return lightColor;
}
