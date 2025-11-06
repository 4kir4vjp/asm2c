#include <idc.idc>

// ============================================================================
// IDC Script: Conditional Breakpoint for wchar_t* containing "map_publish"
// Function: sub_7C687790(_DWORD *this, wchar_t *Source, struct_a3 *a3, int a4)
// Convention: __thiscall (ECX = this, Source at [ESP+4])
// ============================================================================

static read_wchar_string(addr, max_len) {
    auto wstr = "";
    auto i = 0;

    // Đọc wide string (mỗi wchar_t = 2 bytes)
    while (i < max_len) {
        auto wchar = Word(addr + i * 2);
        if (wchar == 0) break;  // NULL terminator

        // Chỉ lấy ký tự ASCII printable
        if (wchar >= 0x20 && wchar < 0x7F) {
            wstr = wstr + form("%c", wchar);
        } else {
            wstr = wstr + "?";
        }
        i++;
    }

    return wstr;
}

static check_map_publish() {
    auto esp = get_reg_value("ESP");
    auto ecx = get_reg_value("ECX");

    // Đọc parameters từ stack
    auto source_ptr = Dword(esp + 4);   // Source parameter
    auto a3_ptr = Dword(esp + 8);       // a3 parameter
    auto a4_val = Dword(esp + 12);      // a4 parameter

    // Validate pointer
    if (source_ptr == 0 || source_ptr == 0xFFFFFFFF || source_ptr == BADADDR) {
        return 0;
    }

    // Đọc wide string từ Source
    auto wstr = read_wchar_string(source_ptr, 200);

    // Check nếu chứa "map_publish"
    if (strstr(wstr, "map_publish") != -1) {
        Message("\n");
        Message("========================================\n");
        Message("[!!!] BREAKPOINT HIT - map_publish FOUND!\n");
        Message("========================================\n");
        Message("Function : sub_7C687790\n");
        Message("EIP      : 0x%08X\n", get_reg_value("EIP"));
        Message("this     : 0x%08X (ECX)\n", ecx);
        Message("Source   : 0x%08X\n", source_ptr);
        Message("  -> \"%s\"\n", wstr);
        Message("a3       : 0x%08X\n", a3_ptr);
        Message("a4       : 0x%08X\n", a4_val);
        Message("========================================\n");
        Message("\n");

        return 1;  // Dừng lại tại breakpoint
    }

    return 0;  // Tiếp tục execution
}

static main() {
    auto func_addr;

    // Thử lấy địa chỉ từ tên function
    func_addr = get_name_ea_simple("sub_7C687790");

    // Nếu không tìm thấy, dùng địa chỉ cố định
    if (func_addr == BADADDR) {
        func_addr = 0x7C687790;
        Message("Warning: Using hardcoded address 0x7C687790\n");
        Message("If breakpoint doesn't work, update func_addr in script\n");
    }

    // Xóa breakpoint cũ nếu có
    del_bpt(func_addr);

    // Set breakpoint mới
    if (add_bpt(func_addr, 0, BPT_SOFT) == 1) {
        // Set condition
        SetBptCnd(func_addr, "check_map_publish()");

        Message("\n");
        Message("========================================\n");
        Message("Breakpoint Setup Complete!\n");
        Message("========================================\n");
        Message("Function  : sub_7C687790\n");
        Message("Address   : 0x%08X\n", func_addr);
        Message("Condition : Source contains 'map_publish'\n");
        Message("========================================\n");
        Message("\nStart debugging (F9). Breakpoint will trigger when\n");
        Message("Source parameter contains 'map_publish'\n\n");
    } else {
        Message("Error: Failed to set breakpoint at 0x%08X\n", func_addr);
        Message("Make sure the address is correct and debugger is attached\n");
    }
}
