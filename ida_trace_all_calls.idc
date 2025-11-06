#include <idc.idc>

// ============================================================================
// IDC Script: Trace ALL calls to sub_7C687790 and log Source parameter
// Useful for debugging when you want to see all values
// ============================================================================

static read_wchar_string(addr, max_len) {
    auto wstr = "";
    auto i = 0;

    while (i < max_len) {
        auto wchar = Word(addr + i * 2);
        if (wchar == 0) break;

        if (wchar >= 0x20 && wchar < 0x7F) {
            wstr = wstr + form("%c", wchar);
        } else {
            wstr = wstr + "?";
        }
        i++;
    }

    return wstr;
}

static trace_all_calls() {
    auto esp = get_reg_value("ESP");
    auto ecx = get_reg_value("ECX");
    auto source_ptr = Dword(esp + 4);
    auto wstr = "";

    if (source_ptr != 0 && source_ptr != 0xFFFFFFFF && source_ptr != BADADDR) {
        wstr = read_wchar_string(source_ptr, 100);

        // Log tất cả các lần gọi
        Message("sub_7C687790: this=0x%08X, Source='%s'", ecx, wstr);

        // Highlight khi tìm thấy map_publish
        if (strstr(wstr, "map_publish") != -1) {
            Message(" <-- MATCH!");
        }
        Message("\n");
    }

    return 0;  // Không dừng lại, chỉ trace
}

static main() {
    auto func_addr;

    func_addr = get_name_ea_simple("sub_7C687790");
    if (func_addr == BADADDR) {
        func_addr = 0x7C687790;
    }

    del_bpt(func_addr);

    if (add_bpt(func_addr, 0, BPT_SOFT) == 1) {
        SetBptCnd(func_addr, "trace_all_calls()");

        Message("\n");
        Message("========================================\n");
        Message("Tracing Mode Enabled\n");
        Message("========================================\n");
        Message("Function : sub_7C687790\n");
        Message("Address  : 0x%08X\n", func_addr);
        Message("Mode     : Log all calls (no break)\n");
        Message("========================================\n");
        Message("\nAll calls will be logged to Output window\n\n");
    } else {
        Message("Error: Failed to set breakpoint\n");
    }
}
