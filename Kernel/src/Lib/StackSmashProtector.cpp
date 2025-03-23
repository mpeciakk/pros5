#define STACK_CHK_GUARD 0xe2dee396

unsigned long __stack_chk_guard = STACK_CHK_GUARD;

extern "C" [[noreturn]] void __stack_chk_fail() {
    while (true) {
    }
}