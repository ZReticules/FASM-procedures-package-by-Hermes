# New set of macros for defining and invoking a procedure
## Prologue
The main goal of this package is to provide a customizable and optimal set of macros for defining and invoking procedures. Unfortunately, the default macro set has three main problems:  
1. Calling conventions are fully hardcoded, so if you want to make a new one, you must write your own macros for both defining and invoking a procedure  
2. Default marcos uses a high amount of compiler and preprocessor resources for, in the author's opinion, completely unnecessary reasons  
3. Lack of some helpful features, which may be easily added, like nested procedures or a parameter list as a parameter of a procedure
## Contents
- [Procedure definition](#procedure-definition)  
	- [Argument types](#argument-types)  
	- [Calling conventions](#calling-conventions)  
	- [Examples](#examples)
- [Local variables](#local-variables)
- [Frame modes](#frame-modes)
- [Procedure invocation](#procedure-invocation)
	- [Side effects](#side-effects)
	- [Arguments](#arguments)
		- [Size and type specifiers](#size-and-type-specifiers)
		- [Constants](#constants)
		- [Memory and addresses](#memory-and-addresses)
- [New features](#new-features)  
	- [Nested procedures](#nested-procedures)
	- [List of variables](#list-of-variables)

<i><u>Important! All local variables and arguments must start with a dot in case they are plain labels based on the stack frame by the <b>`virtual`</b> directive. Therefore, it's highly recommended to use inside procedure labels that also start with a dot.</u></i> 
## Procedure definition
Macro named `.proc` is used for procedure definition. It has the following syntax:  
```asm
.proc [calling_convention] name[(arg1[:TYPE], ...)] [uses reg1 reg2...]
; procedure body
.endp
```
<i><u>Important! If you use <b>`uses`</b>, always use the full register. Use eax in x86-32 and rax in x86-64.</u></i> Only GPR registers allowed.  
### Argument types
Those argument types are allowed:  
- `BYTE`  
- `WORD`  
- `DWORD`  
- `PWORD`  
- `QWORD`  
- `TBYTE`  
- `DQWORD`  
- `QQWORD`  
- `XWORD`  
- `YWORD`  
- `ZWORD`
- Any structure made by `struct`  

All types except user-defined structures must be uppercase.  
By default, in x86-64, parameters are qwords, in x86-32 - dwords.  
  
### Conventions
Built-in calling conventions:
- `cdecl`/`c`
- `stdcall`
- `fastcall`(in x86-64)  

Default calling convention - `stdcall`. `stdcall`, `c`, and `cdecl` are aliases for fastcall in x86-64.  
### Examples 
For example, a function that returns the sum of two dwords:  
```asm
; x86-32
.proc sum(.a, .b)
	mov eax, [.a]
	add eax, [.b]
	ret
.endp
```
```asm
; x86-64
.proc sum(.a:DWORD, .b:DWORD)
	mov eax, ecx
	add eax, edx
	ret
.endp

```
For a full program example, see `example32.fasm` and `example64.fasm`.   
## local variables
Inside procedure you can use macros `.local` and `.locals/.endl`. They inherit syntax from `local` and `locals/endl` macros in the standard package, but you always need initialize them manually.  
## Frame modes
`.proc` provides 2 types of procedure frames:  

1. Standard bp-based mode. Enabled by default. By macro `.proc_frame_mode_standard`, you can also enable it for all further procedures.
2. Static sp-based mode. In that mode stackframe based on \*sp register, so you can use \*bp as you wish, but you should be very accurate with stack because any change of \*sp register in this mode lead to break of local variables and arguments addresses, so you won't be able to access them right without correction.  To enable this mode for all further procedures, you can use the `.proc_frame_mode_static` macro.  

Also, you can use `.proc_frame_mode_previous` to return to the previous frame mode for all further procedures.  
## Procedure invocation
<b><i>Macros in this package allow you to invoke a procedure ONLY inside another procedure, defined by the `.proc` macro. But you can invoke procedures that weren't declared by that macro, of course.</i></b>  
This package provides three built-in macros for procedure invocation:  

- `@stdcall`  
- `@ccall`/`@cdeclcall`  
- `@fastcall`(x86-64 only)   

In all cases, the order of passing arguments is reversed.
As with procedure definition, `@ccall`, `@cdeclcall`, and `@stdcall` in x86-64 are aliases for `@fastcall`.   
All those macros have the same syntax, similar to standard macros for procedure invocation:  
`@ccall proc [, arg1, ...]`
### Side effects
In x86-64 invocation macros, xmm5 and r11 can be destroyed at any moment. In x86-32, xmm5 and eax can also be destroyed, but if eax is an argument or part of an address, it will be restored. This system is smart, so you don't need to worry about inefficiency or safety, for example:  
`@ccall foo, [eax], eax, addr ecx + edx + 5`  
In this case, before argument loading, eax will be saved into a stack variable, then destroyed for address calculation, then restored for the second argument, and, in case it was restored earlier, it will not be restored for the first argument. Besides, if the address contains only one register, eax will not be destroyed.  
### Arguments
<u><i>All specifiers/keywords are case-sensitive and must be in lower-case.</i></u>  
#### Size and type specifiers
For almost any type of argument that you can pass in a procedure, you can use a specifier. It tells macro what size and what kind of data you want to pass, usually integer or float. List of specifiers:  

- `qword`  
- `dword`  
- `word`  
- `byte`  
- `double`  
- `float`  
- `real`(float16)  

If you don't use any specifier, it will be automatically selected based on the type of argument:   

| Type | Size in x86-32 | Size in x86-64 |  
| - | - | - |  
| Memory | How defined | How defined |  
| GPR | By itself | By itself |  
| SSE | double | double |   
| addr | dword | qword |  
| numeric literal | dword | qword |  
| separated qword | qword | qword |

Also compatibility for specifiers and argument types:

| Type\specifier | qword | dword | word | byte | double | float | real |
| - | - | - | - | - | - | - | - |
| GPR | x86-64 only | + | + | + | + | x86-64 only | + | - |
| SSE | + | + | - | - | + | + | - |
| Numeric literal | + | + | + | + | + | + | + | + |
| Memory | + | + | + | + | + | + | + | + |
| separated qword | - | - | - | - | - | - | - |
| addr | x86-64 only | + | - | - | - | - | - |


#### Constants
Like in standard invocation macros, you can pass a constant string or an array of bytes as a parameter to a procedure. It will be automatically null-terminated:
`@ccall foo, "12"`  
`@ccall foo, <0x31, 0x32>`  
Also, you can pass a wide string using the L prefix  
`@ccall wfoo, L "12"`  
Besides, you can also specify the type of a constant that you want to pass:  
`@ccall foo, <POINT 1, 2>`  
`@ccall foo, <dd 1, 2>`  
`@ccall foo, <const dd 1, 2>`  
`@ccall foo, <const POINT>` - if you want to pass a constant object of structure with default field values.  
All generated objects will be placed at the end of your procedure, and if your procedure is not used, it will not be included in the binary file.  
Size/type specifiers not allowed.  

#### Memory and addresses 
- To pass operand-memory, you must use square braces. PTR keyword not allowed.  
- To pass an address, you can use the prefix `addr` or the symbol `&`:  
`@ccall foo, addr eax + ebx + 10` equal to `@ccall foo, & eax + ebx + 10`.

#### Separated qword
You can pass a qword argument as a separated pair of dwords, like  
`@ccall foo, edx:eax`  
In that case, edx represents the high part of the qword and eax the low part. You can only use 32-bit registers and numeric literals for that.  

## New features
### Nested procedures
You can make a procedure inside another procedure using the following syntax:  
```asm
.proc [calling_convention] name[(arg1[:TYPE], ...)] [uses reg1 reg2...]
	; procedure body

	.proc [calling_convention] .name1[<captured1, ...>][(arg1[:TYPE], ...)] [uses reg1 reg2...]
	; subprocedure body

	.endp
	.proc [calling_convention] .name2[<captured1, ...>][(arg1[:TYPE], ...)] [uses reg1 reg2...]
	; subprocedure body

	.endp
	...
.endp
```
Rules:

- The nested procedure must be started with a dot. The `.proc` macro will generate a global identifier by combining the name of the external function and the internal one. Inside a nested procedure, you can appeal to it by its short name.  
- Nesting level and number of procedures have no limit.  
- A nested procedure can capture local variables or arguments from the outer procedure, but there are limitations: 
	1. You can access those variables only if the procedure is called manually by the outer procedure.  
	2. You shouldn't change the stack pointer(and, for standard frame mode, base pointer) manually before invocation.  
	3. Outer procedure and inner procedure must have the same frame mode.  
	4. Captured parameters are represented in a nested procedure as plain labels without information about type or size. If you want, you can use the `virtual` directive to specify them.  

### List of variables as a procedure parameter
You can use the va_list specifier, which allows you to load a list of arguments into a function not directly, but as a pointer to memory that contains it. Example:  
`@ccall foo, <va_list 10, [eax], "hello">`  
If you want to pass a list with only one value, you can drop the braces  
`@ccall foo, va_list "hello"`  

Values will be loaded into memory; the distance between them is at least 4 bytes in the x86-32 architecture and 8 bytes in the x86-64 architecture. The destination procedure will receive a pointer to the place of the first parameter in the list. A place will be reserved in the stack.  
Size/type specifiers not allowed.  
<i><u>Important! You can pass only one va_list parameter per call for now. Another one can rewrite one that was loaded before. It may be changed later.</u></i>  