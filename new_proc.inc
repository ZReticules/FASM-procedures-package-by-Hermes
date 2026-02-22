proc@name equ

reg_bits.ax = 0
reg_bits.cx = 1
reg_bits.dx = 2
reg_bits.bx = 3
reg_bits.sp = 4
reg_bits.bp = 5
reg_bits.si = 6
reg_bits.di = 7

sizeof.BYTE 	= 1
sizeof.WORD 	= 2
sizeof.DWORD 	= 4
sizeof.PWORD 	= 6
sizeof.QWORD 	= 8
sizeof.TBYTE 	= 10
sizeof.DQWORD 	= 16
sizeof.QQWORD 	= 32
sizeof.XWORD 	= 16
sizeof.YWORD 	= 32
sizeof.ZWORD 	= 64

macro procbuf_alloc dest, size{
	dest#@_size equ size
	local ..buf
	..buf = proc_buf@index
	dest equ proc_buf@space + ..buf
	proc_buf@index = proc_buf@index + size
	if proc_buf@index > proc_buf@counter
		proc_buf@counter = proc_buf@index
	end if
}

macro procbuf_free dest{
	proc_buf@index = proc_buf@index - dest#@_size
	restore dest, dest#@_size
}

macro .proc_frame_mode_static{
	prologue@.proc equ sp@prologue
	epilogue@.proc equ sp@epilogue
	close@.proc equ sp@close
	catchparams@.proc equ sp@catch
}

macro .proc_frame_mode_standard{
	prologue@.proc equ bp@prologue
	epilogue@.proc equ bp@epilogue
	close@.proc equ bp@close
	catchparams@.proc equ bp@catch
}

macro .proc_frame_mode_previous{
	restore prologue@.proc
	restore epilogue@.proc
	restore close@.proc
}

.proc_frame_mode_standard

macro .locals { 
	virtual proc@locals
}

macro .endl { 
  	end virtual 
}

match , {
	local cntr, result
	local _mp ; match param
	macro %preproc_cmp resl, a, b\{
		rept 1 cntr:(a) - (b)\\{
			match =0, cntr\\\{
				result equ EQ
			\\\}
			match -_mp, cntr\\\{
				result equ LT
			\\\}
			match =result, result\\\{
				result equ GT
			\\\}
		\\} 
		resl equ result
		restore result
	\}
}

