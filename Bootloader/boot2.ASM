org 0x7C00
use16
jmp word start

version         dq 0x1
flags           dq ?
size_ofFS_in_cl dq ?
map_link        dq ?
map_size        dq ?
size_of_cl_in_b dq 0x512
boot_name        db "boot.bin",0

nError           db 0
disk_id          db ?
cnt_of_heads     db ?
sector_per_track dw ?
size_of_disk     db ?

virtual at 0x800
f_info:
        f_name          rb 256
        f_flags         dq ?
        f_l_prev        dd ?
        f_l_next        dd ?
        f_l_parent      dd ?
        f_size_in_cl    dq ?
        f_size_in_b     dq ?
        f_data          dd ?
end virtual

message                 db "Welcome to the bootloader ", 10, 13, 0
load_file_mes           db "File loading: ",0
load_file_data_mes      db "Loading data",10,13,0
load_file_done          db "Loaded", 10,13,0

start:
                mov     ax, cs
                mov     ds, ax
                mov     es, ax

                mov     ss, ax
                mov     sp, $$

                sti

                mov     [disk_id], dl

                mov     si, message
                call    Print

                mov     ah, 0x8
                mov     dl, [disk_id]
                int     13h

                inc     dh
                mov     [cnt_of_heads], dh
                and     cx, 111111b
                mov     [sector_per_track], cx

                mov     si, boot_name
                mov     bx, 0x7e00
                call    load_file

                jmp     boot

; Полная загрузка файла
; вход ds:si - name, bx:0 - начало в памяти
; выход bx:0 - все данные, f_info файл, ax - размер файла в секторах
load_file:
                push    si
                mov     si, load_file_mes
                call    Print
                pop     si
                call    Print
                mov     ah, 0eh
                mov     al, 13
                int     10h
                mov     al, 10
                int     10h

                push    dx bx

                push    bx
                xor     dx, dx
                mov     ax, 1
                mov     bx, 0x800
                call    load_sector
                pop     bx

                mov     dx, word[f_data+2]
                mov     ax, word[f_data]
                call    find_file

                call    load_file_data

                pop     bx dx
                ret



; поиск файла с именем ds:si в каталоге dx:ax(первый файл каталога),
; выход f_info
find_file:
                push bx cx bx dx di
 .find:
                cmp     ax, -1
                jne     @f
                cmp     dx, -1
                jne     @f
 .not_found:
                call    Error
                db "NOT FOUND",13,10,0
 @@:
                mov     di, f_info
                mov     bx, f_info
                call    load_sector
                push    di
                mov     cx, 0xFFFF
                xor     al, al
                repne   scasb
                neg     cx
                dec     cx
                pop     di
                push    si
                repe    cmpsb
                pop     si
                je      .found
                mov     ax, word[f_l_next]
                mov     dx, word[f_l_next + 2]
                jmp     .find
 .found:
                pop di dx bx cx bx
                ret

;загружает f_data всю в память
;вход f_info - файл, bx:0 - адрес куда писать
;выход ax количество секторов загруженных, bx:0 сектора
load_file_data:
                push    si
                mov     si, load_file_data_mes
                call    Print
                pop     si

                push    si bx cx
                mov     ax, word[f_size_in_cl]
                push    ax

                mov     cx, ax
                push    bx
                mov     bx, 0x800
                mov     cx, word[f_size_in_cl]
                mov     dx, word[f_data+2]
                mov     ax, word[f_data]
                call    load_sector
                pop     bx

                mov     si, f_info

                mov     ax, word[si]
                mov     dx, word[si+2]
 .loop_start:

                call    load_sector
                dec     cx
                jz      .loaded
                add     si, 4
                mov     ax, word[si]
                mov     dx, word[si+2]
                add     bx, 0x200
                jmp     .loop_start
 .loaded:
                mov     si, load_file_done
                call    Print

                pop     ax
                pop     cx bx si
                ret



;принимает желаемый адрес в DX(старшые) AX(младшие) в секторах
; es:bx - адрес буфера результата
load_sector:
                push ax bx dx cx

                div     [sector_per_track]
                mov     cl, dl
                inc     cl                      ;cx - sector
                div     [cnt_of_heads]          ; ah = Head, al = Cylinder
                mov     dl, [disk_id]
                mov     dh, ah
                mov     ch, al
                mov     al, 0x1
                mov     ah, 0x2
                int     0x13
                jnc     .good
                mov     [nError], ah
                call    Error
                db      "DISK ERROR",13,10,0
 .good:
                pop cx dx bx ax
                ret

; пишет строку
; ds:si - строка с детерменирующем нулем
Print:
                push    ax si
                mov     ah, 0eh

 .loop_start:
                lodsb
                test    al, al
                jz      .loop_end
                int     10h
                jmp     .loop_start

 .loop_end:
                pop     si ax
                ret

Error:
                pop     si
                call    Print
                ret

; Пустое пространство и сигнатура
rb 510 - ($ - $$)
db 0x55, 0xAA

;В памяти будет как продолжение этого файла
cfg_name        db      "boot.cfg",0
starting        db      "Starting Kernel",13,10,0
core_load_16    db      "The 16-byte kernel started", 13, 10, 0
core_load_32    db      "The 32-byte kernel started", 13, 10, 0
core_load_64    db      "The 64-byte kernel started", 13, 10, 0

label memory_map at 0x7000

