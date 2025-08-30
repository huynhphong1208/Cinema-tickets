#include "Ticket.h"
#include <string>
#include <vector>
#include <ctime>

struct Ticket {
    std::wstring name;
    std::wstring phone;
    std::wstring code;
    std::wstring seat;
};

static std::vector<Ticket> g_tickets;

// Sinh mã vé đơn giản dựa trên thời gian + kích thước vector
static std::wstring GenerateCode() {
    std::wstring code = L"VE";
    std::time_t t = std::time(nullptr);
    long v = static_cast<long>(t) % 1000000;
    code += std::to_wstring(v);
    return code;
}

std::wstring AddTicket(const std::wstring& name, const std::wstring& phone, const std::wstring& seat) {
    Ticket t;
    t.name = name;
    t.phone = phone;
    t.seat = seat;
    t.code = GenerateCode();
    g_tickets.push_back(t);
    return t.code;
}

bool CancelTicket(const std::wstring& name, const std::wstring& phone, const std::wstring& code) {
    for (auto it = g_tickets.begin(); it != g_tickets.end(); ++it) {
        if (it->name == name && it->phone == phone && it->code == code) {
            g_tickets.erase(it);
            return true;
        }
    }
    return false;
}
