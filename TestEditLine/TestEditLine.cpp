// TestEditLine.cpp : ����Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "TestEditLine.h"
#pragma comment( lib, "Imm32.lib")
class CLineEdit
{
public:
	CLineEdit( HWND wndParent, int x, int y, int width, int height );
	~CLineEdit();
	//�ַ�����ӿڡ�Ϊ�˼򻯣���������λ�ã���֧��׷���ַ����˸񣬹��λ��ʼ�����ַ������
	void pushChar( WCHAR ch );
private:
	WCHAR	m_input[256];	//������ַ�������Ϊ�����ӣ����Զ��˸���󳤶ȡ�
	INT		m_length;		//�Ѿ�������ַ�����
	HWND	m_wndParent;	//�ı���������ĸ�������
	HDC		m_dcParent;		//���ƴ��ڵ� DC
	RECT	m_texBox;		//�ı��������
};

CLineEdit::CLineEdit( HWND wndParent, int x, int y, int width, int height )
{
	m_wndParent		= wndParent;			//���ù��캯��ʱ��Ҫָ�����ƴ��ڡ�
	m_dcParent		= GetDC( m_wndParent );	//�õ����ڶ�Ӧ�� DC��֮���ʹ�� GDI �����ı���
	m_texBox.left	= x;
	m_texBox.top	= y;
	m_texBox.right	= x + width;
	m_texBox.bottom	= y + height;
	memset( m_input, 0, sizeof( m_input ) );
	m_length		= 0;
	//��Щ��������һ��Ҫ�ڹ��캯���г�ʼ����Ҳ����д�����ĺ��������á�
}

CLineEdit::~CLineEdit()
{
	if ( m_dcParent ) ReleaseDC( m_wndParent, m_dcParent );
}

void CLineEdit::pushChar( WCHAR ch )
{
	if ( ch == VK_BACK )	//���������˸������ɾ�����һ���ַ���
	{
		if ( m_length == 0 ) return;
		m_input[--m_length]	= 0;
	}
	else if ( ch )			//�����ַ�����׷�ӵ��ַ���ĩβ
	{
		if ( m_length == ARRAYSIZE( m_input ) ) return;
		m_input[m_length++]	= ch;
	}
	//����һ�����ο򣬱�ʾ�ı����λ�á�
	FillRect( m_dcParent, &m_texBox, (HBRUSH)GetStockObject( WHITE_BRUSH ) );
	Rectangle( m_dcParent, m_texBox.left, m_texBox.top, m_texBox.right, m_texBox.bottom );
	//�����ı��Ļ������򣬸��߿������������صľ��롣
	RECT	texCalc	= { m_texBox.left + 4, m_texBox.top + 2, m_texBox.right - 4, m_texBox.bottom - 2 };
	RECT	texDraw	= texCalc;	//���ڼ������ֵ�ʵ�ʻ�������ʱ������� texCalc �е�ֵ���������ﱸ��һ����
	//�������ֵ�ʵ�ʻ���λ�á�����û�д��������ȵ������������ľ����ұ߽���Ϊ����λ�á�
	DrawText( m_dcParent, m_input, m_length, &texCalc, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_CALCRECT );
	//�������֣���Ϊ���֮ǰ�Ľ�� texCalc �����μ���ᵼ������ı䣬��������ʹ�ñ��ݵĻ������� texDraw��
	DrawText( m_dcParent, m_input, m_length, &texDraw, DT_LEFT | DT_VCENTER | DT_SINGLELINE );
	//ȡ�õ�ǰ���ڹ��������뷨���
	HIMC	imc	= ImmGetContext( m_wndParent );
	//�������뷨��ʾ���λ��
	COMPOSITIONFORM cf;
	cf.dwStyle = CFS_FORCE_POSITION;	//����������������ѡ����ϸ��˵����鿴 MSDN ���ĵ���
	cf.ptCurrentPos.x = texCalc.right;
	cf.ptCurrentPos.y = texCalc.top;
	ImmSetCompositionWindow( imc, &cf );	//���� API��ʹ������Ч��
	//�������뷨������ѡ����λ��
	CANDIDATEFORM	candf;
	candf.dwIndex	= 0;				// 0 ��ѡ���һЩ���ϵ����뷨�ж��ѡ������ڵĺܶ����뷨ѡ����Ѿ�����ʾ��ϲ���
	candf.dwStyle	= CFS_CANDIDATEPOS;	// ����������������ѡ����ϸ��˵����鿴 MSDN ���ĵ���
	candf.ptCurrentPos.x = texCalc.right;
	candf.ptCurrentPos.y = texCalc.bottom;
	candf.rcArea	= m_texBox;			// �����ų�����������ѡ���������ʾ����������ڡ�
	ImmSetCandidateWindow( imc, &candf );	//���� API��ʹ������Ч��
	//Ϊ�˴���򵥣����ﲢ��������˸�Ĺ�꣬�����������ڶ�ʱ���л��ƣ�����ʹ�� SetCaretPos/ShowCaret/HideCaret  API��
	//ע�� SetCaretPos ��һ�� API �������� DirectUI���������ջ���Ҫ�Լ����ơ�
}

