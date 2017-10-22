#include <windows.h>
#include "commctrl.h"
#include <stdio.h>
#include <ctime>
#include <cmath>

#define BTN_CLICK 222 //индекс сообщения о нажатии кнопки
#define ListView_InsertColumnW(w,i,p) (int)SNDMSG((w),LVM_INSERTCOLUMNW,i,(WPARAM)(const LV_COLUMNW*)(p)) //немного измененный дефайн из commctrl.h, что бы он работал с wchar_t
#define ListView_InsertItemW(w,i) (int)SNDMSG((w),LVM_INSERTITEMW,0,(WPARAM)(const LV_ITEMW*)(i)) //немного измененный дефайн из commctrl.h, что бы он работал с wchar_t

HWND eQueryN; //хендлер (обработчик) для текстового поля Размер выборки N
HWND eQuantInt; //хендлер для текстового поля Количество интервалов n
HWND eRangeA; //хендлер для текстового поля a
HWND eRangeB; //хендлер для текстового поля b
HWND eSigma; //хендлер для текстового поля Sigma
HWND eAlpha; //хендлер для текстового поля Альфа
HWND lvNorm; //хендлер для списка нормального распределения
HWND lvRavn; //хендлер для списка равномерного распределения
HWND lvPokaz; //хендлер для списка показательного распределения


//основная функция принятия событий (сообщений)
LRESULT CALLBACK WindowProcedure(HWND,UINT,WPARAM,LPARAM);

//функция для добовления кнопок. срабатывает один раз при старте программы
void AddControls(HWND);

//функция для сравнения в сортировке
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

//точка входа в программу для win32 api
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR args, int ncmdshow)
{
    srand((unsigned int)time(0)); //инициализация сида
	WNDCLASSW wc = { 0 }; //класс окна
    //настройка окна
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW; //фоновый цвет
	wc.hCursor = LoadCursor(NULL, IDC_ARROW); //курсор
	wc.hInstance = hInst; //инстанс (дочерний процесс)
	wc.lpszClassName = L"myWindowClass"; //название класса
	wc.lpfnWndProc = WindowProcedure; //функция для принятия сообщений (событий)
    if (!RegisterClassW(&wc)){ //регистрация класса
        return -1;
    }
    CreateWindowW(TEXT(L"myWindowClass"),TEXT(L"Распределение"), WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME | WS_VISIBLE, 100, 100, 500, 500, NULL, NULL, NULL, NULL); //создание окна
    MSG msg = {0}; //инициализация пустого сообщения
    while (GetMessage(&msg,NULL, NULL, NULL)){ //получение сообщения
        TranslateMessage(&msg); //трансляция сообщения
        DispatchMessage(&msg); //пересылка сообщения в функцию WindowProcedure
    }

    return 0;
}

//добавить строку в список
int CreateItemW(HWND hwndList, wchar_t *Text)
{
    LVITEMW lvi = {0}; //инициализация пустого элемента списка
    lvi.mask = LVIF_TEXT; //настройки ячекйи
    lvi.pszText = Text; //инициализация текста
    return ListView_InsertItemW(hwndList, &lvi); //вставка строки в список
}

//функция равномерного распределения
void ravnomRaspr(double a, double b, int N, double* mass)
{
    for (int i=0; i < N; i++) {
        mass[i] = a + rand() % 1000 * (b-a) / 1000.0 / (b-a);
    }
}

//функция нормального распределения
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

//функция показательного распределения
void pokazatRaspr(double a, double b, int N, double* mass, double la)
{
    for (int i=0; i < N; i++) {
        mass[i] = (log(rand() % 1000) / 1000.0 ) / la  / (b-a);
    }
}


LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp){
    switch(msg){
    case WM_COMMAND:{ //срабатывает при нажатии на кнопки, и не только
        switch(wp){ //в wp хранится номер кнопки, которой ей присваивается при создании
        case BTN_CLICK:{ //номер нашей кнопки
            int N; //размер выборки
            wchar_t* text = new wchar_t[20]; //буфер
            GetWindowTextW(eQueryN, text, 20); //получаем текст из текстбокса
            N = _wtoi(text); //переводим его в инт
            double a, b; //левая и правая граница интервала выборки
            GetWindowTextW(eRangeA, text, 20); //получаем текст из текстбокса
            a = _wtof(text); //переводим его в флоат
            GetWindowTextW(eRangeB, text, 20); //получаем текст из текстбокса
            b = _wtof(text); //переводим его в флоат
            if (N>0){
                double *mass = new double[N]; //инициализируем массив
                ravnomRaspr(a,b,N,mass); //заполняем его значениями равномерного распределения
                qsort(mass, N, sizeof(double), compare); //сортируем полученные значения
                ListView_DeleteAllItems(lvRavn); //отищаем список
                for (int i = 0; i < N; i++){
                    snwprintf(text,20,L"%f",mass[i]); //преобразуем все значения в строку
                    CreateItemW(lvRavn,text); //и передаем ее в список
                }
                delete [] mass; //отчищаем массив
                GetWindowTextW(eSigma, text, 20); //получаем текст из текстбокса
                int G = _wtoi(text); //переводим его в инт
                mass = new double[N]; //инициализируем массив
                normRaspr(a,b,N,mass,G); //заполняем его значениями нормального распределения
                qsort(mass, N, sizeof(double), compare); //сортируем полученные значения
                ListView_DeleteAllItems(lvNorm); //отищаем список
                for (int i = 0; i < N; i++){
                    snwprintf(text,20,L"%f",mass[i]); //преобразуем все значения в строку
                    CreateItemW(lvNorm,text); //и передаем ее в список
                }
                delete [] mass; //отчищаем массив
                GetWindowTextW(eAlpha, text, 20); //получаем текст из текстбокса
                double la = _wtof(text); //переводим его в флоат
                mass = new double[N]; //инициализируем массив
                pokazatRaspr(a,b,N,mass,la); //заполняем его значениями нормального распределения
                qsort(mass, N, sizeof(double), compare); //сортируем полученные значения
                ListView_DeleteAllItems(lvPokaz); //отищаем список
                for (int i = 0; i < N; i++){
                    snwprintf(text,20,L"%f",mass[i]); //преобразуем все значения в строку
                    CreateItemW(lvPokaz,text); //и передаем ее в список
                }
                delete [] mass; //отчищаем массив
            }
            break;
        }
        }
        break;
    }
    case WM_CREATE: //срабатывает при отрисовке окна
        if (wp == 0){ //wp = 0, только при первом запуске
            AddControls(hWnd); //рисуем кнопки, лейблы, текстбоксы, и списки
        }
        break;
    case WM_DESTROY: //срабатывает при закрытии окна
        PostQuitMessage(0); //отправляет сообщение для завершения процесса
        break;
    default:
        return DefWindowProcW(hWnd, msg, wp, lp);//пропускает сообщение
    }
}

//добавляет столбец в список
int CreateColumnW(HWND hwndLV, int iCol, wchar_t* Text, int iWidth)
{
    LVCOLUMNW lvc; //объявления столбца
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM; //настройки столбца
	lvc.fmt = LVCFMT_CENTER; //выравнивание
	lvc.cx = iWidth; //ширина столбца
	lvc.pszText = Text; //инициализация текста
	lvc.iSubItem = iCol; //порядковый номер столбца
	return ListView_InsertColumnW(hwndLV, iCol, &lvc); //вставка столбца
}