boot:
                call    get_memory_map
                ; загрузка модулей
                mov     si, cfg_name
                mov     bx, 0x1000
                call    load_file

                mov     si, 0x1000
                push    ds

 .loop_start:   ;Нужно в 0x1000:si хранить данные
                mov     al, "L"
                cmp     al, [si]
                je     .load
                mov     al, "S"
                cmp     al, [si]
                je      .start
                mov     al, 00h
                cmp     al, [si]
                je      .error
 .return:
                inc     si
                jmp     .loop_start

 .load:
                mov     dh, 00h
                cmp     byte[0x1000], dh
                je      .return
                mov     byte[0x1000], 00h

                mov     al, 0x0D
                inc     si
                push    si

 ..loop_start:
                cmp     byte[si], al
                je      ..found
                inc     si
                jmp     ..loop_start
 ..found:
                mov     byte[si], 0

                mov     bp, si

                pop     si
                mov     bx, 0xD000      ;адрес, где начинается ядро
                call    load_file

                mov     si, bp
                jmp     .return

 .start:
                push    si
                mov     si, starting
                call    Print
                pop     si

                inc     si
                pop     ds
                mov     ax, "16"
                cmp     word[si], ax
                je      .start16
                mov     ax, "32"
                cmp     word[si], ax
                je      .start32
                mov     ax, "64"
                cmp     word[si], ax
                je      .start64
 .error:
                call    Error
                db      "Config file error",13,10,0

 .start16:
                mov     si, core_load_16
                call    Print
                mov     ah, 00h
                int     16h
 .start32:
                mov     si, core_load_32
                call    Print

                ;отчищаем каталог страниц
                xor     ax, ax                  ;обнуление ax
                mov     di, 0x1000              ;адрес начала 0:0x1000
                mov     cx, 0x800               ;2048 повторений
                rep     stosw                   ;замена 2048 раз по 2 байта (4КБ)

                ;заполняем каталог страниц
                mov     dword[0x1000], 0x2000
                ;mov     dword[0x1FFC], 0x3000

                ;очищаем первую таблицу страниц
                xor     ax, ax                  ;обнуление ax
                mov     di, 0x2000              ;адрес начала 0:0х2000
                mov     cx, 0x800               ;2048 повторений
                rep     stosw                   ;замена 2048 раз по 2 байта (4КБ)

                ;заполняем первую таблицу страниц
                mov     eax, 0x0000             ;первый адрес - нулевой
                mov     cx, 0x400               ;1024 повторений
                mov     di, 0x2000              ;адрес начла 0:0х2000
  ..First_table:
                stosd                           ;запсь одного адреса страницы
                add     eax, 0x1000             ;задание следующего адреса
                loop    ..First_table           ;возвращение цикла

                ;очищаем последнюю таблицу страниц
                xor      ax, ax
                mov      di, 0x3000
                mov      cx, 0x800
                rep      stosw

                ;заполняем последнюю таблицу страниц
                ;mov di, 0x3000
                ;mov eax, dword[0x6000]
                ;or eax, 11b
                ;mov ecx, dword[0x6008]
                ;shr ecx, 12
  ;..Second_table:
                ;stosd
                ;add eax, 0x1000
                ;loop ..Second_table
                ;mov word[0x3FF4], 0x4000 + 11b ; Kernel stack
                ;mov word[0x3FF8], 0x3000 + 11b ; Kernel page table

                mov     eax, 0x1000
                mov     cr3, eax

                ;загрузим значение в GDTR
                lgdt    [gdtr32]

                cli

                mov     eax,cr0
                or      eax,1
                mov     cr0,eax

                jmp     8:start32
 .start64:
                mov     si, core_load_64
                call    Print
                mov     ah, 00h
                int     16h

; получение карты памяти
get_memory_map:
        mov di, memory_map
        xor ebx, ebx
 .una:
        mov eax, 0xE820
        mov edx, 0x534D4150
        mov ecx, 24
        mov dword[di + 20], 1
        int 0x15
        jc .dos
        add di, 24
        test ebx, ebx
        jnz .una
 .dos:
        cmp di, 0x7000
        ja .ok
        mov dword[di], 0x100000
        mov dword[di + 4], 0
        mov dword[di + 12], 0
        mov dword[di + 16], 1
        mov dword[di + 20], 0
        mov ax, 0xE801
        int 0x15
        jnc .tres
        mov ah, 0x88
        int 0x15
        jc .ok
        mov cx, ax
        xor dx, dx
 .tres:
        test cx, cx
        jz .quatro
        mov ax, cx
        mov bx, dx
 .quatro:
        movzx eax, ax
        movzx ebx, bx
        mov ecx, 1024
        mul ecx
        push eax
        mov eax, ebx
        mov ecx, 65536
        mul ecx
        pop edx
        add eax, edx
        mov [di + 8], eax
        add di, 24
        jmp .ok
 .ok:
        xor ax, ax
        mov cx, 24 / 2
        rep stosw
        ret

; таблица дескрипторов сегментов для 32-битного ядра
align 16
gdt32:
        dq 0                  ; NULL - 0
        dq 0x00CF9A000000FFFF ; CODE - 8
        dq 0x00CF92000000FFFF ; DATA - 16
gdtr32:
        dw $ - gdt32 - 1
        dd gdt32

;32-битный код
use32
start32:
                mov     eax, 16
                mov     ds, ax
                mov     es, ax
                mov     fs, ax
                mov     gs, ax
                mov     ss, ax
                mov     esp, 0x0000A000

                xor     ebx, ebx
                xor     ecx, ecx
                xor     edx, edx

                mov     eax, memory_map
                mov     bl, [disk_id]
                mov     cl, [cnt_of_heads]
                mov     dx, [sector_per_track]

                jmp     0x0000D000
