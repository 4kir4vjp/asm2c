#include <idc.idc>

// ============================================================================
// IDC Script: Log ALL parameters for sub_7C687790
// Function: sub_7C687790(_DWORD *this, wchar_t *Source, struct_a3 *a3, int a4)
// ============================================================================

static read_wchar_string(addr, max_len) {
    auto wstr = "";
    auto i = 0;

    if (addr == 0 || addr == 0xFFFFFFFF || addr == BADADDR) {
        return "<NULL>";
    }

    while (i < max_len) {
        auto wchar = Word(addr + i * 2);
        if (wchar == 0) break;

        if (wchar >= 0x20 && wchar < 0x7F) {
            wstr = wstr + form("%c", wchar);
        } else if (wchar < 0x100) {
            wstr = wstr + form("\\x%02X", wchar);
        } else {
            wstr = wstr + form("\\u%04X", wchar);
        }
        i++;
    }

    if (i == 0) {
        return "<empty>";
    }

    return wstr;
}

static log_all_parameters() {
    auto esp = get_reg_value("ESP");
    auto source_ptr = Dword(esp + 4);

    // Kiểm tra Source pointer hợp lệ
    if (source_ptr == 0 || source_ptr == 0xFFFFFFFF || source_ptr == BADADDR) {
        return 0;  // Skip nếu Source NULL
    }

    // Đọc Source string
    auto wstr = read_wchar_string(source_ptr, 200);

    // Kiểm tra nếu string empty hoặc NULL
    if (wstr == "<NULL>" || wstr == "<empty>" || strlen(wstr) == 0) {
        return 0;  // Skip nếu Source empty
    }

    // In 1 dòng duy nhất với Source
    Message("Source: %s\n", wstr);

    return 0;  // Không dừng lại, chỉ log
}

static main() {
    auto func_addr;

    // Tìm địa chỉ function
    func_addr = get_name_ea_simple("sub_7C687790");
    if (func_addr == BADADDR) {
        func_addr = 0x7C687790;
        Message("Warning: Using hardcoded address 0x7C687790\n");
    }

    // Xóa breakpoint cũ
    del_bpt(func_addr);

    // Set breakpoint mới
    if (add_bpt(func_addr, 0, BPT_SOFT) == 1) {
        SetBptCnd(func_addr, "log_all_parameters()");

        Message("\n");
        Message("════════════════════════════════════════════════════════════════\n");
        Message("Source Parameter Logging Enabled\n");
        Message("════════════════════════════════════════════════════════════════\n");
        Message("Function  : sub_7C687790\n");
        Message("Address   : 0x%08X\n", func_addr);
        Message("Mode      : Log Source parameter only (1 line per call)\n");
        Message("════════════════════════════════════════════════════════════════\n");
    } else {
        Message("Error: Failed to set breakpoint at 0x%08X\n", func_addr);
    }
}
