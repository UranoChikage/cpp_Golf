//“ü—Í—p
struct vertexIn
{
	float4 pos : POSITION0;
	float2 tx0 : TEXCOORD0;
};

//o—Í—p
struct vertexOut
{
	float4 pos : SV_POSITION;
	float2 tx0 : TEXCOORD0;
};

vertexOut main(vertexIn IN)
{
	vertexOut OUT;

	OUT.pos = IN.pos;
	OUT.tx0 = IN.tx0;

	return OUT;
}
