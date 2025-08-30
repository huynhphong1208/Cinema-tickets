#pragma once
#include <windows.h>
#include <string>

#define ROWS 5
#define COLS 5
#define SEAT_SIZE 60
#define SEAT_GAP 10

struct Seat {
    RECT rect;
    bool booked;
    std::wstring ticketCode;
    std::wstring customerName;
    std::wstring phone;
};

extern Seat seats[ROWS][COLS];

void InitSeats();
void DrawSeats(HDC hdc);
void HandleClick(int x, int y, HWND hwnd);

// optional persistence
void LoadSeats();
void SaveSeats();
