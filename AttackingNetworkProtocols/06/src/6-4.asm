section .text

global main

main:
  call print_add

print_add:
  push ebp
  mov ebp, esp

  push 10
  push 1
  call add

