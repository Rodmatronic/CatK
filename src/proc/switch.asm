section .text
  global do_first_context_switch
  extern isr_stub_end

do_first_context_switch:
  mov eax, [esp + 0x04]
  mov esp, eax
  jmp isr_stub_end