void AddControls(HWND hWnd){
    CreateWindowExW(WS_EX_CLIENTEDGE|WS_EX_DLGMODALFRAME, L"STATIC", L"",
        WS_CHILD|WS_VISIBLE|SS_CENTER|SS_CENTERIMAGE|SS_SUNKEN, 4, 121, 481,41, hWnd,  NULL, NULL, NULL); //панель (рамка) равномерного распределения
    CreateWindowExW(WS_EX_CLIENTEDGE|WS_EX_DLGMODALFRAME, L"STATIC", L"",
        WS_CHILD|WS_VISIBLE|SS_CENTER|SS_CENTERIMAGE|SS_SUNKEN, 4, 176, 481,41, hWnd,  NULL, NULL, NULL); //панель равномерного распределения
    CreateWindowW(L"static",L"Равномерное распределение", WS_VISIBLE | WS_CHILD, 17, 116, 199, 17, hWnd, NULL,NULL,NULL);
    CreateWindowW(L"static",L"Показательное распределение", WS_VISIBLE | WS_CHILD, 17, 172, 212, 17, hWnd, NULL,NULL,NULL);
    CreateWindowW(L"static",L"Размер выборки N:", WS_VISIBLE | WS_CHILD, 10, 16, 236, 17, hWnd, NULL,NULL,NULL);
    CreateWindowW(L"static",L"Количество интервалов n:", WS_VISIBLE | WS_CHILD, 10, 48, 236, 17, hWnd, NULL,NULL,NULL);
    CreateWindowW(L"static",L"Промежуток [a;b]:", WS_VISIBLE | WS_CHILD, 10, 80, 236, 17, hWnd, NULL,NULL,NULL); //лейблы
    CreateWindowW(L"static",L"Сигма:", WS_VISIBLE | WS_CHILD, 15, 135, 118, 17, hWnd, NULL,NULL,NULL);
    CreateWindowW(L"static",L"Альфа:", WS_VISIBLE | WS_CHILD, 15, 190, 236, 17, hWnd, NULL,NULL,NULL);
    CreateWindowW(L"static",L"[", WS_VISIBLE | WS_CHILD, 256, 78, 15, 22, hWnd, NULL,NULL,NULL);
    CreateWindowW(L"static",L";", WS_VISIBLE | WS_CHILD, 365, 78, 15, 22, hWnd, NULL,NULL,NULL);
    CreateWindowW(L"static",L"]", WS_VISIBLE | WS_CHILD, 475, 78, 5, 22, hWnd, NULL,NULL,NULL);

    eQueryN = CreateWindowW(L"edit",L"",WS_VISIBLE | WS_CHILD, 255,9,235,22, hWnd, NULL, NULL, NULL);
    eQuantInt = CreateWindowW(L"edit",L"",WS_VISIBLE | WS_CHILD, 255,42,235,22, hWnd, NULL, NULL, NULL);
    eRangeA = CreateWindowW(L"edit",L"",WS_VISIBLE | WS_CHILD, 265,78,95,22, hWnd, NULL, NULL, NULL);
    eRangeB = CreateWindowW(L"edit",L"",WS_VISIBLE | WS_CHILD, 375,78,95,22, hWnd, NULL, NULL, NULL);  //текстбоксы
    eSigma = CreateWindowW(L"edit",L"",WS_VISIBLE | WS_CHILD, 255, 133, 225, 22, hWnd, NULL, NULL, NULL);
    eAlpha = CreateWindowW(L"edit",L"",WS_VISIBLE | WS_CHILD, 254,186,225,22, hWnd, NULL, NULL, NULL);

    CreateWindowW(L"button",L"Вычислить", WS_VISIBLE | WS_CHILD, 8, 223, 477,21, hWnd, (HMENU)BTN_CLICK, NULL, NULL); //кнопка, которая запускает алгоритм

    lvNorm = CreateWindow(WC_LISTVIEW, "",
         WS_VISIBLE|WS_BORDER|WS_CHILD | LVS_REPORT | LVS_EDITLABELS,
         8, 250, 156, 211,
         hWnd, NULL, NULL, 0);
    lvRavn = CreateWindow(WC_LISTVIEW, "",
         WS_VISIBLE|WS_BORDER|WS_CHILD | LVS_REPORT | LVS_EDITLABELS,
         168, 250, 156, 211,
         hWnd, NULL, NULL, 0); //списки
    lvPokaz = CreateWindow(WC_LISTVIEW, "",
         WS_VISIBLE|WS_BORDER|WS_CHILD | LVS_REPORT | LVS_EDITLABELS,
         328, 250, 156, 211,
         hWnd, NULL, NULL, 0);
	CreateColumnW(lvNorm, 0, L"Нормальное", 139);
	CreateColumnW(lvRavn, 0, L"Равномерное", 139);    //столбцы в них
	CreateColumnW(lvPokaz, 0, L"Показательное", 139);
}
