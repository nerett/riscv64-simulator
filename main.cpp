//#define DEBUG

#include "hart.hpp"
#include "elf_reader.hpp"

#include <cstdint>
#include <vector>

#include <chrono>

bool test_fib_imm() {
    // fibonacci
    /*
_start:
    addi x7, x0, 2
    addi x1, x0, 10
    addi x2, x0, 0
    addi x3, x0, 1
lop:
    add x4, x2, x3
    add x2, x3, x0
    add x3, x4, x0
    addi x7, x7, 1
    blt x7, x1, lop
// answer is in x3
*/

    Hart hart;
    std::vector<uint32_t> fib = {
        0x00000000,
        0x00200393,
        0x00a00093,
        0x00000113,
        0x00100193,
        // lop
        0x00310233,
        0x00018133,
        0x000201b3,
        0x00138393,
        0xfe13c8e3,
        0x00000073 // ecall
    };

    hart.memory = (uint8_t*)fib.data();
    hart.pc = 4;
    hart.simulate();
    std::cout << "fib(9): " << hart.registers[3] << " (34=>ok)\n";
    if (hart.registers[3] != 34) {
        std::cerr << "Fibonacci test from array was not passed!!!\n";
        return false;
    }
    return true;
}

bool test_elf_reader() {
    Hart hart;
    ElfReader reader("build/sample_rv64");
    ReaderStatus read_st = reader.load_instructions(hart);
    if (read_st != ReaderStatus::SUCCESS) {
        std::cout << "failed to load instrs, ELF LOAD test failed :(\n";
        std::cout << "load err: " << int(read_st) << '\n';
        return false;
    }
    hart.simulate();
    bool status = (hart.registers[20] == 10)
        and (hart.registers[11] == 20)
        and (hart.registers[12] == 30)
        and (hart.registers[13] == 10);
    if (!status) {
        std::cerr << "read elf test was not passed!!!\n";
        return false;
    }
    return true;
}

void run_8q() {
    Hart hart;
    ElfReader reader("build/8queens");
    ReaderStatus read_st = reader.load_instructions(hart);
    if (read_st != ReaderStatus::SUCCESS) {
        std::cout << "failed to load instrs, ELF LOAD test failed :(\n";
        std::cout << "load err: " << int(read_st) << '\n';
        return;
    }

    auto start = std::chrono::steady_clock::now();

    hart.simulate();

    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;

    std::cout << "Time taken: " << elapsed.count() << " ms" << std::endl;
    std::cout << "Intructions executed: " << hart.ins_cnt << std::endl;
    std::cout << "Performance total: " << (hart.ins_cnt / elapsed.count()) / 1e3 << " Mips" << std::endl;
}

int main() {
    if (test_fib_imm() and test_elf_reader())
        std::cout << "tests are OK!\n";
    else {
        std::cout << "tests are bad :(\n";
        return 1;
    }

    run_8q();

    return 0;
}
