# 函数调用方式

## 一、x86 函数调用约定

### 1. __cdecl 函数调用约定

__cdecl 是 C Declaration 的缩写，表示 C 语言默认的函数调用方法：所有**参数从右到左依次入栈**，这些**参数由调用者清除**，称为**手动清栈**，返回值存放在 EAX 中。被调用函数不会要求调用者传递多少参数，调用者传递过多或者过少的参数，甚至完全不同的参数都不会产生编译阶段的错误。

```assembly
20: int cdecl_sum = cdecl_add(1, 2, 3, 4, 5, 6, 7);
00401138   push     7
0040113A   push     6
0040113C   push     5
0040113E   push     4
00401140   push     3
00401142   push     2
00401144   push     1
00401146   call     @ILT+5(_cdecl_add) (0040100a)
0040114B   add      esp,1Ch    # 栈平衡
0040114E   mov      dword ptr [ebp-4],eax    # 返回值
3:  int __cdecl cdecl_add(int a, int b, int c, int d, int e, int f, int g)
4:  {
00401030   push     ebp
00401031   mov      ebp,esp
00401033   sub      esp,44h
00401036   push     ebx
00401037   push     esi
00401038   push    edi
00401039   lea      edi,[ebp-44h]
0040103C   mov      ecx,11h
00401041   mov      eax,0CCCCCCCCh
00401046   rep stos   dword ptr [edi]
5:  int sum = a+b+c+d+e+f+g;
00401048   mov      eax,dword ptr [ebp+8]
0040104B   add      eax,dword ptr [ebp+0Ch]
0040104E   add      eax,dword ptr [ebp+10h]
00401051   add      eax,dword ptr [ebp+14h]
00401054   add      eax,dword ptr [ebp+18h]
00401057   add      eax,dword ptr [ebp+1Ch]
0040105A   add      eax,dword ptr [ebp+20h]
0040105D   mov      dword ptr [ebp-4],eax
6:  return sum;
00401060   mov      eax,dword ptr [ebp-4]   # 存放返回值
7:  }
00401063   pop      edi
00401064   pop      esi
00401065   pop      ebx
00401066   mov      esp,ebp
00401068   pop      ebp
00401069   ret
```

### 2. __stdcall 函数调用约定

__stdcall 是 Standard Call 的缩写，是 C++ 的标准调用方式：所有**参数从右到左依次入栈**，如果是**调用类成员**的话，最后一个入栈的是 **this** **指针**。这些堆栈中的**参数由被调用的函数在返回后清除**，使用的指令是 retn X，X 表示参数占用的字节数，CPU 在 ret 之后自动弹出 X 个字节的堆栈空间，称为**自动清栈**，返回值存放在 EAX 中。函数在编译的时候就必须确定参数个数，并且调用者必须严格的控制参数的生成，不能多，不能少，否则返回后会出错。

```assembly
21:  int stdcall_sum = stdcall_add(1, 2, 3, 4, 5, 6, 7);
00401151   push     7
00401153   push     6
00401155   push     5
00401157   push    4
00401159   push   3
0040115B   push    2
0040115D   push     1
0040115F   call     @ILT+15(_stdcall_add@28) (00401014)
00401164   mov      dword ptr [ebp-8],eax   # 返回值
9:   int __stdcall stdcall_add(int a, int b, int c, int d, int e, int f, int g)
10:  {
00401080   push     ebp
00401081   mov      ebp,esp
00401083   sub      esp,44h
00401086   push     ebx
00401087   push    esi
00401088   push    edi
00401089   lea      edi,[ebp-44h]
0040108C   mov      ecx,11h
00401091   mov      eax,0CCCCCCCCh
00401096   rep stos  dword ptr [edi]
11:  int sum = a+b+c+d+e+f+g;
00401098   mov      eax,dword ptr [ebp+8]
0040109B   add      eax,dword ptr [ebp+0Ch]
0040109E   add      eax,dword ptr [ebp+10h]
004010A1   add      eax,dword ptr [ebp+14h]
004010A4   add      eax,dword ptr [ebp+18h]
004010A7   add      eax,dword ptr [ebp+1Ch]
004010AA   add      eax,dword ptr [ebp+20h]
004010AD   mov      dword ptr [ebp-4],eax
12:  return sum;
004010B0   mov      eax,dword ptr [ebp-4]   # 存放返回值
13:  }
004010B3   pop      edi
004010B4   pop      esi
004010B5   pop      ebx
004010B6   mov      esp,ebp
004010B8   pop      ebp
004010B9   ret      1Ch  # 栈平衡（等价于先 add esp, 1Ch 再 ret）
```