CLineEdit*	lineEdit	= NULL;

#define MAX_LOADSTRING 100

// ȫ�ֱ���: 
HINSTANCE hInst;                                // ��ǰʵ��
WCHAR szTitle[MAX_LOADSTRING];                  // �������ı�
WCHAR szWindowClass[MAX_LOADSTRING];            // ����������

// �˴���ģ���а����ĺ�����ǰ������: 
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

    // TODO: �ڴ˷��ô��롣

    // ��ʼ��ȫ���ַ���
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_TESTEDITLINE, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // ִ��Ӧ�ó����ʼ��: 
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TESTEDITLINE));

    MSG msg;

    // ����Ϣѭ��: 
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
//  ����: MyRegisterClass()
//
//  Ŀ��: ע�ᴰ���ࡣ
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
//   ����: InitInstance(HINSTANCE, int)
//
//   Ŀ��: ����ʵ�����������������
//
//   ע��: 
//
//        �ڴ˺����У�������ȫ�ֱ����б���ʵ�������
//        ��������ʾ�����򴰿ڡ�
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // ��ʵ������洢��ȫ�ֱ�����

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }
   //���� LineEdit ����
   lineEdit	= new CLineEdit( hWnd, 100, 100, 400, 30 );

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  ����: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  Ŀ��:    ���������ڵ���Ϣ��
//
//  WM_COMMAND  - ����Ӧ�ó���˵�
//  WM_PAINT    - ����������
//  WM_DESTROY  - �����˳���Ϣ������
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // �����˵�ѡ��: 
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
            // TODO: �ڴ˴����ʹ�� hdc ���κλ�ͼ����...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
	case WM_ACTIVATE:	//���ڱ�������ý���ʱ�������������뷨���ڵ�λ�á�
	case WM_SETFOCUS:	//Ϊ�˼򻯴��룬�����ǵ��� pushChar ����һ��ֵΪ 0 ���ַ���
		lineEdit->pushChar( 0 );
		break;
	case WM_CHAR:		//����Ӣ�ĵ��ַ�ʱ������ WM_CHAR ��Ϣ��
		lineEdit->pushChar( wParam );
		break;
	case WM_IME_CHAR:	//ʹ�����뷨�����ַ�ʱ������ WM_IME_CHAR ��Ϣ���������ʱ�������ж��ٸ��ַ��ʹ������ٴΡ�
		//�ر�ע�⣺����������� unicode ����ģ�WM_IME_CHAR ��Ϣ�ᱻϵͳ�޸�Ϊ WM_CHAR��
		//���� wParam ������ʾ���ַ���ʹ��ϵͳ�ı��ر������ã����������ϵͳ��ʹ�� GBK ���롣
		lineEdit->pushChar( wParam );
		break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// �����ڡ������Ϣ�������
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
