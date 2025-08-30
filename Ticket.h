#pragma once
#include <string>

// Trả về mã vé khi đặt thành công (mã dạng VExxxx)
std::wstring AddTicket(const std::wstring& name, const std::wstring& phone, const std::wstring& seat);

// Hủy vé: trả về true nếu tìm thấy và đã xóa
bool CancelTicket(const std::wstring& name, const std::wstring& phone, const std::wstring& code);