### 3. __pascal 函数调用约定

\_\_pascal 是 Pascal 语言（Delphi）的函数调用方式，也可以在 C/C++ 中使用，**参数从左到右依次入栈**。返回时的**清栈方式与 __stdcall 相同**。

### 4. __fastcall 函数调用约定

\_\_fastcall 是编译器指定的快速调用方式。由于大多数的函数参数个数很少，使用堆栈传递比较费时。因此 \__fastcall 通常规定将**前两个（或若干个）参数由寄存器传递**，其余参数还是通过堆栈传递。不同编译器编译的程序规定的寄存器不同，返回方式和 __stdcall 相同。

### 5. __thiscall 函数调用约定

\_\_thiscall 是为了解决类成员调用中 this 指针传递而规定的。\__thiscall 要求把 this 指针放在特定寄存器中，该寄存器由编译器决定。VC 使用 ecx，Borland 的 C++ 编译器使用 eax。返回方式和 __stdcall 相当。

## 二、x64函数调用约定

X64 只有一种 __fastcall 函数调用约定。

普通函数：参数1、参数2、参数3、参数4分别保存在 RCX、RDX、R8D、R9D，剩下的参数从右往左依次入栈，被调用者实现栈平衡，返回值存放在 RAX 中。

```assembly
# 该代码是 msvc 2017 x64 生成的汇编代码
int fastcall_sum = fastcall_add(1, 2, 3, 4, 5, 6, 7);
00007FF6577A366E  mov      dword ptr [rsp+30h],7
00007FF6577A3676  mov      dword ptr [rsp+28h],6 
00007FF6577A367E  mov      dword ptr [rsp+20h],5 
00007FF6577A3686  mov      r9d,4 
00007FF6577A368C  mov      r8d,3 
00007FF6577A3692  mov      edx,2 
00007FF6577A3697  mov      ecx,1 
00007FF6577A369C  call     fastcall_add (07FF6577A11C2h) 
00007FF6577A36A1  mov      dword ptr [fastcall_sum],eax  # 返回值

int __fastcall fastcall_add(int a, int b, int c, int d, int e, int f, int g)
{
00007FF6D22D1790  mov      dword ptr [rsp+20h],r9d 
00007FF6D22D1795  mov      dword ptr [rsp+18h],r8d 
00007FF6D22D179A  mov      dword ptr [rsp+10h],edx 
00007FF6D22D179E  mov      dword ptr [rsp+8],ecx 
00007FF6D22D17A2  push     rbp 
00007FF6D22D17A3  push     rdi 
00007FF6D22D17A4  sub      rsp,0E8h 
00007FF6D22D17AB  mov      rbp,rsp 
00007FF6D22D17AE  mov      rdi,rsp 
00007FF6D22D17B1  mov      ecx,3Ah 
00007FF6D22D17B6  mov      eax,0CCCCCCCCh 
00007FF6D22D17BB  rep stos  dword ptr [rdi] 
00007FF6D22D17BD  mov      ecx,dword ptr [rsp+108h] 
int sum = a+b+c+d+e+f+g;
00007FF6D22D17C4  mov      eax,dword ptr [b] 
00007FF6D22D17CA  mov      ecx,dword ptr [a] 
00007FF6D22D17D0  add      ecx,eax 
00007FF6D22D17D2  mov      eax,ecx 
00007FF6D22D17D4  add      eax,dword ptr [c] 
00007FF6D22D17DA  add      eax,dword ptr [d] 
00007FF6D22D17E0  add      eax,dword ptr [e] 
00007FF6D22D17E6  add      eax,dword ptr [f] 
00007FF6D22D17EC  add      eax,dword ptr [g] 
00007FF6D22D17F2  mov      dword ptr [sum],eax 
   return sum;
00007FF6D22D17F5  mov      eax,dword ptr [sum]     # 存放返回值
}
00007FF6D22D17F8  lea      rsp,[rbp+0E8h] 
00007FF6D22D17FF  pop      rdi 
00007FF6D22D1800  pop      rbp 
00007FF6D22D1801  ret                        # 没做栈平衡
```

