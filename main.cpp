#include <windows.h>
#include "commctrl.h"
#include <stdio.h>
#include <ctime>
#include <cmath>

#define BTN_CLICK 222 //������ ��������� � ������� ������
#define ListView_InsertColumnW(w,i,p) (int)SNDMSG((w),LVM_INSERTCOLUMNW,i,(WPARAM)(const LV_COLUMNW*)(p)) //������� ���������� ������ �� commctrl.h, ��� �� �� ������� � wchar_t
#define ListView_InsertItemW(w,i) (int)SNDMSG((w),LVM_INSERTITEMW,0,(WPARAM)(const LV_ITEMW*)(i)) //������� ���������� ������ �� commctrl.h, ��� �� �� ������� � wchar_t

HWND eQueryN; //������� (����������) ��� ���������� ���� ������ ������� N
HWND eQuantInt; //������� ��� ���������� ���� ���������� ���������� n
HWND eRangeA; //������� ��� ���������� ���� a
HWND eRangeB; //������� ��� ���������� ���� b
HWND eSigma; //������� ��� ���������� ���� Sigma
HWND eAlpha; //������� ��� ���������� ���� �����
HWND lvNorm; //������� ��� ������ ����������� �������������
HWND lvRavn; //������� ��� ������ ������������ �������������
HWND lvPokaz; //������� ��� ������ �������������� �������������


//�������� ������� �������� ������� (���������)
LRESULT CALLBACK WindowProcedure(HWND,UINT,WPARAM,LPARAM);

//������� ��� ���������� ������. ����������� ���� ��� ��� ������ ���������
void AddControls(HWND);

//������� ��� ��������� � ����������
int compare (const void * a, const void * b)
{
    if (*(double*)a > *(double*)b)
        return 1;
    else
        if (*(double*)a < *(double*)b)
            return -1;
        else
            return 0;
}

//����� ����� � ��������� ��� win32 api
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR args, int ncmdshow)
{
    srand((unsigned int)time(0)); //������������� ����
	WNDCLASSW wc = { 0 }; //����� ����
    //��������� ����
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW; //������� ����
	wc.hCursor = LoadCursor(NULL, IDC_ARROW); //������
	wc.hInstance = hInst; //������� (�������� �������)
	wc.lpszClassName = L"myWindowClass"; //�������� ������
	wc.lpfnWndProc = WindowProcedure; //������� ��� �������� ��������� (�������)
    if (!RegisterClassW(&wc)){ //����������� ������
        return -1;
    }
    CreateWindowW(TEXT(L"myWindowClass"),TEXT(L"�������������"), WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME | WS_VISIBLE, 100, 100, 500, 500, NULL, NULL, NULL, NULL); //�������� ����
    MSG msg = {0}; //������������� ������� ���������
    while (GetMessage(&msg,NULL, NULL, NULL)){ //��������� ���������
        TranslateMessage(&msg); //���������� ���������
        DispatchMessage(&msg); //��������� ��������� � ������� WindowProcedure
    }

    return 0;
}

//�������� ������ � ������
int CreateItemW(HWND hwndList, wchar_t *Text)
{
    LVITEMW lvi = {0}; //������������� ������� �������� ������
    lvi.mask = LVIF_TEXT; //��������� ������
    lvi.pszText = Text; //������������� ������
    return ListView_InsertItemW(hwndList, &lvi); //������� ������ � ������
}

//������� ������������ �������������
void ravnomRaspr(double a, double b, int N, double* mass)
{
    for (int i=0; i < N; i++) {
        mass[i] = a + rand() % 1000 * (b-a) / 1000.0 / (b-a);
    }
}

//������� ����������� �������������
void normRaspr(double a, double b, int N,  double* mass, int G)
{
    double* X = new double[N], S = 0.0;
    for (int i=0; i < N; i++) {
        for (int j=0; j < 12; j++) {
            mass[i] = rand() % 1000 / 1000.0 / (b-a);
            S += mass[i];
        }
        X[i] = S - 6;
        S = 0;
    }
}

//������� �������������� �������������
void pokazatRaspr(double a, double b, int N, double* mass, double la)
{
    for (int i=0; i < N; i++) {
        mass[i] = (log(rand() % 1000) / 1000.0 ) / la  / (b-a);
    }
}


LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp){
    switch(msg){
    case WM_COMMAND:{ //����������� ��� ������� �� ������, � �� ������
        switch(wp){ //� wp �������� ����� ������, ������� �� ������������� ��� ��������
        case BTN_CLICK:{ //����� ����� ������
            int N; //������ �������
            wchar_t* text = new wchar_t[20]; //�����
            GetWindowTextW(eQueryN, text, 20); //�������� ����� �� ����������
            N = _wtoi(text); //��������� ��� � ���
            double a, b; //����� � ������ ������� ��������� �������
            GetWindowTextW(eRangeA, text, 20); //�������� ����� �� ����������
            a = _wtof(text); //��������� ��� � �����
            GetWindowTextW(eRangeB, text, 20); //�������� ����� �� ����������
            b = _wtof(text); //��������� ��� � �����
            if (N>0){
                double *mass = new double[N]; //�������������� ������
                ravnomRaspr(a,b,N,mass); //��������� ��� ���������� ������������ �������������
                qsort(mass, N, sizeof(double), compare); //��������� ���������� ��������
                ListView_DeleteAllItems(lvRavn); //������� ������
                for (int i = 0; i < N; i++){
                    snwprintf(text,20,L"%f",mass[i]); //����������� ��� �������� � ������
                    CreateItemW(lvRavn,text); //� �������� �� � ������
                }
                delete [] mass; //�������� ������
                GetWindowTextW(eSigma, text, 20); //�������� ����� �� ����������
                int G = _wtoi(text); //��������� ��� � ���
                mass = new double[N]; //�������������� ������
                normRaspr(a,b,N,mass,G); //��������� ��� ���������� ����������� �������������
                qsort(mass, N, sizeof(double), compare); //��������� ���������� ��������
                ListView_DeleteAllItems(lvNorm); //������� ������
                for (int i = 0; i < N; i++){
                    snwprintf(text,20,L"%f",mass[i]); //����������� ��� �������� � ������
                    CreateItemW(lvNorm,text); //� �������� �� � ������
                }
                delete [] mass; //�������� ������
                GetWindowTextW(eAlpha, text, 20); //�������� ����� �� ����������
                double la = _wtof(text); //��������� ��� � �����
                mass = new double[N]; //�������������� ������
                pokazatRaspr(a,b,N,mass,la); //��������� ��� ���������� ����������� �������������
                qsort(mass, N, sizeof(double), compare); //��������� ���������� ��������
                ListView_DeleteAllItems(lvPokaz); //������� ������
                for (int i = 0; i < N; i++){
                    snwprintf(text,20,L"%f",mass[i]); //����������� ��� �������� � ������
                    CreateItemW(lvPokaz,text); //� �������� �� � ������
                }
                delete [] mass; //�������� ������
            }
            break;
        }
        }
        break;
    }
    case WM_CREATE: //����������� ��� ��������� ����
        if (wp == 0){ //wp = 0, ������ ��� ������ �������
            AddControls(hWnd); //������ ������, ������, ����������, � ������
        }
        break;
    case WM_DESTROY: //����������� ��� �������� ����
        PostQuitMessage(0); //���������� ��������� ��� ���������� ��������
        break;
    default:
        return DefWindowProcW(hWnd, msg, wp, lp);//���������� ���������
    }
}

//��������� ������� � ������
int CreateColumnW(HWND hwndLV, int iCol, wchar_t* Text, int iWidth)
{
    LVCOLUMNW lvc; //���������� �������
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM; //��������� �������
	lvc.fmt = LVCFMT_CENTER; //������������
	lvc.cx = iWidth; //������ �������
	lvc.pszText = Text; //������������� ������
	lvc.iSubItem = iCol; //���������� ����� �������
	return ListView_InsertColumnW(hwndLV, iCol, &lvc); //������� �������
}

