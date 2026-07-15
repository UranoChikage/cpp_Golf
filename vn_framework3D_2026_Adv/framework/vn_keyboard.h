//--------------------------------------------------------------//
//	"vn_keyboard.h"												//
//		キーボード入力(DirectInput)								//
//													2026/01/01	//
//														Ichii	//
//--------------------------------------------------------------//
#pragma once

class vnKeyboard
{
public:
	static bool on(BYTE key);	//オン状態(押している間)の取得
	static bool trg(BYTE key);	//トリガー(押した瞬間)の取得
	static bool rel(BYTE key);	//リリース(押した瞬間)の取得
};

/*

[参考]
DirectInputのキーコード

typedef enum {
    DIK_0,
    DIK_1,
    DIK_2,
    DIK_3,
    DIK_4,
    DIK_5,
    DIK_6,
    DIK_7,
    DIK_8,
    DIK_9,
    DIK_A,
    DIK_ABNT_C1,
    DIK_ABNT_C2,
    DIK_ADD,
    DIK_APOSTROPHE,
    DIK_APPS,
    DIK_AT,
    DIK_AX,
    DIK_B,
    DIK_BACK,
    DIK_BACKSLASH,
    DIK_C,
    DIK_CALCULATOR,
    DIK_CAPITAL,
    DIK_COLON,
    DIK_COMMA,
    DIK_CONVERT,
    DIK_D,
    DIK_DECIMAL,
    DIK_DELETE,
    DIK_DIVIDE,
    DIK_DOWN,
    DIK_E,
    DIK_END,
    DIK_EQUALS,
    DIK_ESCAPE,
    DIK_F,
    DIK_F1,
    DIK_F2,
    DIK_F3,
    DIK_F4,
    DIK_F5,
    DIK_F6,
    DIK_F7,
    DIK_F8,
    DIK_F9,
    DIK_F10,
    DIK_F11,
    DIK_F12,
    DIK_F13,
    DIK_F14,
    DIK_F15,
    DIK_G,
    DIK_GRAVE,
    DIK_H,
    DIK_HOME,
    DIK_I,
    DIK_INSERT,
    DIK_J,
    DIK_K,
    DIK_KANA,
    DIK_KANJI,
    DIK_L,
    DIK_LBRACKET,
    DIK_LCONTROL,
    DIK_LEFT,
    DIK_LMENU,
    DIK_LSHIFT,
    DIK_LWIN,
    DIK_M,
    DIK_MAIL,
    DIK_MEDIASELECT,
    DIK_MEDIASTOP,
    DIK_MINUS,
    DIK_MULTIPLY,
    DIK_MUTE,
    DIK_MYCOMPUTER,
    DIK_N,
    DIK_NEXT,
    DIK_NEXTTRACK,
    DIK_NOCONVERT,
    DIK_NUMLOCK,
    DIK_NUMPAD0,
    DIK_NUMPAD1,
    DIK_NUMPAD2,
    DIK_NUMPAD3,
    DIK_NUMPAD4,
    DIK_NUMPAD5,
    DIK_NUMPAD6,
    DIK_NUMPAD7,
    DIK_NUMPAD8,
    DIK_NUMPAD9,
    DIK_NUMPADCOMMA,
    DIK_NUMPADENTER,
    DIK_NUMPADEQUALS,
    DIK_O,
    DIK_OEM_102,
    DIK_P,
    DIK_PAUSE,
    DIK_PERIOD,
    DIK_PLAYPAUSE,
    DIK_POWER,
    DIK_PREVTRACK,
    DIK_PRIOR,
    DIK_Q,
    DIK_R,
    DIK_RBRACKET,
    DIK_RCONTROL,
    DIK_RETURN,
    DIK_RIGHT,
    DIK_RMENU,
    DIK_RSHIFT,
    DIK_RWIN,
    DIK_S,
    DIK_SCROLL,
    DIK_SEMICOLON,
    DIK_SLASH,
    DIK_SLEEP,
    DIK_SPACE,
    DIK_STOP,
    DIK_SUBTRACT,
    DIK_SYSRQ,
    DIK_T,
    DIK_TAB,
    DIK_U,
    DIK_UNDERLINE,
    DIK_UNLABELED,
    DIK_UP,
    DIK_V,
    DIK_VOLUMEDOWN,
    DIK_VOLUMEUP,
    DIK_W,
    DIK_WAKE,
    DIK_WEBBACK,
    DIK_WEBFAVORITES,
    DIK_WEBFORWARD,
    DIK_WEBHOME,
    DIK_WEBREFRESH,
    DIK_WEBSEARCH,
    DIK_WEBSTOP,
    DIK_X,
    DIK_Y,
    DIK_YEN,
    DIK_Z
} Keyboard Device;


値       マクロ名	    シンボル	備考
0x01    DIK_ESCAPE	    Esc
0x02	DIK_1	        1
0x03	DIK_2	        2
0x04	DIK_3	        3
0x05	DIK_4	        4
0x06	DIK_5	        5
0x07	DIK_6	        6
0x08	DIK_7	        7
0x09	DIK_8	        8
0x0A	DIK_9	        9
0x0B	DIK_0	        0
0x0C	DIK_MINUS	    -
0x0D	DIK_EQUALS	    =
0x0E	DIK_BACK	    Back Space
0x0F	DIK_TAB	        Tab
0x10	DIK_Q	        Q
0x11	DIK_W	        W
0x12	DIK_E	        E
0x13	DIK_R	        R
0x14	DIK_T	        T
0x15	DIK_Y	        Y
0x16	DIK_U	        U
0x17	DIK_I	        I
0x18	DIK_O	        O
0x19	DIK_P	        P
0x1A	DIK_LBRACKET	[
0x1B	DIK_RBRACKET	]
0x1C	DIK_RETURN	    Enter
0x1D	DIK_LContol	    Ctrl (左)
0x1E	DIK_A	        A
0x1F	DIK_S	        S
0x20	DIK_D	        D
0x21	DIK_F	        F
0x22	DIK_G	        G
0x23	DIK_H	        H
0x24	DIK_J	        J
0x25	DIK_K	        K
0x26	DIK_L	        L
0x27	DIK_SEMICOLON	;
0x28	DIK_APOSTROPHE	'
0x29	DIK_GRAVE	    `
0x2A	DIK_LSHIFT	    Shift (左)
0x2B	DIK_BACKSLASH	\
0x2C	DIK_Z	        Z
0x2D	DIK_X	        X
0x2E	DIK_C	        C
0x2F	DIK_V	        V
0x30	DIK_B	        B
0x31	DIK_N	        N
0x32	DIK_M	        M
0x33	DIK_COMMA	    ,
0x34	DIK_PERIOD	    .
0x35	DIK_SLASH	    /
0x36	DIK_RSHIFT	    Shift (右)
0x37	DIK_MULTIPLY	* (Numpad)
0x38	DIK_LMENU	    Alt (左)
0x39	DIK_SPACE	    Space
0x3A	DIK_CAPITAL	    Caps Lock
0x3B	DIK_F1	        F1
0x3C	DIK_F2	        F2
0x3D	DIK_F3	        F3
0x3E	DIK_F4	        F4
0x3F	DIK_F5	        F5
0x40	DIK_F6	        F6
0x41	DIK_F7	        F7
0x42	DIK_F8	        F8
0x43	DIK_F9	        F9
0x44	DIK_F10	        F10
0x45	DIK_NUMLOCK	    Num Lock
0x46	DIK_SCROLL	    Scroll Lock
0x47	DIK_NUMPAD7	    7 (Numpad)
0x48	DIK_NUMPAD8	    8 (Numpad)
0x49	DIK_NUMPAD9	    9 (Numpad)
0x4A	DIK_SUBTRACT	- (Numpad)
0x4B	DIK_NUMPAD4	    4 (Numpad)
0x4C	DIK_NUMPAD5	    5 (Numpad)
0x4D	DIK_NUMPAD6	    6 (Numpad)
0x4E	DIK_ADD	        + (Numpad)
0x4F	DIK_NUMPAD1	    1 (Numpad)
0x50	DIK_NUMPAD2	    2 (Numpad)
0x51	DIK_NUMPAD3	    3 (Numpad)
0x52	DIK_NUMPAD0	    0 (Numpad)
0x53	DIK_DECIMAL	    . (Numpad)
0x57	DIK_F11	        F11
0x58	DIK_F12	        F12
0x64	DIK_F13	        F13	NEC PC-98
0x65	DIK_F14	        F14	NEC PC-98
0x66	DIK_F15	        F15	NEC PC-98
0x70	DIK_KANA	    カナ	日本語キーボード
0x79	DIK_CONVERT	    変換	日本語キーボード
0x7B	DIK_NOCONVERT	無変換	日本語キーボード
0x7D	DIK_YEN	        ¥	日本語キーボード
0x8D	DIK_NUMPADEQUALS	=	NEC PC-98
0x90	DIK_CIRCUMFLEX	^	日本語キーボード
0x91	DIK_AT	        @	NEC PC-98
0x92	DIK_COLON	    :	NEC PC-98
0x93	DIK_UNDERLINE	_	NEC PC-98
0x94	DIK_KANJI	    漢字	日本語キーボード
0x95	DIK_STOP	    Stop	NEC PC-98
0x96	DIK_AX	        (Japan AX)
0x97	DIK_UNLABELED	(J3100)
0x9C	DIK_NUMPADENTER	Enter (Numpad)
0x9D	DIK_RCONTROL	Ctrl (右)
0xB3	DIK_NUMPADCOMMA	, (Numpad)	NEC PC-98
0xB5	DIK_DIVIDE	    / (Numpad)
0xB7	DIK_SYSRQ	    Sys Rq
0xB8	DIK_RMENU	    Alt (右)
0xC5	DIK_PAUSE	    Pause
0xC7	DIK_HOME	    Home
0xC8	DIK_UP	        ↑
0xC9	DIK_PRIOR	    Page Up
0xCB	DIK_LEFT	    ←
0xCD	DIK_RIGHT	    →
0xCF	DIK_END	        End
0xD0	DIK_DOWN	    ↓
0xD1	DIK_NEXT	    Page Down
0xD2	DIK_INSERT	    Insert
0xD3	DIK_DELETE	    Delete
0xDB	DIK_LWIN	    Windows
0xDC	DIK_RWIN	    Windows
0xDD	DIK_APPS	    Menu
0xDE	DIK_POWER	    Power
0xDF	DIK_SLEEP	    Windows
*/
