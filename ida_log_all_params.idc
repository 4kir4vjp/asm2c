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
    auto fp;
    auto wstr;

    // Debug: log mỗi lần function được gọi
    Message("[CALL] sub_7C687790 called! ESP=0x%08X, source_ptr=0x%08X\n", esp, source_ptr);

    // Kiểm tra Source pointer hợp lệ
    if (source_ptr == 0 || source_ptr == 0xFFFFFFFF || source_ptr == BADADDR) {
        Message("[SKIP] Source pointer is NULL or invalid\n");
        return 0;  // Skip nếu Source NULL
    }

    // Đọc Source string
    wstr = read_wchar_string(source_ptr, 200);
    Message("[READ] Source string: '%s'\n", wstr);

    // Kiểm tra nếu string empty hoặc NULL
    if (wstr == "<NULL>" || wstr == "<empty>" || strlen(wstr) == 0) {
        Message("[SKIP] Source string is empty\n");
        return 0;  // Skip nếu Source empty
    }

    // Debug: in ra IDA output
    Message("Source: %s\n", wstr);

    // Ghi vào file D:\1.log (append mode)
    fp = fopen("D:\\1.log", "a");
    if (fp != 0) {
        fprintf(fp, "Source: %s\n", wstr);
        fclose(fp);
        Message("[OK] Wrote to D:\\1.log\n");
    } else {
        Message("[ERROR] Cannot open D:\\1.log!\n");
    }

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
        Message("Output    : D:\\1.log\n");
        Message("Mode      : Append Source parameter (1 line per call)\n");
        Message("════════════════════════════════════════════════════════════════\n");
    } else {
        Message("Error: Failed to set breakpoint at 0x%08X\n", func_addr);
    }
}
