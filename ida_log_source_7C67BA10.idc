#include <idc.idc>

// ============================================================================
// IDC Script: Log Source parameter for sub_7C67BA10
// Function: sub_7C67BA10(struct _RTL_CRITICAL_SECTION *this, wchar_t *Source, struct_a3 *a3, int a4)
// Output: IDA Output window, one line per value
// ============================================================================

static read_wchar_string(addr, max_len) {
    auto wstr = "";
    auto i = 0;

    if (addr == 0 || addr == 0xFFFFFFFF || addr == BADADDR) {
        return "";
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

    return wstr;
}

static log_source_only() {
    auto esp = get_reg_value("ESP");
    auto source_ptr = Dword(esp + 4);
    auto wstr;

    // Kiểm tra Source pointer hợp lệ
    if (source_ptr == 0 || source_ptr == 0xFFFFFFFF || source_ptr == BADADDR) {
        return 0;
    }

    // Đọc Source string
    wstr = read_wchar_string(source_ptr, 200);

    // Kiểm tra nếu string empty
    if (strlen(wstr) == 0) {
        return 0;
    }

    // In chỉ giá trị, không có prefix
    Message("%s\n", wstr);

    return 0;
}

static main() {
    auto func_addr;

    // Tìm địa chỉ function
    func_addr = get_name_ea_simple("sub_7C67BA10");
    if (func_addr == BADADDR) {
        func_addr = 0x7C67BA10;
        Message("Warning: Using hardcoded address 0x7C67BA10\n");
    }

    // Xóa breakpoint cũ
    del_bpt(func_addr);

    // Set breakpoint mới
    if (add_bpt(func_addr, 0, BPT_SOFT) == 1) {
        SetBptCnd(func_addr, "log_source_only()");

        Message("\n");
        Message("════════════════════════════════════════════════════════════════\n");
        Message("Source Logging Enabled\n");
        Message("════════════════════════════════════════════════════════════════\n");
        Message("Function  : sub_7C67BA10\n");
        Message("Address   : 0x%08X\n", func_addr);
        Message("Output    : IDA Output window (one value per line)\n");
        Message("════════════════════════════════════════════════════════════════\n");
    } else {
        Message("Error: Failed to set breakpoint at 0x%08X\n", func_addr);
    }
}
