// TestEditLine.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "TestEditLine.h"
#pragma comment( lib, "Imm32.lib")
class CLineEdit
{
public:
	CLineEdit( HWND wndParent, int x, int y, int width, int height );
	~CLineEdit();
	//字符输入接口。为了简化，不处理光标位置，仅支持追加字符和退格，光标位置始终在字符串最后。
	void pushChar( WCHAR ch );
private:
	WCHAR	m_input[256];	//输入的字符串，因为是例子，所以定了个最大长度。
	INT		m_length;		//已经输入的字符数。
	HWND	m_wndParent;	//文本框绘制在哪个窗口上
	HDC		m_dcParent;		//绘制窗口的 DC
	RECT	m_texBox;		//文本框的区域
};

CLineEdit::CLineEdit( HWND wndParent, int x, int y, int width, int height )
{
	m_wndParent		= wndParent;			//调用构造函数时，要指定绘制窗口。
	m_dcParent		= GetDC( m_wndParent );	//得到窗口对应的 DC，之后会使用 GDI 绘制文本框。
	m_texBox.left	= x;
	m_texBox.top	= y;
	m_texBox.right	= x + width;
	m_texBox.bottom	= y + height;
	memset( m_input, 0, sizeof( m_input ) );
	m_length		= 0;
	//这些参数并不一定要在构造函数中初始化，也可以写其它的函数来设置。
}

CLineEdit::~CLineEdit()
{
	if ( m_dcParent ) ReleaseDC( m_wndParent, m_dcParent );
}

void CLineEdit::pushChar( WCHAR ch )
{
	if ( ch == VK_BACK )	//如果输入的退格键，就删除最后一个字符。
	{
		if ( m_length == 0 ) return;
		m_input[--m_length]	= 0;
	}
	else if ( ch )			//其它字符，就追加到字符串末尾
	{
		if ( m_length == ARRAYSIZE( m_input ) ) return;
		m_input[m_length++]	= ch;
	}
	//绘制一个矩形框，表示文本框的位置。
	FillRect( m_dcParent, &m_texBox, (HBRUSH)GetStockObject( WHITE_BRUSH ) );
	Rectangle( m_dcParent, m_texBox.left, m_texBox.top, m_texBox.right, m_texBox.bottom );
	//设置文本的绘制区域，给边框留出几个像素的距离。
	RECT	texCalc	= { m_texBox.left + 4, m_texBox.top + 2, m_texBox.right - 4, m_texBox.bottom - 2 };
	RECT	texDraw	= texCalc;	//由于计算文字的实际绘制区域时，会更改 texCalc 中的值，所以这里备份一个。
	//计算文字的实际绘制位置。这里没有处理超出长度的情况。计算出的矩形右边将作为光标的位置。
	DrawText( m_dcParent, m_input, m_length, &texCalc, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_CALCRECT );
	//绘制文字，因为如果之前的结果 texCalc 来两次计算会导致坐标改变，所以这里使用备份的绘制区域 texDraw。
	DrawText( m_dcParent, m_input, m_length, &texDraw, DT_LEFT | DT_VCENTER | DT_SINGLELINE );
	//取得当前窗口关联的输入法句柄
	HIMC	imc	= ImmGetContext( m_wndParent );
	//设置输入法提示框的位置
	COMPOSITIONFORM cf;
	cf.dwStyle = CFS_FORCE_POSITION;	//这里有其它参数可选，详细的说明请查看 MSDN 的文档。
	cf.ptCurrentPos.x = texCalc.right;
	cf.ptCurrentPos.y = texCalc.top;
	ImmSetCompositionWindow( imc, &cf );	//调用 API，使设置生效。
	//设置输入法的文字选择框的位置
	CANDIDATEFORM	candf;
	candf.dwIndex	= 0;				// 0 号选择框。一些古老的输入法有多个选择框，现在的很多输入法选择框已经和提示框合并。
	candf.dwStyle	= CFS_CANDIDATEPOS;	// 这里有其它参数可选，详细的说明请查看 MSDN 的文档。
	candf.ptCurrentPos.x = texCalc.right;
	candf.ptCurrentPos.y = texCalc.bottom;
	candf.rcArea	= m_texBox;			// 这是排除的区域，文字选择框不允许显示在这个区域内。
	ImmSetCandidateWindow( imc, &candf );	//调用 API，使设置生效。
	//为了代码简单，这里并不绘制闪烁的光标，光标可以自行在定时器中绘制，或者使用 SetCaretPos/ShowCaret/HideCaret  API。
	//注意 SetCaretPos 这一套 API 不能用于 DirectUI，所以最终还是要自己绘制。
}

CLineEdit*	lineEdit	= NULL;

#define MAX_LOADSTRING 100

// 全局变量: 
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名

// 此代码模块中包含的函数的前向声明: 
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 在此放置代码。

    // 初始化全局字符串
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_TESTEDITLINE, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 执行应用程序初始化: 
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TESTEDITLINE));

    MSG msg;

    // 主消息循环: 
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
	if ( lineEdit ) delete lineEdit;
    return (int) msg.wParam;
}



//
//  函数: MyRegisterClass()
//
//  目的: 注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TESTEDITLINE));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_TESTEDITLINE);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目的: 保存实例句柄并创建主窗口
//
//   注释: 
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 将实例句柄存储在全局变量中

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }
   //创建 LineEdit 对象
   lineEdit	= new CLineEdit( hWnd, 100, 100, 400, 30 );

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的:    处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 分析菜单选择: 
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: 在此处添加使用 hdc 的任何绘图代码...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
	case WM_ACTIVATE:	//窗口被激活或获得焦点时，立即设置输入法窗口的位置。
	case WM_SETFOCUS:	//为了简化代码，这里是调用 pushChar 输入一个值为 0 的字符。
		lineEdit->pushChar( 0 );
		break;
	case WM_CHAR:		//输入英文的字符时，触发 WM_CHAR 消息。
		lineEdit->pushChar( wParam );
		break;
	case WM_IME_CHAR:	//使用输入法输入字符时，触发 WM_IME_CHAR 消息。输入词组时，词组有多少个字符就触发多少次。
		//特别注意：如果程序本身不是 unicode 编码的，WM_IME_CHAR 消息会被系统修改为 WM_CHAR，
		//并且 wParam 参数表示的字符会使用系统的本地编码设置，如简体中文系统就使用 GBK 编码。
		lineEdit->pushChar( wParam );
		break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// “关于”框的消息处理程序。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
