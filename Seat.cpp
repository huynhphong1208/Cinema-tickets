#include "Seat.h"
#include "Ticket.h"
#include "resource.h"
#include <string>
#include <windows.h>

Seat seats[ROWS][COLS];

// Biến tạm để truyền dữ liệu giữa DialogProc và HandleClick
static std::wstring g_BookingName;
static std::wstring g_BookingPhone;
static std::wstring g_CancelName;
static std::wstring g_CancelPhone;
static std::wstring g_CancelCode;

// DialogProc cho đặt vé (2 ô: name, phone)
INT_PTR CALLBACK BookingDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK) {
            wchar_t buf[256];
            GetDlgItemTextW(hDlg, IDC_EDIT_NAME, buf, 256);
            g_BookingName = buf;
            GetDlgItemTextW(hDlg, IDC_EDIT_PHONE, buf, 256);
            g_BookingPhone = buf;
            EndDialog(hDlg, IDOK);
            return (INT_PTR)TRUE;
        }
        else if (LOWORD(wParam) == IDCANCEL) {
            EndDialog(hDlg, IDCANCEL);
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

// DialogProc cho hủy vé (3 ô)
INT_PTR CALLBACK CancelDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK) {
            wchar_t buf[256];
            GetDlgItemTextW(hDlg, IDC_EDIT_CANCEL_NAME, buf, 256);
            g_CancelName = buf;
            GetDlgItemTextW(hDlg, IDC_EDIT_CANCEL_PHONE, buf, 256);
            g_CancelPhone = buf;
            GetDlgItemTextW(hDlg, IDC_EDIT_CANCEL_CODE, buf, 256);
            g_CancelCode = buf;
            EndDialog(hDlg, IDOK);
            return (INT_PTR)TRUE;
        }
        else if (LOWORD(wParam) == IDCANCEL) {
            EndDialog(hDlg, IDCANCEL);
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

void InitSeats() {
    int startX = 20;
    int startY = 20;
    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < COLS; ++j) {
            seats[i][j].rect.left = startX + j * (SEAT_SIZE + SEAT_GAP);
            seats[i][j].rect.top = startY + i * (SEAT_SIZE + SEAT_GAP);
            seats[i][j].rect.right = seats[i][j].rect.left + SEAT_SIZE;
            seats[i][j].rect.bottom = seats[i][j].rect.top + SEAT_SIZE;
            seats[i][j].booked = false;
            seats[i][j].ticketCode.clear();
            seats[i][j].customerName.clear();
            seats[i][j].phone.clear();
        }
    }
}

void DrawSeats(HDC hdc) {
    SetBkMode(hdc, TRANSPARENT);
    HFONT hFont = CreateFontW(18, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, L"Segoe UI");
    HFONT old = (HFONT)SelectObject(hdc, hFont);

    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < COLS; ++j) {
            // brush màu
            HBRUSH brush = seats[i][j].booked ? CreateSolidBrush(RGB(220, 50, 50)) : CreateSolidBrush(RGB(100, 220, 120));
            FillRect(hdc, &seats[i][j].rect, brush);
            FrameRect(hdc, &seats[i][j].rect, (HBRUSH)GetStockObject(BLACK_BRUSH));
            DeleteObject(brush);

            // Tên ghế (A1..)
            wchar_t name[8];
            swprintf(name, 8, L"%c%d", L'A' + i, j + 1);
            RECT tr = seats[i][j].rect;
            tr.top += 8;
            DrawTextW(hdc, name, -1, &tr, DT_CENTER | DT_SINGLELINE);

            // Nếu đã đặt, hiển thị mã vé nhỏ phía dưới
            if (seats[i][j].booked && !seats[i][j].ticketCode.empty()) {
                RECT cr = seats[i][j].rect;
                cr.top += SEAT_SIZE / 2;
                DrawTextW(hdc, seats[i][j].ticketCode.c_str(), -1, &cr, DT_CENTER | DT_SINGLELINE);
            }
        }
    }

    SelectObject(hdc, old);
    DeleteObject(hFont);
}

void HandleClick(int x, int y, HWND hwnd) {
    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < COLS; ++j) {
            if (PtInRect(&seats[i][j].rect, POINT{ x, y })) {
                // Tạo tên ghế
                wchar_t seatNameBuf[8];
                swprintf(seatNameBuf, 8, L"%c%d", L'A' + i, j + 1);
                std::wstring seatName = seatNameBuf;

                if (!seats[i][j].booked) {
                    // mở dialog đặt vé (tên + sdt)
                    g_BookingName.clear();
                    g_BookingPhone.clear();
                    INT_PTR res = DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_INPUTBOX), hwnd, BookingDlgProc);
                    if (res == IDOK) {
                        // gọi AddTicket để sinh mã và lưu
                        std::wstring code = AddTicket(g_BookingName, g_BookingPhone, seatName);
                        seats[i][j].booked = true;
                        seats[i][j].ticketCode = code;
                        seats[i][j].customerName = g_BookingName;
                        seats[i][j].phone = g_BookingPhone;

                        // thông báo
                        std::wstring msg = L"Đặt vé thành công!\nGhế: " + seatName + L"\nMã vé: " + code;
                        MessageBoxW(hwnd, msg.c_str(), L"Thông báo", MB_OK | MB_ICONINFORMATION);
                    }
                }
                else {
                    // mở dialog hủy vé
                    g_CancelName.clear();
                    g_CancelPhone.clear();
                    g_CancelCode.clear();
                    INT_PTR res = DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_CANCELBOX), hwnd, CancelDlgProc);
                    if (res == IDOK) {
                        bool ok = CancelTicket(g_CancelName, g_CancelPhone, g_CancelCode);
                        if (ok) {
                            seats[i][j].booked = false;
                            seats[i][j].ticketCode.clear();
                            seats[i][j].customerName.clear();
                            seats[i][j].phone.clear();
                            MessageBoxW(hwnd, L"Đã hủy vé thành công.", L"Thông báo", MB_OK | MB_ICONINFORMATION);
                        }
                        else {
                            MessageBoxW(hwnd, L"Không tìm thấy vé (tên/SDT/mã không khớp).", L"Lỗi", MB_OK | MB_ICONERROR);
                        }
                    }
                }

                InvalidateRect(hwnd, NULL, TRUE);
                return;
            }
        }
    }
}

// placeholder (nếu bạn muốn lưu / load)
void LoadSeats() {}
void SaveSeats() {}
