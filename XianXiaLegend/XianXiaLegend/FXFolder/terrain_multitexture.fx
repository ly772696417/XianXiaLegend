//=============================================================================
// terrain_multitexture.fx by Frank Luna (C) 2004 All Rights Reserved.
//=============================================================================

uniform extern float4x4 gWVP;
uniform extern float2   gTexScale;
uniform extern float3   gDirToSunW; // Assumed to be unit length.
 
uniform extern texture  gLayerMap0;
uniform extern texture  gLayerMap1;
uniform extern texture  gLayerMap2;
uniform extern texture  gBlendMap;

sampler LayerMap0Sampler = sampler_state
{
    Texture   = <gLayerMap0>;
    MinFilter = LINEAR; 
    MagFilter = LINEAR;
    MipFilter = LINEAR;
    AddressU  = WRAP;
    AddressV  = WRAP;
};

sampler LayerMap1Sampler = sampler_state
{
	Texture   = <gLayerMap1>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
	AddressU  = WRAP;
    AddressV  = WRAP;
};

sampler LayerMap2Sampler = sampler_state
{
    Texture   = <gLayerMap2>;
    MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
	AddressU  = WRAP;
    AddressV  = WRAP;
};

sampler BlendMapSampler = sampler_state
{
    Texture   = <gBlendMap>;
    MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
};

struct OutputVS
{
    float4 posH         : POSITION0;
    float  shade        : TEXCOORD0;
    float2 tiledTexC    : TEXCOORD1; 
    float2 nonTiledTexC : TEXCOORD2;
};

OutputVS Terrain_MultiTexVS(float4 posL         : POSITION0, 
                            float3 normalL      : NORMAL0,  // Assumed to be unit length
                            float2 tiledTexC    : TEXCOORD0,
                            float2 nonTiledTexC : TEXCOORD1)
{
	OutputVS outVS = (OutputVS)0;


	// Project position homogeneous clip space.
    outVS.posH  = mul(posL, gWVP);
    
    // Do basic diffuse lighting calculating to compute vertex shade.
    outVS.shade = max(0.0f, dot(normalL, gDirToSunW));
    
    // Scale tiled tex-coords as specified by the application.
    outVS.tiledTexC.x  = tiledTexC.x * gTexScale.x;
    outVS.tiledTexC.y  = tiledTexC.y * gTexScale.y;
    
    // Forward non-tiled tex-coords to pixel shader.
    outVS.nonTiledTexC = nonTiledTexC;
    
  
    return outVS;
}

float4 Terrain_MultiTexPS(float  shade        : TEXCOORD0,
                          float2 tiledTexC    : TEXCOORD1,
					      float2 nonTiledTexC : TEXCOORD2) : COLOR
{
	// Layer maps are tiled
    float3 c0 = tex2D(LayerMap0Sampler, tiledTexC) ;
    float3 c1 = tex2D(LayerMap1Sampler, tiledTexC);
    float3 c2 = tex2D(LayerMap2Sampler, tiledTexC);
    
    // Blendmap is not tiled.
    float3 B = tex2D(BlendMapSampler, nonTiledTexC);

	// Find the inverse of all the blend weights so that we can
	// scale the total color to the range [0, 1].
    float totalInverse = 1.0f / (B.r + B.g + B.b);
    
    c0 *= B.r * totalInverse;
    c1 *= B.g * totalInverse;
    c2 *= B.b * totalInverse;
    
    float3 final = (c0 + c1 + c2) * shade;
 
		//return float4(shade,shade,shade,1.0f);
    return float4(final, 1.0f);
}

technique Terrain_MultiTex
{
    pass P0
    {
        vertexShader = compile vs_2_0 Terrain_MultiTexVS();
        pixelShader  = compile ps_2_0 Terrain_MultiTexPS();

		// DO NOT SET SAMPLERS IN EFFECT
    }
}
