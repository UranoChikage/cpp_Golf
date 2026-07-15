//--------------------------------------------------------------//
//	"vn_shader.h"												//
//		シェーダ管理											//
//													2026/01/01	//
//														Ichii	//
//--------------------------------------------------------------//
#pragma once

class vnShader
{
public:

	enum class eVertexShader
	{
		VS_3D,
		VS_3D_Skin,
		VS_3D_Line,
		VS_2D,
		VS_Copy,
		VS_Gray,
		VS_MAX,
	};

	enum class ePixelShader
	{
		PS_3D,
		PS_3D_NoLight,
		PS_3D_Line,
		PS_2D,
		PS_Copy,
		PS_Gray,
		PS_MAX,
	};

	class vnShaderObject
	{
	protected:
		const BYTE *code;
		long length;
	public:
		vnShaderObject(const WCHAR *path);
		~vnShaderObject();

		const BYTE *getCode(){ return code; }
		long getLength(){ return length; }
	};

private:
	static vnShaderObject **vshader;
	static vnShaderObject **pshader;

public:
	static bool initialize();
	static void terminate();

	static vnShaderObject* getVShader(eVertexShader vs) { return vshader[(unsigned int)vs]; }
	static vnShaderObject* getPShader(ePixelShader ps) { return pshader[(unsigned int)ps]; }
};