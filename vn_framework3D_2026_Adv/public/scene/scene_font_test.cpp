//--------------------------------------------------------------//
//	"scene_font_test.cpp"										//
//		シーン：フォントテスト									//
//													2026/01/01	//
//														Ichii	//
//--------------------------------------------------------------//
#include "../../framework.h"
#include "../../framework/vn_environment.h"

//参考URL
//https://docs.microsoft.com/ja-jp/archive/msdn-magazine/2013/november/windows-with-c-exploring-fonts-with-directwrite-and-modern-c

#define FONT_NAME_X			(40.0f)
#define FONT_NAME_Y			(40.0f)
#define FONT_NAME_LINE		(26.0f)
#define SAMPLE_TEXT_X		(640.0f)
#define SAMPLE_TEXT_Y		(40.0f)
#define SAMPLE_TEXT_LINE	(40.0f)

/*

----自身で複数のフォントを作成し、文字列描画に使用するサンプル----

vnFontクラスが、PCにインストールされているフォントを調べてフォント名を保持しています。
※日本語ロケールを持つフォントに限ります。

vnFont::create()関数を呼ぶことで、自身でフォント名を指定して作成、以降vnFont::print()関数で使うことができます。

フォントを使用するためには、IDWriteTextFormatのポインタが必要になります。(create()関数の戻り値)
フォントの作成に失敗した場合は、NULLが返され、そのフォントは使用することができません。

指定したいフォントで描画する文字列をprint()する前に、vnFont::setTextFormat()でフォントを指定しておきます。
一度指定すると、以降のprint()はすべてそのフォントで描画されます。

デフォルトのフォントに戻す場合は、vnFont::setTextFormat()を引数なしで呼び出します。

【重要】
使用しなくなった自作フォントは、SAFE_RELEASE()マクロを使い、必ず削除しておく必要があります。

*/

//初期化
bool SceneFontTest::initialize()
{
	Cursor = 0;

	//使用できるフォントの数を取得
	FontNum = vnFont::getFontNum();

	//作成したフォント用の情報を確保しておく変数を必要な数作成
	textFormat = new IDWriteTextFormat * [FontNum];

	for (int i = 0; i < FontNum; i++)
	{
		//フォント名とサイズを指定してフォントを作成(フォント名は直接指定することも可能)
		textFormat[i] = vnFont::create(vnFont::getFontName(i), 24);
	}

	return true;
}

//終了
void SceneFontTest::terminate()
{
	//描画フォントをデフォルトに戻す
	vnFont::setTextFormat();

	for (int i = 0; i < FontNum; i++)
	{
		//作成したフォントを削除する
		SAFE_RELEASE(textFormat[i]);
	}
	//作成したフォント用の情報を確保しておく変数を削除
	delete[] textFormat;
}

//処理関数
void SceneFontTest::execute()
{
	if(vnKeyboard::trg(DIK_UP) && --Cursor<0)
	{	//↑キーでカーソルを上に移動
		Cursor = FontNum -1;
	}
	else if(vnKeyboard::trg(DIK_DOWN) && ++Cursor>= FontNum)
	{	//↓キーでカーソルを下に移動
		Cursor = 0;
	}

	//フォント名の描画
	for (int i = 0; i < FontNum; i++)
	{
		//選択/非選択によって文字の色を変える
		DWORD col = i == Cursor ? 0xffffff80 : 0xffffffff;
		//これから描画(vnFont::print)するフォントを指定する
		vnFont::setTextFormat(textFormat[i]);
		//指定された情報でフォント名を描画する
		vnFont::print(FONT_NAME_X, FONT_NAME_Y + FONT_NAME_LINE * (float)i, col, vnFont::getFontName(i));
	}

	//選択されたフォントでサンプル文字列の描画を行う

	int line = 0;
	//これから描画(vnFont::print)するフォントを指定する
	vnFont::setTextFormat(textFormat[Cursor]);
	//選択フォント名
	vnFont::print(SAMPLE_TEXT_X, SAMPLE_TEXT_Y + SAMPLE_TEXT_LINE * line++, 0xffffffff, L"\"%s\"", vnFont::getFontName(Cursor));
	//サンプル文字列①
	vnFont::print(SAMPLE_TEXT_X, SAMPLE_TEXT_Y + SAMPLE_TEXT_LINE * line++, 0xffffffff, L"【サンプル文字列】");
	//サンプル文字列②
	vnFont::print(SAMPLE_TEXT_X, SAMPLE_TEXT_Y + SAMPLE_TEXT_LINE * line++, 0xffffffff, L"vn_framework_2026_Adv - Font Test");

	//現在時刻を取得して表示する
	time_t now = time(NULL);
	struct tm pnow;
	localtime_s(&pnow, &now);
	WCHAR week[][3] = { L"日",L"月",L"火",L"水",L"木",L"金",L"土" };

	//年月日
	vnFont::print(SAMPLE_TEXT_X, SAMPLE_TEXT_Y + SAMPLE_TEXT_LINE * line++, 0xffffffff, L"%2d年%2d月%2d日(%s)",
		pnow.tm_year + 1900,
		pnow.tm_mon + 1,
		pnow.tm_mday,
		week[pnow.tm_wday]);

	//現在時刻
	vnFont::print(SAMPLE_TEXT_X, SAMPLE_TEXT_Y + SAMPLE_TEXT_LINE * line++, 0xffffffff, L"%d:%d:%d", 
		pnow.tm_hour, 
		pnow.tm_min, 
		pnow.tm_sec);

	vnScene::execute();
}

//描画関数
void SceneFontTest::render()
{
	vnScene::render();
}