类的成员函数：参数1、参数2、参数3分别保存在 RDX、R8D、R9D 中，this 指针存放 RCX 中，剩下的参数从右往左依次入栈，被调用者实现栈平衡，返回值存放在 RAX 中。

```assembly
# 该代码是 msvc 2017 x64 生成的汇编代码
int sum = calc.thiscall_add(1, 2, 3, 4, 5, 6, 7);
00007FF602E6190F  mov      dword ptr [rsp+38h],7 
00007FF602E61917  mov      dword ptr [rsp+30h],6 
00007FF602E6191F  mov      dword ptr [rsp+28h],5 
00007FF602E61927  mov      dword ptr [rsp+20h],4 
00007FF602E6192F  mov      r9d,3 
00007FF602E61935  mov      r8d,2 
00007FF602E6193B  mov      edx,1 
00007FF602E61940  lea      rcx,[calc]          # this指针
00007FF602E61944  call     Calc::thiscall_add (07FF602E610A0h) 
00007FF602E61949  mov      dword ptr [sum],eax   # 返回值

int Calc::thiscall_add(int a, int b, int c, int d, int e, int f, int g)
{
00007FF602E61770  mov      dword ptr [rsp+20h],r9d 
00007FF602E61775  mov      dword ptr [rsp+18h],r8d 
00007FF602E6177A  mov      dword ptr [rsp+10h],edx 
00007FF602E6177E  mov      qword ptr [rsp+8],rcx 
00007FF602E61783  push     rbp 
00007FF602E61784  push     rdi 
00007FF602E61785  sub      rsp,0E8h 
00007FF602E6178C  mov      rbp,rsp 
00007FF602E6178F  mov      rdi,rsp 
00007FF602E61792  mov      ecx,3Ah 
00007FF602E61797  mov      eax,0CCCCCCCCh 
00007FF602E6179C  rep stos  dword ptr [rdi] 
00007FF602E6179E  mov      rcx,qword ptr [rsp+108h] 
   int sum = a+b+c+d+e+f+g;
00007FF602E617A6  mov      eax,dword ptr [b] 
00007FF602E617AC  mov      ecx,dword ptr [a] 
00007FF602E617B2  add      ecx,eax 
00007FF602E617B4  mov      eax,ecx 
00007FF602E617B6  add      eax,dword ptr [c] 
00007FF602E617BC  add      eax,dword ptr [d] 
00007FF602E617C2  add      eax,dword ptr [e] 
00007FF602E617C8  add      eax,dword ptr [f] 
00007FF602E617CE  add      eax,dword ptr [g] 
00007FF602E617D4  mov      dword ptr [sum],eax 
   return sum;
00007FF602E617D7  mov      eax,dword ptr [sum] # 存放返回值
}
00007FF602E617DA  lea      rsp,[rbp+0E8h]
00007FF602E617E1  pop      rdi
00007FF602E617E2  pop      rbp
00007FF602E617E3  ret                      # 没做栈平衡
```

## 三、ARM/ARM64 函数调用约定

ARM 和 ARM64 使用的是 ATPCS（ARM-Thumb Procedure Call Standard/ARM-Thumb 过程调用标准）的函数调用约定。

### 1. ARM

参数 1 ~ 参数 4 分别保存到 R0~R3 寄存器中，剩下的参数从右往左依次入栈，被调用者实现栈平衡，返回值存放在 R0 中。