rept 1 {
	local _mp1, _mp2, _mp3, _mp4
	macro .local [var]\{ 
		common
		.locals
	   	forward
	   	var@name equ var 
	   	var@type equ
	   	var@def equ
		match _mp1:_mp2, var\\{ 
			var@name equ _mp1
			var@type equ _mp2
			var@def equ rb sizeof.\\#_mp2
		\\}
		match _mp1[count], var@name\\{ 
			var@name equ _mp1
			var@def equ var@def * count
	  	\\}
	  	match _mp1|_mp2, var@name|var@type\\{
			match _mp3 =_mp2 _mp4, _any1 BYTE WORD DWORD PWORD QWORD TBYTE DQWORD QQWORD XWORD YWORD ZWORD _any2\\\{
		   		label _mp1 _mp2
		   		var@def
			rept 0\\\{\\\} rept 1 \\\{ 
		   		virtual
			   		_mp1 _mp2
			  	end virtual
			  	var@def
		  	\\\} 
		\\}
	   	common
		.endl 
	\}
}

rept 1 {
	local _mp1, _mp2
	macro make@.proc_label name, conv=stdcall\{
		match _mp1, proc@name \\{
   			if used _mp1\\#name
			_mp1\\#name:
			_mp1\\#name\\#.conv equ conv
			name:
			name\#.conv equ conv
			proc@name equ _mp1\\#name
		rept 0\\{\\} rept 1 \\{
	   		if used name
			name:
			name\#.conv equ conv
			proc@name equ name
		\\}
	\}
}

rept 1 {
   	local _mp1, _mp2, _mp3, _mp4	; match param
   	local _ip1
	local statement, reg_counter
	macro define@.proc procdef&\{ 
		\local params, regs, parambytes, localbytes, name, outer_name, catched

		proc@regs equ regs
		proc@localbytes equ localbytes
		proc@parambytes equ parambytes
		statement equ procdef
		regs equ
		params equ
		match _mp1 =uses _mp2, statement\\{
			regs equ _mp2
			statement equ _mp1
		\\}
		match _mp1(_mp2), statement\\{
			statement equ _mp1
			params equ _mp2
		\\}
		match _mp1<_mp2> | _mp3, statement | proc@name\\{
			outer_name equ _mp3
			catched equ _mp2
			statement equ _mp1
		\\}
		name equ statement
		statement equ
		match _mp1 _mp2, name\\{
			name equ _mp2
			statement equ _mp1
		\\}
	    match _mp1, name\\{
	    	match _mp2, statement\\\{
				make@.proc_label _mp1, _mp2
    		rept 0\\\{\\\}rept 1\\\{
				make@.proc_label _mp1
			\\\}
			reg_counter = 0
			match _mp2, regs\\\{
				irps _ip1, _mp2\\\\{
					reg_counter = reg_counter + 1
				\\\\}
			\\\}
			.regs_count = reg_counter
			.locals_size = localbytes
			match _mp2:_mp3, prologue@.proc:<regs> \\\{ _mp2 _mp1,parambytes,localbytes,_mp3 \\\}
			match =catched, catched\\\{rept 0\\\{\\\}match _mp2|_mp3|_mp4, outer_name|catched|catchparams@.proc\\\{
				irp _ip1, _mp3\\\\{
					_mp4 _ip1, _mp2, parambytes, localbytes
				\\\\}
			\\\}
		   	
		   	\local ..vl_cnt, ..vl, ..vl_cntr, ..buf_mem, ..buf_cntr, ..buf_idx, .._locals, ..buf_size
			
			proc@locals equ .._locals
			virtual at localbase@proc
				.._locals::
			end virtual

		   	..vl_cntr = 0
		   	va_list@counter equ ..vl_cntr
		   	va_list@count equ ..vl_cnt

		   	..buf_cntr = 0
		   	..buf_idx = 0
		   	proc_buf@counter equ ..buf_cntr
		   	proc_buf@size equ ..buf_size
		   	proc_buf@index equ ..buf_idx

			.locals 
				..vl rb ..vl_cnt
		   		..buf_mem rb ..buf_size
		   	.endl

		   	proc_buf@space equ ..buf_mem
			va@list equ ..vl

			virtual at parmbase@proc
				match _mp2, proc@name\\\{
					match _mp3|_mp4, _mp2\\\#.conv|params\\\\{
						defargs@.proc@\\\\#_mp3 _mp4
					\\\\}
				\\\}
				parambytes = $-(parmbase@proc)
			end virtual
		\\}

		macro ret operand\\{ 
			match any, operand \\\{ 
				retn operand 
			\\\}
		  	match , operand \\\{ 
		  		match _mp1:_mp2, epilogue@.proc:<regs> \\\\{ 
		  			_mp1 proc@name, proc@parambytes, proc@localbytes, _mp2 
		  		\\\\}
	  		\\\} 
	  	\\}
	\}

	macro finish@.proc\{ 
		match _mp1, proc@localbytes\\{
			virtual proc@locals
				_mp1 = $-(localbase@proc)
			end virtual
		  	match _mp2:reglist, close@.proc:<proc@regs> \\\{ 
		  		_mp2 proc@name, parmbytes, _mp1, reglist 
		  	\\\}
		\\}
		; \\}
		end if 
	\} 
}

; proc@const_literals equ
rept 1{
	local _ivp

	macro .proc args&\{ 
	 	irpv _ivp, proc@const_literals\\{
	 		_ivp
	 		restore proc@const_literals
		\\}
		define@.proc args
	\}
	macro .endp \{
		va_list@count = va_list@counter
		restore va@list, va_list@counter, va_list@count

		proc_buf@size = proc_buf@counter
		restore proc_buf@space, proc_buf@counter, proc_buf@size, proc_buf@index
	 	
	 	purge ret

	 	irpv _ivp, proc@const_literals\\{
	 		_ivp
	 		restore proc@const_literals
 		\\}
		finish@.proc

		restore proc@locals, proc@regs, proc@localbytes, proc@parambytes
		restore proc@name
	\}
}

match =x64, __architecture{
	include "new_proc64.inc"
	include "new_call64.inc"
}

match =x86, __architecture{
	include "new_proc32.inc"
	include "new_call32.inc"
}

rept 1 {
	local _mp1, _mp2, _mp3, _mp4, _mp5
	local _any1, _any2
	local _type, _def
	macro _parse@arg type, def, src\{
		match =va_list _mp1, src\\{
			define _type va list
			define _def _mp1
		rept 0\\{\\}rept 1\\{
			match =const _mp1, src\\\{
				define _type pointer lit
				\\\local ..bytes
				proc@const_literals equ ..bytes _mp1
				define _def ..bytes
			\\\}
			match =_type _mp1=,_mp2, _type src\\\{
				\\\local ..bytes
				define _type pointer lit
				match _mp3 _mp4, _mp1\\\\{
					proc@const_literals equ ..bytes src
				rept 0\\\\{\\\\} rept 1\\\\{
					proc@const_literals equ ..bytes db src, 0
				\\\\}
				define _def ..bytes
			\\\}
			match =_type _mp1 _mp2, _type src\\\{
				match [_mp3], _mp2\\\\{
					define _type _mp1 mem
					define _def _mp2
				\\\\}
				match _mp3 =_mp1 _mp4, _any1 addr & _any2\\\\{
					define _type pointer addr
					\\\\local ..addr
					virtual at _mp2
						..addr:
					end virtual
					define _def ..addr
				\\\\}
				match =addr _mp3,  _mp2\\\\{
					define _type _mp1 addr 
					\\\\local ..addr
					virtual at _mp3
						..addr:
					end virtual
					define _def ..addr
				\\\\}
				match &_mp3,  _mp2\\\\{
					define _type _mp1 addr 
					\\\\local ..addr
					virtual at _mp3
						..addr:
					end virtual
					define _def ..addr
				\\\\}
				match _mp3 =_mp2 _mp4, _any1 rax rbx rcx rdx rsi rdi rsp rbp r8 r9 r10 r11 r12 r13 r14 r15 _any2\\\\{
					define _type _mp1 GPR
					define _def _mp2
				\\\\}
				match _mp3 =_mp2 _mp4, _any1 eax ebx ecx edx esi edi esp ebp r8d r9d r10d r11d r12d r13d r14d r15d _any2\\\\{
					define _type _mp1 GPR
					define _def _mp2
				\\\\}
				match _mp3 =_mp2 _mp4, _any1 xmm0 xmm1 xmm2 xmm3 xmm4 xmm5 xmm6 xmm7 xmm8 xmm9 xmm10 xmm11 xmm12 xmm13 xmm14 xmm15 _any2\\\\{
					define _type _mp1 SSE
					define _def _mp2
				\\\\}
				match _mp3 =_mp2 _mp4, _any1 st0 st1 st2 st3 st4 st5 st6 st7 _any2\\\\{
					define _type _mp1 FPU
					define _def _mp2
				\\\\}
				match =L \\\`_mp2, _mp1 _mp2\\\\{
					define _type pointer lit
					\\\\local ..bytes
					proc@const_literals equ ..bytes du _mp2, 0
					define _def ..bytes
				\\\\}
				match =_type | \\\`_mp2, _type |  _mp2\\\\{
					define _type _mp1 lit
					\\\\local ..bytes
					proc@const_literals equ ..bytes db _mp2, 0
					define _def ..bytes
				\\\\}
				match =L _mp3, _mp2\\\\{
					match \\\\`_mp3, _mp3\\\\\{
						define _type _mp1 lit
						\\\\\local ..bytes
						proc@const_literals equ ..bytes du _mp2, 0
						define _def ..bytes
					\\\\\}
				\\\\}
				match =_type | :_mp3, _type | _mp2\\\\{
					define _type sep qword
					define _def _mp1 _mp2
				\\\\}
				match [, _mp1 \\\\{rept 0\\\\{\\\\} match =_type, _type\\\\{
					define _type _mp1 lit
					define _def _mp2
				\\\\}
			\\\}
		\\}
		match \`src, src\\{
			define _type pointer lit
			\\local ..bytes
			proc@const_literals equ ..bytes db src, 0
			define _def ..bytes
		\\}
		match _mp1 =src _mp2, _any1 xmm0 xmm1 xmm2 xmm3 xmm4 xmm5 xmm6 xmm7 xmm8 xmm9 xmm10 xmm11 xmm12 xmm13 xmm14 xmm15 _any2\\{
			define _type double SSE
			define _def src
		\\}
		match _mp1 =src _mp2, _any1 st0 st1 st2 st3 st4 st5 st6 st7 _any2\\{
			define _type double FPU
			define _def src
		\\}
		match _mp1 =src _mp2, _any1 rax rbx rcx rdx rsi rdi rsp rbp r8 r9 r10 r11 r12 r13 r14 r15 _any2\\{
			define _type qword GPR
			define _def src
		\\}
		match _mp1 =src _mp2, _any1 eax ebx ecx edx esi edi esp ebp r8d r9d r10d r11d r12d r13d r14d r15d _any2\\{
			define _type dword GPR
			define _def src
		\\}
		match _mp1 =src _mp2, _any1 ax bx cx dx si di sp bp r8w r9w r10w r11w r12w r13w r14w r15w _any2\\{
			define _type word GPR
			define _def src
		\\}
		match _mp1 =src _mp2, _any1 al ah bl bh cl ch dl dh sil dil spl bpl r8l r9l r10l r11l r12l r13l r14l r15l _any2\\{
			define _type byte GPR
			define _def src
		\\}
		match =_type [_mp1], _type src\\{
			define _type unk mem
			define _def src
		\\}
		match =_type, _type\\{
			define _type pointer lit
			define _def src
		\\}
		match _mp1 _mp2|_mp3, _type|_def\\{
			restore _type, _def
			define type _mp1 _mp2
			define def _mp3
		\\}
	\}
}

@$AX_SAVED		= 1
@$AX_DESTROYED	= 10b
@$AX_NEED_SAVE	= 100b

rept 1{
	local origin, mode_rm, sib, offset
	macro has@reg_in_addr result, mem, reg_16\{
		offset = 0
		virtual at 0
			origin = $
				inc dword mem
			align 8
			mode_rm = 0
			while mode_rm <> 0xFF
				load mode_rm byte from origin + offset
				offset = offset + 1
			end while
			load mode_rm byte from origin + offset
			load sib byte from origin + offset + 1
		end virtual
		if ~(mode_rm and 11000111b = 00000101b) & \
				((mode_rm and 11000000b <> 11000000b & mode_rm and 111b <> 100b & mode_rm and 111b = reg_bits.\#reg_16) | \
					(mode_rm and 111b = 100b & (sib and 111b = reg_bits.\#reg_16 | sib and 111000b = reg_bits.\#reg_16 shl 3)))
			result = 1
		else
			result = 0
		end if
	\}
}

rept 1{
	local _mp1
	local _start, _size
	macro @set_result val\{
		match _mp1, proc@name\\{
			\\local src_type, src_def
			match , val\\\{rept 0\\\{\\\}match _mp2, _mp1\\#.conv\\\{
				_parse@arg src_type, src_def, val
				@set_result@\\\#_mp2 src_type, src_def
			\\\}
		\\}
	\}
}