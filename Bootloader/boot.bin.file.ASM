use16
f_name          db "boot.bin"

rb 256 - ($ - $$)

f_flags         dq ?
f_l_prev        dd 0xFFFF
f_l_next        dd 0x6
f_l_parent      dd 0x1
f_size_in_cl    dq 0x2
f_size_in_b     dq 0xFFFFFFFFFFFFFFFF
f_data          dd 0x3

rb 510 - ($ - $$)
db 0x55, 0xAA
