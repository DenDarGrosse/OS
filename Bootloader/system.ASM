use32
                mov     byte[0xB8000 + (25 * 80 - 1) * 2], "2"
                mov     byte[0xB8000 + (25 * 80 - 2) * 2], "3"
                mov     byte[0xB8000 + (25 * 80 - 3) * 2], "l"
                mov     byte[0xB8000 + (25 * 80 - 4) * 2], "e"
                mov     byte[0xB8000 + (25 * 80 - 5) * 2], "n"
                mov     byte[0xB8000 + (25 * 80 - 6) * 2], "r"
                mov     byte[0xB8000 + (25 * 80 - 7) * 2], "e"
                mov     byte[0xB8000 + (25 * 80 - 8) * 2], "K"
                jmp     $