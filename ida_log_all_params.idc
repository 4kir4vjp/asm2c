#include <idc.idc>

// ============================================================================
// IDC Script: Log ALL parameters for sub_7C687790
// Function: sub_7C687790(_DWORD *this, wchar_t *Source, struct_a3 *a3, int a4)
// ============================================================================

// Global counter for call tracking
auto g_call_count;

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

static dump_memory(addr, size) {
    auto i;
    auto result = "";

    if (addr == 0 || addr == 0xFFFFFFFF || addr == BADADDR) {
        return "<NULL>";
    }

    for (i = 0; i < size; i++) {
        if (i > 0 && i % 16 == 0) {
            result = result + "\n                ";
        }
        result = result + form("%02X ", Byte(addr + i));
    }

    return result;
}

static dump_struct_a3(a3_ptr) {
    if (a3_ptr == 0 || a3_ptr == 0xFFFFFFFF || a3_ptr == BADADDR) {
        Message("  a3       : <NULL>\n");
        return;
    }

    Message("  a3       : 0x%08X\n", a3_ptr);

    // Dump first 32 bytes of struct
    Message("    +0x00  : %s\n", dump_memory(a3_ptr, 16));
    Message("    +0x10  : %s\n", dump_memory(a3_ptr + 16, 16));

    // Nếu biết structure, có thể parse chi tiết:
    // Message("    field1 : 0x%08X\n", Dword(a3_ptr + 0));
    // Message("    field2 : 0x%08X\n", Dword(a3_ptr + 4));
}

static dump_this_object(this_ptr) {
    if (this_ptr == 0 || this_ptr == 0xFFFFFFFF || this_ptr == BADADDR) {
        Message("  this     : <NULL>\n");
        return;
    }

    Message("  this     : 0x%08X\n", this_ptr);

    // Dump vtable pointer và first few members
    auto vtable = Dword(this_ptr);
    Message("    vtable : 0x%08X\n", vtable);
    Message("    +0x04  : 0x%08X\n", Dword(this_ptr + 4));
    Message("    +0x08  : 0x%08X\n", Dword(this_ptr + 8));
    Message("    +0x0C  : 0x%08X\n", Dword(this_ptr + 12));
}

static log_all_parameters() {
    auto esp = get_reg_value("ESP");
    auto ecx = get_reg_value("ECX");
    auto eip = get_reg_value("EIP");

    // Đọc parameters
    auto this_ptr = ecx;
    auto source_ptr = Dword(esp + 4);
    auto a3_ptr = Dword(esp + 8);
    auto a4_val = Dword(esp + 12);
    auto ret_addr = Dword(esp);

    g_call_count++;

    // Header
    Message("\n");
    Message("════════════════════════════════════════════════════════════════\n");
    Message("Call #%d to sub_7C687790\n", g_call_count);
    Message("════════════════════════════════════════════════════════════════\n");
    Message("Called from: 0x%08X\n", ret_addr);
    Message("EIP        : 0x%08X\n", eip);
    Message("\n");

    // Parameters
    Message("Parameters:\n");
    Message("────────────────────────────────────────────────────────────────\n");

    // this pointer
    dump_this_object(this_ptr);
    Message("\n");

    // Source (wchar_t*)
    auto wstr = read_wchar_string(source_ptr, 200);
    Message("  Source   : 0x%08X\n", source_ptr);
    Message("    -> \"%s\"\n", wstr);

    // Highlight nếu chứa map_publish
    if (strstr(wstr, "map_publish") != -1) {
        Message("    >>> CONTAINS 'map_publish' <<<\n");
    }
    Message("\n");

    // struct_a3 *a3
    dump_struct_a3(a3_ptr);
    Message("\n");

    // int a4
    Message("  a4       : 0x%08X (%d)\n", a4_val, a4_val);

    Message("════════════════════════════════════════════════════════════════\n");
    Message("\n");

    return 0;  // Không dừng lại, chỉ log
}

static main() {
    auto func_addr;

    // Initialize global counter
    g_call_count = 0;

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
        Message("Parameter Logging Enabled\n");
        Message("════════════════════════════════════════════════════════════════\n");
        Message("Function  : sub_7C687790\n");
        Message("Address   : 0x%08X\n", func_addr);
        Message("Mode      : Log all parameters (no break)\n");
        Message("════════════════════════════════════════════════════════════════\n");
        Message("\nAll calls will be logged with full parameter details\n");
        Message("Check Output window (View -> Open subviews -> Output window)\n\n");
    } else {
        Message("Error: Failed to set breakpoint at 0x%08X\n", func_addr);
    }
}