```assembly
; 该代码是 arm-linux-androideabi-gcc + IDA PRO 生成的反汇编代码
.text:00008438            MOV         R3, #5
.text:0000843C            STR         R3, [SP]
.text:00008440            MOV         R3, #6
.text:00008444            STR         R3, [SP,#4]
.text:00008448            MOV         R3, #7
.text:0000844C            STR         R3, [SP,#8]
.text:00008450            MOV         R3, #8
.text:00008454            STR         R3, [SP,#12]
.text:00008458            MOV         R3, #9
.text:0000845C            STR         R3, [SP,#16]
.text:00008460            MOV         R3, #10
.text:00008464            STR         R3, [SP,#20]
.text:00008468            MOV         R0, #1
.text:0000846C            MOV         R1, #2
.text:00008470            MOV         R2, #3
.text:00008474            MOV         R3, #4
.text:00008478            BL         add
.text:0000847C            STR         R0, [R11,#-8]
.text:000083C4            EXPORT add
.text:000083C4
.text:000083C4            STR         R11, [SP,#-4]!
.text:000083C8            ADD         R11, SP, #0
.text:000083CC            SUB         SP, SP, #0x1C
.text:000083D0            STR         R0, [R11,#-16]
.text:000083D4            STR         R1, [R11,#-20]
.text:000083D8            STR         R2, [R11,#-24]
.text:000083DC            STR         R3, [R11,#-28]
.text:000083E0            LDR         R2, [R11,#-16]
.text:000083E4            LDR         R3, [R11,#-20]
.text:000083E8            ADD         R2, R2, R3
.text:000083EC            LDR         R3, [R11,#-24]
.text:000083F0            ADD         R2, R2, R3
.text:000083F4            LDR         R3, [R11,#-28]
.text:000083F8            ADD         R2, R2, R3
.text:000083FC            LDR         R3, [R11,#4]
.text:00008400            ADD         R2, R2, R3
.text:00008404            LDR         R3, [R11,#8]
.text:00008408            ADD         R2, R2, R3
.text:0000840C            LDR         R3, [R11,#12]
.text:00008410            ADD         R3, R2, R3
.text:00008414            STR         R3, [R11,#-8]
.text:00008418            LDR         R3, [R11,#-8]
.text:0000841C            MOV         R0, R3
.text:00008420            SUB         SP, R11, #0
.text:00008424            LDR         R11, [SP],#4
.text:00008428            BX          LR
```

### 2. ARM64

参数1~参数8 分别保存到 X0~X7 寄存器中 ，剩下的参数从右往左依次入栈，被调用者实现栈平衡，返回值存放在 X0 中。

```assembly
; 该代码是 aarch64-linux-android-gcc + IDA PRO 生成的反汇编代码
.text:000000000040065C            MOV         W0, #9
.text:0000000000400660            STR         W0, [SP]
.text:0000000000400664            MOV         W0, #10
.text:0000000000400668            STR         W0, [SP,#8]
.text:000000000040066C            MOV         W0, #1
.text:0000000000400670            MOV         W1, #2
.text:0000000000400674            MOV         W2, #3
.text:0000000000400678            MOV         W3, #4
.text:000000000040067C            MOV         W4, #5
.text:0000000000400680            MOV         W5, #6
.text:0000000000400684            MOV         W6, #7
.text:0000000000400688            MOV         W7, #8
.text:000000000040068C            BL          add
.text:0000000000400690            STR         W0, [X29,#28]
.text:00000000004005E8            EXPORT add
.text:00000000004005E8
.text:00000000004005E8            SUB         SP, SP, #0x30
.text:00000000004005EC            STR         W0, [SP,#28]
.text:00000000004005F0            STR         W1, [SP,#24]
.text:00000000004005F4            STR         W2, [SP,#20]
.text:00000000004005F8            STR         W3, [SP,#16]
.text:00000000004005FC            STR         W4, [SP,#12]
.text:0000000000400600            STR         W5, [SP,#8]
.text:0000000000400604            STR         W6, [SP,#4]
.text:0000000000400608            STR         W7, [SP]
.text:000000000040060C            LDR         W1, [SP,#28]
.text:0000000000400610            LDR         W0, [SP,#24]
.text:0000000000400614            ADD         W1, W1, W0
.text:0000000000400618            LDR         W0, [SP,#20]
.text:000000000040061C            ADD         W1, W1, W0
.text:0000000000400620            LDR         W0, [SP,#16]
.text:0000000000400624            ADD         W1, W1, W0
.text:0000000000400628            LDR         W0, [SP,#12]
.text:000000000040062C            ADD         W1, W1, W0
.text:0000000000400630            LDR         W0, [SP,#8]
.text:0000000000400634            ADD         W1, W1, W0
.text:0000000000400638            LDR         W0, [SP,#4]
.text:000000000040063C            ADD         W0, W1, W0
.text:0000000000400640            STR         W0, [SP,#44]
.text:0000000000400644            LDR         W0, [SP,#44]
.text:0000000000400648            ADD         SP, SP, #0x30
.text:000000000040064C            RET
```