void AddControls(HWND hWnd){
    CreateWindowExW(WS_EX_CLIENTEDGE|WS_EX_DLGMODALFRAME, L"STATIC", L"",
        WS_CHILD|WS_VISIBLE|SS_CENTER|SS_CENTERIMAGE|SS_SUNKEN, 4, 121, 481,41, hWnd,  NULL, NULL, NULL); //������ (�����) ������������ �������������
    CreateWindowExW(WS_EX_CLIENTEDGE|WS_EX_DLGMODALFRAME, L"STATIC", L"",
        WS_CHILD|WS_VISIBLE|SS_CENTER|SS_CENTERIMAGE|SS_SUNKEN, 4, 176, 481,41, hWnd,  NULL, NULL, NULL); //������ ������������ �������������
    CreateWindowW(L"static",L"����������� �������������", WS_VISIBLE | WS_CHILD, 17, 116, 199, 17, hWnd, NULL,NULL,NULL);
    CreateWindowW(L"static",L"������������� �������������", WS_VISIBLE | WS_CHILD, 17, 172, 212, 17, hWnd, NULL,NULL,NULL);
    CreateWindowW(L"static",L"������ ������� N:", WS_VISIBLE | WS_CHILD, 10, 16, 236, 17, hWnd, NULL,NULL,NULL);
    CreateWindowW(L"static",L"���������� ���������� n:", WS_VISIBLE | WS_CHILD, 10, 48, 236, 17, hWnd, NULL,NULL,NULL);
    CreateWindowW(L"static",L"���������� [a;b]:", WS_VISIBLE | WS_CHILD, 10, 80, 236, 17, hWnd, NULL,NULL,NULL); //������
    CreateWindowW(L"static",L"�����:", WS_VISIBLE | WS_CHILD, 15, 135, 118, 17, hWnd, NULL,NULL,NULL);
    CreateWindowW(L"static",L"�����:", WS_VISIBLE | WS_CHILD, 15, 190, 236, 17, hWnd, NULL,NULL,NULL);
    CreateWindowW(L"static",L"[", WS_VISIBLE | WS_CHILD, 256, 78, 15, 22, hWnd, NULL,NULL,NULL);
    CreateWindowW(L"static",L";", WS_VISIBLE | WS_CHILD, 365, 78, 15, 22, hWnd, NULL,NULL,NULL);
    CreateWindowW(L"static",L"]", WS_VISIBLE | WS_CHILD, 475, 78, 5, 22, hWnd, NULL,NULL,NULL);

    eQueryN = CreateWindowW(L"edit",L"",WS_VISIBLE | WS_CHILD, 255,9,235,22, hWnd, NULL, NULL, NULL);
    eQuantInt = CreateWindowW(L"edit",L"",WS_VISIBLE | WS_CHILD, 255,42,235,22, hWnd, NULL, NULL, NULL);
    eRangeA = CreateWindowW(L"edit",L"",WS_VISIBLE | WS_CHILD, 265,78,95,22, hWnd, NULL, NULL, NULL);
    eRangeB = CreateWindowW(L"edit",L"",WS_VISIBLE | WS_CHILD, 375,78,95,22, hWnd, NULL, NULL, NULL);  //����������
    eSigma = CreateWindowW(L"edit",L"",WS_VISIBLE | WS_CHILD, 255, 133, 225, 22, hWnd, NULL, NULL, NULL);
    eAlpha = CreateWindowW(L"edit",L"",WS_VISIBLE | WS_CHILD, 254,186,225,22, hWnd, NULL, NULL, NULL);

    CreateWindowW(L"button",L"���������", WS_VISIBLE | WS_CHILD, 8, 223, 477,21, hWnd, (HMENU)BTN_CLICK, NULL, NULL); //������, ������� ��������� ��������

    lvNorm = CreateWindow(WC_LISTVIEW, "",
         WS_VISIBLE|WS_BORDER|WS_CHILD | LVS_REPORT | LVS_EDITLABELS,
         8, 250, 156, 211,
         hWnd, NULL, NULL, 0);
    lvRavn = CreateWindow(WC_LISTVIEW, "",
         WS_VISIBLE|WS_BORDER|WS_CHILD | LVS_REPORT | LVS_EDITLABELS,
         168, 250, 156, 211,
         hWnd, NULL, NULL, 0); //������
    lvPokaz = CreateWindow(WC_LISTVIEW, "",
         WS_VISIBLE|WS_BORDER|WS_CHILD | LVS_REPORT | LVS_EDITLABELS,
         328, 250, 156, 211,
         hWnd, NULL, NULL, 0);
	CreateColumnW(lvNorm, 0, L"����������", 139);
	CreateColumnW(lvRavn, 0, L"�����������", 139);    //������� � ���
	CreateColumnW(lvPokaz, 0, L"�������������", 139);
}
