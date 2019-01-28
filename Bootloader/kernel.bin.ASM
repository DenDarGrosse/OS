use16
f_name          db "kernel.bin"

rb 256 - ($ - $$)

f_flags         dq ?
f_l_prev        dd 6h
f_l_next        dd 0xFFFFFFFF
f_l_parent      dd 1h
f_size_in_cl    dq 11
f_size_in_b     dq 0xFFFFFFFFFFFFFFFF
f_data          dd 10

rb 510 - ($ - $$)
db 0x55, 0xAA
