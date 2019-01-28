use16
f_name          db "boot.cfg"

rb 256 - ($ - $$)

f_flags         dq ?
f_l_prev        dd 0x2
f_l_next        dd 9h
f_l_parent      dd 0x1
f_size_in_cl    dq 0x1
f_size_in_b     dq 0xFFFFFFFFFFFFFFFF
f_data          dd 0x7

rb 510 - ($ - $$)
db 0x55, 0xAA
