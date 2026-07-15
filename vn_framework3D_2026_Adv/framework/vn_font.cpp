//--------------------------------------------------------------//
//	"vn_Font.cpp"												//
//		フォント描画											//
//													2026/01/01	//
//														Ichii	//
//--------------------------------------------------------------//
#include "../framework.h"
#include "vn_environment.h"

WCHAR vnFont::fontName[256][64];
int vnFont::fontNum = 0;

ID2D1SolidColorBrush	*vnFont::pBrush = NULL;
IDWriteTextFormat		*vnFont::pTextFormat = NULL;
D2D1_RECT_F				vnFont::rect;

float		vnFont::Size = 16.0f;
float		vnFont::X = 0.0f;
float		vnFont::Y = 0.0f;
DWORD		vnFont::Color = 0xffffffff;

vnFont::stStringInfo	vnFont::strInfo[vnFontSTRING_MAX];
int			vnFont::registerNum=0;
int			vnFont::registerCount = 0;
WCHAR		*vnFont::allocPtr=NULL;
WCHAR		*vnFont::currentPtr=NULL;

IDWriteTextFormat *vnFont::currentTextFormat = NULL;

bool vnFont::initialize()
{
	registerNum = 0;
	registerCount = 0;
	allocPtr = new WCHAR[vnFontCHARACTER_MAX];
	currentPtr = allocPtr;

	HRESULT hr;

	//使用できるフォント名の取得
	memset(fontName, 0, sizeof(fontName));
	IDWriteFontCollection* fonts = NULL;
	hr = vnDirect2D::getDWFactory()->GetSystemFontCollection(&fonts);
	if (hr == S_OK)
	{
		WCHAR locale[LOCALE_NAME_MAX_LENGTH];
		GetUserDefaultLocaleName(locale, _countof(locale));
		unsigned int const count = fonts->GetFontFamilyCount();
		for (unsigned int familyIndex = 0; familyIndex != count && fontNum < vnFontNUM_MAX; familyIndex++)
		{
			IDWriteFontFamily* family = NULL;
			fonts->GetFontFamily(familyIndex, &family);
			IDWriteLocalizedStrings* names = NULL;
			hr = family->GetFamilyNames(&names);
			if (hr != S_OK)continue;
			unsigned int nameIndex = 0;
			BOOL exists = false;
			hr = names->FindLocaleName(locale, &nameIndex, &exists);
			if (hr == S_OK && exists == TRUE)
			{
				hr = names->GetString(nameIndex, fontName[fontNum], _countof(fontName[fontNum]));
				//vnFont::output(L"%s\n", fontName[fontNum]);
				fontNum++;
			}
			family->Release();
			names->Release();
		}
		fonts->Release();
	}

	//ブラシの生成
	vnDirect2D::getDeviceContext()->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &pBrush);
	if (hr != S_OK)
	{
		return false;
	}

	//フォントの作成
	pTextFormat = create(L"Meiryo", (int)Size);
	if (pTextFormat == NULL)
	{
		return false;
	}
	currentTextFormat = pTextFormat;

	rect.left = 0.0f;
	rect.top = 0.0f;
	rect.right = (float)vnMainFrame::screenWidth;
	rect.bottom = (float)vnMainFrame::screenHeight;
	
	return true;
}

void vnFont::terminate()
{
	delete[] allocPtr;
	SAFE_RELEASE(pTextFormat);
	SAFE_RELEASE(pBrush);
}

void vnFont::registerString(const WCHAR *string, UINT32 count)
{
	//return;
#pragma warning(push)
#pragma warning(disable:4311)
#pragma warning(disable:4302)
	if(string==NULL)return;
	if(registerNum>=vnFontSTRING_MAX)return;
	if (registerCount+count >= vnFontCHARACTER_MAX)return;
	strInfo[registerNum].x = X;
	strInfo[registerNum].y = Y;
	strInfo[registerNum].color = Color;
	strInfo[registerNum].ptr = currentPtr;
	strInfo[registerNum].count = count;
	strInfo[registerNum].textFormat = currentTextFormat;
	wcsncpy_s((WCHAR*)currentPtr, vnFontCHARACTER_MAX-(reinterpret_cast<int>(currentPtr)- reinterpret_cast<int>(allocPtr)), string, count);
	currentPtr+=count;
	registerNum++;
	registerCount += count;
#pragma warning(pop)
}

void vnFont::renderString()
{
	if(pTextFormat==NULL)return;

#if 1
	static float reciprocal = 1.0f / 255.0f;
	for(int i=0; i<registerNum; i++)
	{
		//描画位置の設定
		rect.left = strInfo[i].x;
		rect.top = strInfo[i].y;

		//描画色の設定
		if (!i || Color != strInfo[i].color)
		{
			Color = strInfo[i].color;

			D2D1_COLOR_F col;
			col.r = (float)((Color & 0x00ff0000) >> 16) * reciprocal;
			col.g = (float)((Color & 0x0000ff00) >>  8) * reciprocal;
			col.b = (float)((Color & 0x000000ff) >>  0) * reciprocal;
			col.a = (float)((Color & 0xff000000) >> 24) * reciprocal;
			pBrush->SetColor(&col);
		}

		//テキストの描画
		vnDirect2D::getDeviceContext()->DrawTextW(
			strInfo[i].ptr,
			strInfo[i].count,
			strInfo[i].textFormat,
			&rect,
			pBrush
		);
	}
#endif
	registerNum = 0;
	registerCount = 0;
	currentPtr = allocPtr;
	return;
}

