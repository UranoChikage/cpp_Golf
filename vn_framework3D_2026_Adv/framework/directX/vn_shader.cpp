//--------------------------------------------------------------//
//	"vn_shader.cpp"												//
//		シェーダ管理											//
//													2026/01/01	//
//														Ichii	//
//--------------------------------------------------------------//
#include "../../framework.h"
#include "../vn_environment.h"

extern HWND hWnd;

vnShader::vnShaderObject **vnShader::vshader = NULL;
vnShader::vnShaderObject **vnShader::pshader = NULL;

bool vnShader::initialize()
{
	//頂点シェーダの作成
	vshader = new vnShaderObject * [(unsigned int)eVertexShader::VS_MAX];
	if (vshader)
	{
		vshader[(unsigned int)eVertexShader::VS_3D] = new vnShaderObject(L"shader/vs_3d.cso");
		vshader[(unsigned int)eVertexShader::VS_3D_Skin] = new vnShaderObject(L"shader/vs_3d_skin.cso");
		vshader[(unsigned int)eVertexShader::VS_3D_Line] = new vnShaderObject(L"shader/vs_3d_line.cso");
		vshader[(unsigned int)eVertexShader::VS_2D] = new vnShaderObject(L"shader/vs_2d.cso");
		vshader[(unsigned int)eVertexShader::VS_Copy] = new vnShaderObject(L"shader/postprocess/vs_copy.cso");
		vshader[(unsigned int)eVertexShader::VS_Gray] = new vnShaderObject(L"shader/postprocess/vs_gray.cso");
	}

	//ピクセルシェーダの作成
	pshader = new vnShaderObject * [(unsigned int)ePixelShader::PS_MAX];
	if (pshader)
	{
		pshader[(unsigned int)ePixelShader::PS_3D] = new vnShaderObject(L"shader/ps_3d.cso");
		pshader[(unsigned int)ePixelShader::PS_3D_NoLight] = new vnShaderObject(L"shader/ps_3d_no_light.cso");
		pshader[(unsigned int)ePixelShader::PS_3D_Line] = new vnShaderObject(L"shader/ps_3d_line.cso");
		pshader[(unsigned int)ePixelShader::PS_2D] = new vnShaderObject(L"shader/ps_2d.cso");
		pshader[(unsigned int)ePixelShader::PS_Copy] = new vnShaderObject(L"shader/postprocess/ps_copy.cso");
		pshader[(unsigned int)ePixelShader::PS_Gray] = new vnShaderObject(L"shader/postprocess/ps_gray.cso");
	}
	return (vshader!=NULL && pshader!=NULL);
}

void vnShader::terminate()
{
	for (int i = 0; i < (unsigned int)eVertexShader::VS_MAX; i++)
	{
		delete vshader[i];
		vshader[i] = NULL;
	}
	delete[] vshader;
	vshader = NULL;

	for (int i = 0; i < (unsigned int)ePixelShader::PS_MAX; i++)
	{
		delete pshader[i];
		pshader[i] = NULL;
	}
	delete[] pshader;
	pshader = NULL;
}

vnShader::vnShaderObject::vnShaderObject(const WCHAR *path)
{
	code = NULL;
	length = 0;

	FILE *fp = NULL;
	if ((_wfopen_s(&fp, path, L"rb")) != 0)
	{
		WCHAR text[256];
		swprintf_s(text, L"cannot open file \"%s\"", path);
		MessageBox(hWnd, text, L"vnShader Load Error", MB_OK);
		assert(false);
		return;
	}
	fseek(fp, 0, SEEK_END);
	length = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	code = new BYTE[length];
	fread((void*)code, length, 1, fp);

	fclose(fp);
}

vnShader::vnShaderObject::~vnShaderObject()
{
	if(code)delete[] code;
}