//使用できるフォント数の取得
int vnFont::getFontNum(void)
{
	return fontNum;
}

//使用できるフォント名の取得
WCHAR* vnFont::getFontName(int id)
{
	if (id < 0 || id >= fontNum)return NULL;
	return fontName[id];
}

//フォントの作成
IDWriteTextFormat *vnFont::create(const WCHAR *fontname, int size)
{
	HRESULT hr;
	IDWriteTextFormat* ret = NULL;

	//テキストフォーマットの生成
	hr = vnDirect2D::getDWFactory()->CreateTextFormat(
		fontname,					//フォント ファミリの名前を含む文字配列。
		NULL,						//フォント コレクション オブジェクトへのポインター。これが NULL の場合、システム フォント コレクションを示します。
		DWRITE_FONT_WEIGHT_NORMAL,	//このメソッドによって作成されるテキスト オブジェクトのフォントの太さを示す値。
		DWRITE_FONT_STYLE_NORMAL,	//このメソッドによって作成されるテキスト オブジェクトのフォント スタイルを示す値。
		DWRITE_FONT_STRETCH_NORMAL,	//このメソッドによって作成されるテキスト オブジェクトのフォント伸縮を示す値。
		(float)size,				//DIP ("デバイス非依存ピクセル") 単位でのフォントの論理サイズ。1 DIP は 1/96 インチです。
		L"",						//ロケール名を含む文字配列。
		&ret						//このメソッドが返されるときに、新しく作成されたテキスト形式オブジェクトへのポインターのアドレスが格納されます。失敗した場合は NULL が格納されます。
	);
	return ret;
}

void vnFont::setPos(float x, float y)
{
	X = x;
	Y = y;
}

void vnFont::setPosX(float x)
{
	X = x;
}

void vnFont::setPosY(float y)
{
	Y = y;
}

void vnFont::setColor(DWORD color)
{
	Color = color;
}

void vnFont::setTextFormat(IDWriteTextFormat* tf)
{
	currentTextFormat = (tf == NULL ? pTextFormat : tf);
}

float vnFont::getPosX()
{
	return X;
}

float vnFont::getPosY()
{
	return Y;
}

DWORD vnFont::getColor()
{
	return Color;
}

//文字列の描画(ワイド文字)
void vnFont::print(float x, float y, DWORD color, const WCHAR *string, ...)
{
	if (string == NULL)return;

	X = x;
	Y = y;
	Color = color;

	va_list	va;
	va_start(va, string);
	WCHAR buf[0x100];
	vswprintf_s(buf, string, va);
	va_end(va);

	registerString(buf, (UINT32)wcslen(buf));
}

void vnFont::print(float x, float y, const WCHAR *string, ...)
{
	if (string == NULL)return;

	X = x;
	Y = y;

	va_list	va;
	va_start(va, string);
	WCHAR buf[0x100];
	vswprintf_s(buf, string, va);
	va_end(va);

	registerString(buf, (UINT32)wcslen(buf));
}

void vnFont::print(const WCHAR *string, ...)
{
	if (string == NULL)return;

	va_list	va;
	va_start(va, string);
	WCHAR buf[0x100];
	vswprintf_s(buf, string, va);
	va_end(va);

	registerString(buf, (UINT32)wcslen(buf));
}

//文字列の描画(ワイド文字/文字数指定付き)
void vnFont::printCount(float x, float y, DWORD color, int count, const WCHAR *string, ...)
{
	if (string == NULL)return;

	X = x;
	Y = y;
	Color = color;

	va_list	va;
	va_start(va, string);
	WCHAR buf[0x100];
	vswprintf_s(buf, string, va);
	va_end(va);

	registerString(buf, (UINT32)count);
}

void vnFont::printCount(float x, float y, int count, const WCHAR *string, ...)
{
	if (string == NULL)return;

	X = x;
	Y = y;

	va_list	va;
	va_start(va, string);
	WCHAR buf[0x100];
	vswprintf_s(buf, string, va);
	va_end(va);

	registerString(buf, (UINT32)count);
}

void vnFont::printCount(int count, const WCHAR *string, ...)
{
	if (string == NULL)return;

	va_list	va;
	va_start(va, string);
	WCHAR buf[0x100];
	vswprintf_s(buf, string, va);
	va_end(va);

	registerString(buf, (UINT32)count);
}

//標準出力への文字列描画(ワイド文字)(ワイド文字)
void vnFont::output(const WCHAR *string, ...)
{
	if (string == NULL)return;

	va_list	va;
	va_start(va, string);
	WCHAR buf[0x100];
	vswprintf_s(buf, string, va);
	va_end(va);

	OutputDebugStringW(buf);
}
