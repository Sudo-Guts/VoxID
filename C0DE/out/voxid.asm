
out/voxid.exe:     file format pei-x86-64


Disassembly of section .text:

0000000140001000 <__mingw_invalidParameterHandler>:
   140001000:	c3                   	ret
   140001001:	0f 1f 40 00          	nopl   0x0(%rax)
   140001005:	66 66 2e 0f 1f 84 00 	data16 cs nopw 0x0(%rax,%rax,1)
   14000100c:	00 00 00 00 

0000000140001010 <cpp_unhandled_exception_filter>:
   140001010:	31 d2                	xor    %edx,%edx
   140001012:	48 8b 09             	mov    (%rcx),%rcx
   140001015:	8b 01                	mov    (%rcx),%eax
   140001017:	25 ff ff ff 20       	and    $0x20ffffff,%eax
   14000101c:	3d 43 43 47 20       	cmp    $0x20474343,%eax
   140001021:	75 09                	jne    14000102c <cpp_unhandled_exception_filter+0x1c>
   140001023:	8b 51 04             	mov    0x4(%rcx),%edx
   140001026:	83 e2 01             	and    $0x1,%edx
   140001029:	83 ea 01             	sub    $0x1,%edx
   14000102c:	89 d0                	mov    %edx,%eax
   14000102e:	c3                   	ret
   14000102f:	90                   	nop

0000000140001030 <safe_flush>:
   140001030:	31 c9                	xor    %ecx,%ecx
   140001032:	e9 69 0e 00 00       	jmp    140001ea0 <fflush>
   140001037:	66 0f 1f 84 00 00 00 	nopw   0x0(%rax,%rax,1)
   14000103e:	00 00 

0000000140001040 <__mingw_SEH_signal_dispatcher>:
   140001040:	48 83 ec 48          	sub    $0x48,%rsp
   140001044:	44 8b 49 04          	mov    0x4(%rcx),%r9d
   140001048:	49 89 ca             	mov    %rcx,%r10
   14000104b:	41 83 e1 02          	and    $0x2,%r9d
   14000104f:	74 0f                	je     140001060 <__mingw_SEH_signal_dispatcher+0x20>
   140001051:	41 b9 01 00 00 00    	mov    $0x1,%r9d
   140001057:	44 89 c8             	mov    %r9d,%eax
   14000105a:	48 83 c4 48          	add    $0x48,%rsp
   14000105e:	c3                   	ret
   14000105f:	90                   	nop
   140001060:	48 89 4c 24 30       	mov    %rcx,0x30(%rsp)
   140001065:	8b 09                	mov    (%rcx),%ecx
   140001067:	48 8d 54 24 30       	lea    0x30(%rsp),%rdx
   14000106c:	44 89 4c 24 2c       	mov    %r9d,0x2c(%rsp)
   140001071:	4c 89 44 24 38       	mov    %r8,0x38(%rsp)
   140001076:	4c 89 54 24 50       	mov    %r10,0x50(%rsp)
   14000107b:	e8 78 0e 00 00       	call   140001ef8 <_XcptFilter>
   140001080:	44 8b 4c 24 2c       	mov    0x2c(%rsp),%r9d
   140001085:	83 f8 ff             	cmp    $0xffffffff,%eax
   140001088:	74 cd                	je     140001057 <__mingw_SEH_signal_dispatcher+0x17>
   14000108a:	85 c0                	test   %eax,%eax
   14000108c:	4c 8b 54 24 50       	mov    0x50(%rsp),%r10
   140001091:	74 be                	je     140001051 <__mingw_SEH_signal_dispatcher+0x11>
   140001093:	41 8b 0a             	mov    (%r10),%ecx
   140001096:	e8 4d 0e 00 00       	call   140001ee8 <_exit>
   14000109b:	90                   	nop
   14000109c:	0f 1f 40 00          	nopl   0x0(%rax)

00000001400010a0 <__tmainCRTStartup>:
   1400010a0:	41 57                	push   %r15
   1400010a2:	41 56                	push   %r14
   1400010a4:	41 55                	push   %r13
   1400010a6:	41 54                	push   %r12
   1400010a8:	55                   	push   %rbp
   1400010a9:	57                   	push   %rdi
   1400010aa:	56                   	push   %rsi
   1400010ab:	53                   	push   %rbx
   1400010ac:	48 83 ec 58          	sub    $0x58,%rsp
   1400010b0:	65 48 8b 04 25 30 00 	mov    %gs:0x30,%rax
   1400010b7:	00 00 
   1400010b9:	48 8b 70 08          	mov    0x8(%rax),%rsi
   1400010bd:	48 8b 1d ac 22 00 00 	mov    0x22ac(%rip),%rbx        # 140003370 <.refptr.__native_startup_lock>
   1400010c4:	48 8b 3d 5d 61 00 00 	mov    0x615d(%rip),%rdi        # 140007228 <__imp_Sleep>
   1400010cb:	eb 13                	jmp    1400010e0 <__tmainCRTStartup+0x40>
   1400010cd:	0f 1f 00             	nopl   (%rax)
   1400010d0:	48 39 c6             	cmp    %rax,%rsi
   1400010d3:	0f 84 b7 00 00 00    	je     140001190 <__tmainCRTStartup+0xf0>
   1400010d9:	b9 e8 03 00 00       	mov    $0x3e8,%ecx
   1400010de:	ff d7                	call   *%rdi
   1400010e0:	31 c0                	xor    %eax,%eax
   1400010e2:	f0 48 0f b1 33       	lock cmpxchg %rsi,(%rbx)
   1400010e7:	75 e7                	jne    1400010d0 <__tmainCRTStartup+0x30>
   1400010e9:	45 31 f6             	xor    %r14d,%r14d
   1400010ec:	4c 8b 25 8d 22 00 00 	mov    0x228d(%rip),%r12        # 140003380 <.refptr.__native_startup_state>
   1400010f3:	41 83 3c 24 01       	cmpl   $0x1,(%r12)
   1400010f8:	0f 84 61 03 00 00    	je     14000145f <__tmainCRTStartup+0x3bf>
   1400010fe:	45 8b 1c 24          	mov    (%r12),%r11d
   140001102:	45 85 db             	test   %r11d,%r11d
   140001105:	0f 84 b5 00 00 00    	je     1400011c0 <__tmainCRTStartup+0x120>
   14000110b:	c7 05 eb 4e 00 00 01 	movl   $0x1,0x4eeb(%rip)        # 140006000 <__bss_start__>
   140001112:	00 00 00 
   140001115:	45 85 f6             	test   %r14d,%r14d
   140001118:	0f 84 92 00 00 00    	je     1400011b0 <__tmainCRTStartup+0x110>
   14000111e:	48 8b 05 cb 4f 00 00 	mov    0x4fcb(%rip),%rax        # 1400060f0 <__dyn_tls_init_callback>
   140001125:	48 85 c0             	test   %rax,%rax
   140001128:	74 0c                	je     140001136 <__tmainCRTStartup+0x96>
   14000112a:	45 31 c0             	xor    %r8d,%r8d
   14000112d:	ba 02 00 00 00       	mov    $0x2,%edx
   140001132:	31 c9                	xor    %ecx,%ecx
   140001134:	ff d0                	call   *%rax
   140001136:	48 8b 05 bb 4f 00 00 	mov    0x4fbb(%rip),%rax        # 1400060f8 <__mingw_register_thread_local_exe_atexit_callback_ptr>
   14000113d:	48 85 c0             	test   %rax,%rax
   140001140:	74 02                	je     140001144 <__tmainCRTStartup+0xa4>
   140001142:	ff d0                	call   *%rax
   140001144:	e8 87 0c 00 00       	call   140001dd0 <__p___initenv>
   140001149:	4c 8b 05 b8 4e 00 00 	mov    0x4eb8(%rip),%r8        # 140006008 <envp>
   140001150:	8b 0d c2 4e 00 00    	mov    0x4ec2(%rip),%ecx        # 140006018 <argc>
   140001156:	4c 89 00             	mov    %r8,(%rax)
   140001159:	48 8b 15 b0 4e 00 00 	mov    0x4eb0(%rip),%rdx        # 140006010 <argv>
   140001160:	e8 7b 03 00 00       	call   1400014e0 <main>
   140001165:	8b 0d 99 4e 00 00    	mov    0x4e99(%rip),%ecx        # 140006004 <managedapp>
   14000116b:	85 c9                	test   %ecx,%ecx
   14000116d:	0f 84 e5 02 00 00    	je     140001458 <__tmainCRTStartup+0x3b8>
   140001173:	8b 15 87 4e 00 00    	mov    0x4e87(%rip),%edx        # 140006000 <__bss_start__>
   140001179:	85 d2                	test   %edx,%edx
   14000117b:	74 23                	je     1400011a0 <__tmainCRTStartup+0x100>
   14000117d:	48 83 c4 58          	add    $0x58,%rsp
   140001181:	5b                   	pop    %rbx
   140001182:	5e                   	pop    %rsi
   140001183:	5f                   	pop    %rdi
   140001184:	5d                   	pop    %rbp
   140001185:	41 5c                	pop    %r12
   140001187:	41 5d                	pop    %r13
   140001189:	41 5e                	pop    %r14
   14000118b:	41 5f                	pop    %r15
   14000118d:	c3                   	ret
   14000118e:	66 90                	xchg   %ax,%ax
   140001190:	41 be 01 00 00 00    	mov    $0x1,%r14d
   140001196:	e9 51 ff ff ff       	jmp    1400010ec <__tmainCRTStartup+0x4c>
   14000119b:	0f 1f 44 00 00       	nopl   0x0(%rax,%rax,1)
   1400011a0:	89 44 24 3c          	mov    %eax,0x3c(%rsp)
   1400011a4:	e8 27 0d 00 00       	call   140001ed0 <_cexit>
   1400011a9:	8b 44 24 3c          	mov    0x3c(%rsp),%eax
   1400011ad:	eb ce                	jmp    14000117d <__tmainCRTStartup+0xdd>
   1400011af:	90                   	nop
   1400011b0:	31 c0                	xor    %eax,%eax
   1400011b2:	48 87 03             	xchg   %rax,(%rbx)
   1400011b5:	e9 64 ff ff ff       	jmp    14000111e <__tmainCRTStartup+0x7e>
   1400011ba:	66 0f 1f 44 00 00    	nopw   0x0(%rax,%rax,1)
   1400011c0:	b9 02 00 00 00       	mov    $0x2,%ecx
   1400011c5:	41 c7 04 24 01 00 00 	movl   $0x1,(%r12)
   1400011cc:	00 
   1400011cd:	e8 ae 0c 00 00       	call   140001e80 <__acrt_iob_func>
   1400011d2:	45 31 c9             	xor    %r9d,%r9d
   1400011d5:	41 b8 04 00 00 00    	mov    $0x4,%r8d
   1400011db:	31 d2                	xor    %edx,%edx
   1400011dd:	48 89 c1             	mov    %rax,%rcx
   1400011e0:	e8 cb 0c 00 00       	call   140001eb0 <setvbuf>
   1400011e5:	48 8d 0d 44 fe ff ff 	lea    -0x1bc(%rip),%rcx        # 140001030 <safe_flush>
   1400011ec:	e8 ef 0c 00 00       	call   140001ee0 <_crt_atexit>
   1400011f1:	89 c6                	mov    %eax,%esi
   1400011f3:	85 c0                	test   %eax,%eax
   1400011f5:	0f 85 49 02 00 00    	jne    140001444 <__tmainCRTStartup+0x3a4>
   1400011fb:	e8 f0 06 00 00       	call   1400018f0 <_pei386_runtime_relocator>
   140001200:	48 8d 0d f9 fd ff ff 	lea    -0x207(%rip),%rcx        # 140001000 <__mingw_invalidParameterHandler>
   140001207:	e8 0c 0d 00 00       	call   140001f18 <_set_invalid_parameter_handler>
   14000120c:	e8 cf 0a 00 00       	call   140001ce0 <_fpreset>
   140001211:	48 8b 05 f8 20 00 00 	mov    0x20f8(%rip),%rax        # 140003310 <.refptr.__ImageBase>
   140001218:	66 81 38 4d 5a       	cmpw   $0x5a4d,(%rax)
   14000121d:	75 41                	jne    140001260 <__tmainCRTStartup+0x1c0>
   14000121f:	48 63 50 3c          	movslq 0x3c(%rax),%rdx
   140001223:	48 01 d0             	add    %rdx,%rax
   140001226:	81 38 50 45 00 00    	cmpl   $0x4550,(%rax)
   14000122c:	75 32                	jne    140001260 <__tmainCRTStartup+0x1c0>
   14000122e:	0f b7 50 18          	movzwl 0x18(%rax),%edx
   140001232:	66 81 fa 0b 01       	cmp    $0x10b,%dx
   140001237:	0f 84 e8 01 00 00    	je     140001425 <__tmainCRTStartup+0x385>
   14000123d:	66 81 fa 0b 02       	cmp    $0x20b,%dx
   140001242:	75 1c                	jne    140001260 <__tmainCRTStartup+0x1c0>
   140001244:	83 b8 84 00 00 00 0e 	cmpl   $0xe,0x84(%rax)
   14000124b:	76 13                	jbe    140001260 <__tmainCRTStartup+0x1c0>
   14000124d:	44 8b 88 f8 00 00 00 	mov    0xf8(%rax),%r9d
   140001254:	31 f6                	xor    %esi,%esi
   140001256:	45 85 c9             	test   %r9d,%r9d
   140001259:	40 0f 95 c6          	setne  %sil
   14000125d:	0f 1f 00             	nopl   (%rax)
   140001260:	48 8b 05 f9 20 00 00 	mov    0x20f9(%rip),%rax        # 140003360 <.refptr.__mingw_app_type>
   140001267:	89 35 97 4d 00 00    	mov    %esi,0x4d97(%rip)        # 140006004 <managedapp>
   14000126d:	44 8b 00             	mov    (%rax),%r8d
   140001270:	45 85 c0             	test   %r8d,%r8d
   140001273:	0f 85 2f 01 00 00    	jne    1400013a8 <__tmainCRTStartup+0x308>
   140001279:	b9 01 00 00 00       	mov    $0x1,%ecx
   14000127e:	e8 8d 0c 00 00       	call   140001f10 <__set_app_type>
   140001283:	e8 08 0c 00 00       	call   140001e90 <__p__fmode>
   140001288:	48 8b 15 61 21 00 00 	mov    0x2161(%rip),%rdx        # 1400033f0 <.refptr._fmode>
   14000128f:	8b 12                	mov    (%rdx),%edx
   140001291:	89 10                	mov    %edx,(%rax)
   140001293:	e8 f0 0b 00 00       	call   140001e88 <__p__commode>
   140001298:	48 8b 15 31 21 00 00 	mov    0x2131(%rip),%rdx        # 1400033d0 <.refptr._commode>
   14000129f:	8b 12                	mov    (%rdx),%edx
   1400012a1:	89 10                	mov    %edx,(%rax)
   1400012a3:	e8 38 03 00 00       	call   1400015e0 <_setargv>
   1400012a8:	85 c0                	test   %eax,%eax
   1400012aa:	0f 88 e9 00 00 00    	js     140001399 <__tmainCRTStartup+0x2f9>
   1400012b0:	48 8b 05 39 20 00 00 	mov    0x2039(%rip),%rax        # 1400032f0 <.refptr._MINGW_INSTALL_DEBUG_MATHERR>
   1400012b7:	83 38 01             	cmpl   $0x1,(%rax)
   1400012ba:	0f 84 54 01 00 00    	je     140001414 <__tmainCRTStartup+0x374>
   1400012c0:	48 8b 05 79 20 00 00 	mov    0x2079(%rip),%rax        # 140003340 <.refptr.__globallocalestatus>
   1400012c7:	83 38 ff             	cmpl   $0xffffffff,(%rax)
   1400012ca:	0f 84 35 01 00 00    	je     140001405 <__tmainCRTStartup+0x365>
   1400012d0:	48 8b 15 e9 20 00 00 	mov    0x20e9(%rip),%rdx        # 1400033c0 <.refptr.__xi_z>
   1400012d7:	48 8b 0d d2 20 00 00 	mov    0x20d2(%rip),%rcx        # 1400033b0 <.refptr.__xi_a>
   1400012de:	e8 25 0c 00 00       	call   140001f08 <_initterm_e>
   1400012e3:	85 c0                	test   %eax,%eax
   1400012e5:	0f 85 63 01 00 00    	jne    14000144e <__tmainCRTStartup+0x3ae>
   1400012eb:	48 8b 05 1e 21 00 00 	mov    0x211e(%rip),%rax        # 140003410 <.refptr._newmode>
   1400012f2:	4c 8d 05 0f 4d 00 00 	lea    0x4d0f(%rip),%r8        # 140006008 <envp>
   1400012f9:	48 8d 15 10 4d 00 00 	lea    0x4d10(%rip),%rdx        # 140006010 <argv>
   140001300:	48 8d 0d 11 4d 00 00 	lea    0x4d11(%rip),%rcx        # 140006018 <argc>
   140001307:	8b 00                	mov    (%rax),%eax
   140001309:	89 44 24 4c          	mov    %eax,0x4c(%rsp)
   14000130d:	48 8b 05 cc 20 00 00 	mov    0x20cc(%rip),%rax        # 1400033e0 <.refptr._dowildcard>
   140001314:	44 8b 08             	mov    (%rax),%r9d
   140001317:	48 8d 44 24 4c       	lea    0x4c(%rsp),%rax
   14000131c:	48 89 44 24 20       	mov    %rax,0x20(%rsp)
   140001321:	e8 ea 0a 00 00       	call   140001e10 <__getmainargs>
   140001326:	85 c0                	test   %eax,%eax
   140001328:	78 6f                	js     140001399 <__tmainCRTStartup+0x2f9>
   14000132a:	8b 2d e8 4c 00 00    	mov    0x4ce8(%rip),%ebp        # 140006018 <argc>
   140001330:	8d 4d 01             	lea    0x1(%rbp),%ecx
   140001333:	48 63 c9             	movslq %ecx,%rcx
   140001336:	48 c1 e1 03          	shl    $0x3,%rcx
   14000133a:	e8 29 0c 00 00       	call   140001f68 <malloc>
   14000133f:	49 89 c5             	mov    %rax,%r13
   140001342:	48 85 c0             	test   %rax,%rax
   140001345:	74 52                	je     140001399 <__tmainCRTStartup+0x2f9>
   140001347:	85 ed                	test   %ebp,%ebp
   140001349:	7e 7a                	jle    1400013c5 <__tmainCRTStartup+0x325>
   14000134b:	4c 8b 3d be 4c 00 00 	mov    0x4cbe(%rip),%r15        # 140006010 <argv>
   140001352:	be 01 00 00 00       	mov    $0x1,%esi
   140001357:	eb 20                	jmp    140001379 <__tmainCRTStartup+0x2d9>
   140001359:	0f 1f 80 00 00 00 00 	nopl   0x0(%rax)
   140001360:	49 8b 54 f7 f8       	mov    -0x8(%r15,%rsi,8),%rdx
   140001365:	49 89 f8             	mov    %rdi,%r8
   140001368:	48 89 c1             	mov    %rax,%rcx
   14000136b:	e8 c0 0b 00 00       	call   140001f30 <memcpy>
   140001370:	48 39 f5             	cmp    %rsi,%rbp
   140001373:	74 4b                	je     1400013c0 <__tmainCRTStartup+0x320>
   140001375:	48 83 c6 01          	add    $0x1,%rsi
   140001379:	49 8b 4c f7 f8       	mov    -0x8(%r15,%rsi,8),%rcx
   14000137e:	e8 ed 0a 00 00       	call   140001e70 <strlen>
   140001383:	48 8d 78 01          	lea    0x1(%rax),%rdi
   140001387:	48 89 f9             	mov    %rdi,%rcx
   14000138a:	e8 d9 0b 00 00       	call   140001f68 <malloc>
   14000138f:	49 89 44 f5 f8       	mov    %rax,-0x8(%r13,%rsi,8)
   140001394:	48 85 c0             	test   %rax,%rax
   140001397:	75 c7                	jne    140001360 <__tmainCRTStartup+0x2c0>
   140001399:	b9 08 00 00 00       	mov    $0x8,%ecx
   14000139e:	e8 3d 0a 00 00       	call   140001de0 <_amsg_exit>
   1400013a3:	0f 1f 44 00 00       	nopl   0x0(%rax,%rax,1)
   1400013a8:	b9 02 00 00 00       	mov    $0x2,%ecx
   1400013ad:	e8 5e 0b 00 00       	call   140001f10 <__set_app_type>
   1400013b2:	e9 cc fe ff ff       	jmp    140001283 <__tmainCRTStartup+0x1e3>
   1400013b7:	66 0f 1f 84 00 00 00 	nopw   0x0(%rax,%rax,1)
   1400013be:	00 00 
   1400013c0:	49 8d 44 ed 00       	lea    0x0(%r13,%rbp,8),%rax
   1400013c5:	48 c7 00 00 00 00 00 	movq   $0x0,(%rax)
   1400013cc:	48 8d 0d 3d fc ff ff 	lea    -0x3c3(%rip),%rcx        # 140001010 <cpp_unhandled_exception_filter>
   1400013d3:	4c 89 2d 36 4c 00 00 	mov    %r13,0x4c36(%rip)        # 140006010 <argv>
   1400013da:	ff 15 40 5e 00 00    	call   *0x5e40(%rip)        # 140007220 <__imp_SetUnhandledExceptionFilter>
   1400013e0:	48 8b 15 b9 1f 00 00 	mov    0x1fb9(%rip),%rdx        # 1400033a0 <.refptr.__xc_z>
   1400013e7:	48 8b 0d a2 1f 00 00 	mov    0x1fa2(%rip),%rcx        # 140003390 <.refptr.__xc_a>
   1400013ee:	e8 0d 0b 00 00       	call   140001f00 <_initterm>
   1400013f3:	e8 c8 01 00 00       	call   1400015c0 <__main>
   1400013f8:	41 c7 04 24 02 00 00 	movl   $0x2,(%r12)
   1400013ff:	00 
   140001400:	e9 10 fd ff ff       	jmp    140001115 <__tmainCRTStartup+0x75>
   140001405:	b9 ff ff ff ff       	mov    $0xffffffff,%ecx
   14000140a:	e8 41 0b 00 00       	call   140001f50 <_configthreadlocale>
   14000140f:	e9 bc fe ff ff       	jmp    1400012d0 <__tmainCRTStartup+0x230>
   140001414:	48 8d 0d d5 01 00 00 	lea    0x1d5(%rip),%rcx        # 1400015f0 <_matherr>
   14000141b:	e8 b0 08 00 00       	call   140001cd0 <__mingw_setusermatherr>
   140001420:	e9 9b fe ff ff       	jmp    1400012c0 <__tmainCRTStartup+0x220>
   140001425:	83 78 74 0e          	cmpl   $0xe,0x74(%rax)
   140001429:	0f 86 31 fe ff ff    	jbe    140001260 <__tmainCRTStartup+0x1c0>
   14000142f:	44 8b 90 e8 00 00 00 	mov    0xe8(%rax),%r10d
   140001436:	31 f6                	xor    %esi,%esi
   140001438:	45 85 d2             	test   %r10d,%r10d
   14000143b:	40 0f 95 c6          	setne  %sil
   14000143f:	e9 1c fe ff ff       	jmp    140001260 <__tmainCRTStartup+0x1c0>
   140001444:	b9 1a 00 00 00       	mov    $0x1a,%ecx
   140001449:	e8 92 09 00 00       	call   140001de0 <_amsg_exit>
   14000144e:	b9 0a 00 00 00       	mov    $0xa,%ecx
   140001453:	e8 88 09 00 00       	call   140001de0 <_amsg_exit>
   140001458:	89 c1                	mov    %eax,%ecx
   14000145a:	e8 c9 0a 00 00       	call   140001f28 <exit>
   14000145f:	b9 1f 00 00 00       	mov    $0x1f,%ecx
   140001464:	e8 77 09 00 00       	call   140001de0 <_amsg_exit>
   140001469:	90                   	nop
   14000146a:	66 0f 1f 44 00 00    	nopw   0x0(%rax,%rax,1)

0000000140001470 <WinMainCRTStartup>:
   140001470:	48 8b 05 e9 1e 00 00 	mov    0x1ee9(%rip),%rax        # 140003360 <.refptr.__mingw_app_type>
   140001477:	c7 00 01 00 00 00    	movl   $0x1,(%rax)
   14000147d:	e9 1e fc ff ff       	jmp    1400010a0 <__tmainCRTStartup>
   140001482:	0f 1f 00             	nopl   (%rax)
   140001485:	66 66 2e 0f 1f 84 00 	data16 cs nopw 0x0(%rax,%rax,1)
   14000148c:	00 00 00 00 

0000000140001490 <mainCRTStartup>:
   140001490:	48 8b 05 c9 1e 00 00 	mov    0x1ec9(%rip),%rax        # 140003360 <.refptr.__mingw_app_type>
   140001497:	c7 00 00 00 00 00    	movl   $0x0,(%rax)
   14000149d:	e9 fe fb ff ff       	jmp    1400010a0 <__tmainCRTStartup>
   1400014a2:	0f 1f 00             	nopl   (%rax)
   1400014a5:	66 66 2e 0f 1f 84 00 	data16 cs nopw 0x0(%rax,%rax,1)
   1400014ac:	00 00 00 00 

00000001400014b0 <atexit>:
   1400014b0:	e9 2b 0a 00 00       	jmp    140001ee0 <_crt_atexit>
   1400014b5:	90                   	nop
   1400014b6:	90                   	nop
   1400014b7:	90                   	nop
   1400014b8:	90                   	nop
   1400014b9:	90                   	nop
   1400014ba:	90                   	nop
   1400014bb:	90                   	nop
   1400014bc:	90                   	nop
   1400014bd:	90                   	nop
   1400014be:	90                   	nop
   1400014bf:	90                   	nop

00000001400014c0 <__gcc_register_frame>:
   1400014c0:	48 8d 0d 09 00 00 00 	lea    0x9(%rip),%rcx        # 1400014d0 <__gcc_deregister_frame>
   1400014c7:	e9 e4 ff ff ff       	jmp    1400014b0 <atexit>
   1400014cc:	0f 1f 40 00          	nopl   0x0(%rax)

00000001400014d0 <__gcc_deregister_frame>:
   1400014d0:	c3                   	ret
   1400014d1:	90                   	nop
   1400014d2:	90                   	nop
   1400014d3:	90                   	nop
   1400014d4:	90                   	nop
   1400014d5:	90                   	nop
   1400014d6:	90                   	nop
   1400014d7:	90                   	nop
   1400014d8:	90                   	nop
   1400014d9:	90                   	nop
   1400014da:	90                   	nop
   1400014db:	90                   	nop
   1400014dc:	90                   	nop
   1400014dd:	90                   	nop
   1400014de:	90                   	nop
   1400014df:	90                   	nop

00000001400014e0 <main>:
   1400014e0:	55                   	push   %rbp
   1400014e1:	48 89 e5             	mov    %rsp,%rbp
   1400014e4:	48 83 ec 20          	sub    $0x20,%rsp
   1400014e8:	e8 d3 00 00 00       	call   1400015c0 <__main>
   1400014ed:	48 8d 05 1c 1b 00 00 	lea    0x1b1c(%rip),%rax        # 140003010 <.rdata>
   1400014f4:	48 89 c1             	mov    %rax,%rcx
   1400014f7:	e8 ac 09 00 00       	call   140001ea8 <puts>
   1400014fc:	b8 00 00 00 00       	mov    $0x0,%eax
   140001501:	48 83 c4 20          	add    $0x20,%rsp
   140001505:	5d                   	pop    %rbp
   140001506:	c3                   	ret
   140001507:	90                   	nop
   140001508:	90                   	nop
   140001509:	90                   	nop
   14000150a:	90                   	nop
   14000150b:	90                   	nop
   14000150c:	90                   	nop
   14000150d:	90                   	nop
   14000150e:	90                   	nop
   14000150f:	90                   	nop

0000000140001510 <__do_global_dtors>:
   140001510:	48 83 ec 28          	sub    $0x28,%rsp
   140001514:	48 8b 05 e5 0a 00 00 	mov    0xae5(%rip),%rax        # 140002000 <__data_start__>
   14000151b:	48 8b 10             	mov    (%rax),%rdx
   14000151e:	48 85 d2             	test   %rdx,%rdx
   140001521:	74 29                	je     14000154c <__do_global_dtors+0x3c>
   140001523:	66 90                	xchg   %ax,%ax
   140001525:	66 66 2e 0f 1f 84 00 	data16 cs nopw 0x0(%rax,%rax,1)
   14000152c:	00 00 00 00 
   140001530:	48 83 c0 08          	add    $0x8,%rax
   140001534:	48 89 05 c5 0a 00 00 	mov    %rax,0xac5(%rip)        # 140002000 <__data_start__>
   14000153b:	ff d2                	call   *%rdx
   14000153d:	48 8b 05 bc 0a 00 00 	mov    0xabc(%rip),%rax        # 140002000 <__data_start__>
   140001544:	48 8b 10             	mov    (%rax),%rdx
   140001547:	48 85 d2             	test   %rdx,%rdx
   14000154a:	75 e4                	jne    140001530 <__do_global_dtors+0x20>
   14000154c:	48 83 c4 28          	add    $0x28,%rsp
   140001550:	c3                   	ret
   140001551:	0f 1f 40 00          	nopl   0x0(%rax)
   140001555:	66 66 2e 0f 1f 84 00 	data16 cs nopw 0x0(%rax,%rax,1)
   14000155c:	00 00 00 00 

0000000140001560 <__do_global_ctors>:
   140001560:	56                   	push   %rsi
   140001561:	53                   	push   %rbx
   140001562:	48 83 ec 28          	sub    $0x28,%rsp
   140001566:	48 8b 35 93 1d 00 00 	mov    0x1d93(%rip),%rsi        # 140003300 <.refptr.__CTOR_LIST__>
   14000156d:	48 8b 1e             	mov    (%rsi),%rbx
   140001570:	48 83 fb ff          	cmp    $0xffffffffffffffff,%rbx
   140001574:	74 2a                	je     1400015a0 <__do_global_ctors+0x40>
   140001576:	48 85 db             	test   %rbx,%rbx
   140001579:	74 0e                	je     140001589 <__do_global_ctors+0x29>
   14000157b:	0f 1f 44 00 00       	nopl   0x0(%rax,%rax,1)
   140001580:	ff 14 de             	call   *(%rsi,%rbx,8)
   140001583:	48 83 eb 01          	sub    $0x1,%rbx
   140001587:	75 f7                	jne    140001580 <__do_global_ctors+0x20>
   140001589:	48 8d 0d 80 ff ff ff 	lea    -0x80(%rip),%rcx        # 140001510 <__do_global_dtors>
   140001590:	48 83 c4 28          	add    $0x28,%rsp
   140001594:	5b                   	pop    %rbx
   140001595:	5e                   	pop    %rsi
   140001596:	e9 15 ff ff ff       	jmp    1400014b0 <atexit>
   14000159b:	0f 1f 44 00 00       	nopl   0x0(%rax,%rax,1)
   1400015a0:	31 c0                	xor    %eax,%eax
   1400015a2:	0f 1f 00             	nopl   (%rax)
   1400015a5:	66 66 2e 0f 1f 84 00 	data16 cs nopw 0x0(%rax,%rax,1)
   1400015ac:	00 00 00 00 
   1400015b0:	48 89 c3             	mov    %rax,%rbx
   1400015b3:	48 83 c0 01          	add    $0x1,%rax
   1400015b7:	48 83 3c c6 00       	cmpq   $0x0,(%rsi,%rax,8)
   1400015bc:	75 f2                	jne    1400015b0 <__do_global_ctors+0x50>
   1400015be:	eb b6                	jmp    140001576 <__do_global_ctors+0x16>

00000001400015c0 <__main>:
   1400015c0:	8b 05 5a 4a 00 00    	mov    0x4a5a(%rip),%eax        # 140006020 <initialized.0>
   1400015c6:	85 c0                	test   %eax,%eax
   1400015c8:	74 06                	je     1400015d0 <__main+0x10>
   1400015ca:	c3                   	ret
   1400015cb:	0f 1f 44 00 00       	nopl   0x0(%rax,%rax,1)
   1400015d0:	c7 05 46 4a 00 00 01 	movl   $0x1,0x4a46(%rip)        # 140006020 <initialized.0>
   1400015d7:	00 00 00 
   1400015da:	eb 84                	jmp    140001560 <__do_global_ctors>
   1400015dc:	90                   	nop
   1400015dd:	90                   	nop
   1400015de:	90                   	nop
   1400015df:	90                   	nop

00000001400015e0 <_setargv>:
   1400015e0:	31 c0                	xor    %eax,%eax
   1400015e2:	c3                   	ret
   1400015e3:	90                   	nop
   1400015e4:	90                   	nop
   1400015e5:	90                   	nop
   1400015e6:	90                   	nop
   1400015e7:	90                   	nop
   1400015e8:	90                   	nop
   1400015e9:	90                   	nop
   1400015ea:	90                   	nop
   1400015eb:	90                   	nop
   1400015ec:	90                   	nop
   1400015ed:	90                   	nop
   1400015ee:	90                   	nop
   1400015ef:	90                   	nop

00000001400015f0 <_matherr>:
   1400015f0:	56                   	push   %rsi
   1400015f1:	53                   	push   %rbx
   1400015f2:	48 83 ec 78          	sub    $0x78,%rsp
   1400015f6:	0f 11 74 24 40       	movups %xmm6,0x40(%rsp)
   1400015fb:	0f 11 7c 24 50       	movups %xmm7,0x50(%rsp)
   140001600:	44 0f 11 44 24 60    	movups %xmm8,0x60(%rsp)
   140001606:	83 39 06             	cmpl   $0x6,(%rcx)
   140001609:	0f 87 cd 00 00 00    	ja     1400016dc <_matherr+0xec>
   14000160f:	8b 01                	mov    (%rcx),%eax
   140001611:	48 8d 15 3c 1b 00 00 	lea    0x1b3c(%rip),%rdx        # 140003154 <.rdata+0x124>
   140001618:	48 63 04 82          	movslq (%rdx,%rax,4),%rax
   14000161c:	48 01 d0             	add    %rdx,%rax
   14000161f:	ff e0                	jmp    *%rax
   140001621:	0f 1f 80 00 00 00 00 	nopl   0x0(%rax)
   140001628:	48 8d 1d 20 1a 00 00 	lea    0x1a20(%rip),%rbx        # 14000304f <.rdata+0x1f>
   14000162f:	f2 44 0f 10 41 20    	movsd  0x20(%rcx),%xmm8
   140001635:	f2 0f 10 79 18       	movsd  0x18(%rcx),%xmm7
   14000163a:	f2 0f 10 71 10       	movsd  0x10(%rcx),%xmm6
   14000163f:	48 8b 71 08          	mov    0x8(%rcx),%rsi
   140001643:	b9 02 00 00 00       	mov    $0x2,%ecx
   140001648:	e8 33 08 00 00       	call   140001e80 <__acrt_iob_func>
   14000164d:	f2 44 0f 11 44 24 30 	movsd  %xmm8,0x30(%rsp)
   140001654:	49 89 d8             	mov    %rbx,%r8
   140001657:	48 8d 15 ca 1a 00 00 	lea    0x1aca(%rip),%rdx        # 140003128 <.rdata+0xf8>
   14000165e:	f2 0f 11 7c 24 28    	movsd  %xmm7,0x28(%rsp)
   140001664:	49 89 f1             	mov    %rsi,%r9
   140001667:	48 89 c1             	mov    %rax,%rcx
   14000166a:	f2 0f 11 74 24 20    	movsd  %xmm6,0x20(%rsp)
   140001670:	e8 fb 06 00 00       	call   140001d70 <fprintf>
   140001675:	90                   	nop
   140001676:	0f 10 74 24 40       	movups 0x40(%rsp),%xmm6
   14000167b:	0f 10 7c 24 50       	movups 0x50(%rsp),%xmm7
   140001680:	31 c0                	xor    %eax,%eax
   140001682:	44 0f 10 44 24 60    	movups 0x60(%rsp),%xmm8
   140001688:	48 83 c4 78          	add    $0x78,%rsp
   14000168c:	5b                   	pop    %rbx
   14000168d:	5e                   	pop    %rsi
   14000168e:	c3                   	ret
   14000168f:	90                   	nop
   140001690:	48 8d 1d 99 19 00 00 	lea    0x1999(%rip),%rbx        # 140003030 <.rdata>
   140001697:	eb 96                	jmp    14000162f <_matherr+0x3f>
   140001699:	0f 1f 80 00 00 00 00 	nopl   0x0(%rax)
   1400016a0:	48 8d 1d e9 19 00 00 	lea    0x19e9(%rip),%rbx        # 140003090 <.rdata+0x60>
   1400016a7:	eb 86                	jmp    14000162f <_matherr+0x3f>
   1400016a9:	0f 1f 80 00 00 00 00 	nopl   0x0(%rax)
   1400016b0:	48 8d 1d b9 19 00 00 	lea    0x19b9(%rip),%rbx        # 140003070 <.rdata+0x40>
   1400016b7:	e9 73 ff ff ff       	jmp    14000162f <_matherr+0x3f>
   1400016bc:	0f 1f 40 00          	nopl   0x0(%rax)
   1400016c0:	48 8d 1d 19 1a 00 00 	lea    0x1a19(%rip),%rbx        # 1400030e0 <.rdata+0xb0>
   1400016c7:	e9 63 ff ff ff       	jmp    14000162f <_matherr+0x3f>
   1400016cc:	0f 1f 40 00          	nopl   0x0(%rax)
   1400016d0:	48 8d 1d e1 19 00 00 	lea    0x19e1(%rip),%rbx        # 1400030b8 <.rdata+0x88>
   1400016d7:	e9 53 ff ff ff       	jmp    14000162f <_matherr+0x3f>
   1400016dc:	48 8d 1d 33 1a 00 00 	lea    0x1a33(%rip),%rbx        # 140003116 <.rdata+0xe6>
   1400016e3:	e9 47 ff ff ff       	jmp    14000162f <_matherr+0x3f>
   1400016e8:	90                   	nop
   1400016e9:	90                   	nop
   1400016ea:	90                   	nop
   1400016eb:	90                   	nop
   1400016ec:	90                   	nop
   1400016ed:	90                   	nop
   1400016ee:	90                   	nop
   1400016ef:	90                   	nop

00000001400016f0 <__report_error>:
   1400016f0:	56                   	push   %rsi
   1400016f1:	53                   	push   %rbx
   1400016f2:	48 83 ec 38          	sub    $0x38,%rsp
   1400016f6:	48 89 cb             	mov    %rcx,%rbx
   1400016f9:	48 8d 44 24 58       	lea    0x58(%rsp),%rax
   1400016fe:	b9 02 00 00 00       	mov    $0x2,%ecx
   140001703:	4c 89 44 24 60       	mov    %r8,0x60(%rsp)
   140001708:	4c 89 4c 24 68       	mov    %r9,0x68(%rsp)
   14000170d:	48 89 54 24 58       	mov    %rdx,0x58(%rsp)
   140001712:	48 89 44 24 28       	mov    %rax,0x28(%rsp)
   140001717:	e8 64 07 00 00       	call   140001e80 <__acrt_iob_func>
   14000171c:	48 8d 15 4d 1a 00 00 	lea    0x1a4d(%rip),%rdx        # 140003170 <.rdata>
   140001723:	48 89 c1             	mov    %rax,%rcx
   140001726:	e8 45 06 00 00       	call   140001d70 <fprintf>
   14000172b:	48 8b 74 24 28       	mov    0x28(%rsp),%rsi
   140001730:	b9 02 00 00 00       	mov    $0x2,%ecx
   140001735:	e8 46 07 00 00       	call   140001e80 <__acrt_iob_func>
   14000173a:	48 89 da             	mov    %rbx,%rdx
   14000173d:	49 89 f0             	mov    %rsi,%r8
   140001740:	48 89 c1             	mov    %rax,%rcx
   140001743:	e8 e8 05 00 00       	call   140001d30 <vfprintf>
   140001748:	e8 d3 07 00 00       	call   140001f20 <abort>
   14000174d:	90                   	nop
   14000174e:	66 90                	xchg   %ax,%ax

0000000140001750 <mark_section_writable>:
   140001750:	56                   	push   %rsi
   140001751:	53                   	push   %rbx
   140001752:	48 83 ec 58          	sub    $0x58,%rsp
   140001756:	44 8b 0d 27 49 00 00 	mov    0x4927(%rip),%r9d        # 140006084 <maxSections>
   14000175d:	49 89 c8             	mov    %rcx,%r8
   140001760:	45 85 c9             	test   %r9d,%r9d
   140001763:	0f 8e 57 01 00 00    	jle    1400018c0 <mark_section_writable+0x170>
   140001769:	48 8b 05 18 49 00 00 	mov    0x4918(%rip),%rax        # 140006088 <the_secs>
   140001770:	31 c9                	xor    %ecx,%ecx
   140001772:	48 83 c0 18          	add    $0x18,%rax
   140001776:	66 2e 0f 1f 84 00 00 	cs nopw 0x0(%rax,%rax,1)
   14000177d:	00 00 00 
   140001780:	48 8b 10             	mov    (%rax),%rdx
   140001783:	49 39 d0             	cmp    %rdx,%r8
   140001786:	72 14                	jb     14000179c <mark_section_writable+0x4c>
   140001788:	4c 8b 50 08          	mov    0x8(%rax),%r10
   14000178c:	45 8b 52 08          	mov    0x8(%r10),%r10d
   140001790:	4c 01 d2             	add    %r10,%rdx
   140001793:	49 39 d0             	cmp    %rdx,%r8
   140001796:	0f 82 cb 00 00 00    	jb     140001867 <mark_section_writable+0x117>
   14000179c:	83 c1 01             	add    $0x1,%ecx
   14000179f:	48 83 c0 28          	add    $0x28,%rax
   1400017a3:	44 39 c9             	cmp    %r9d,%ecx
   1400017a6:	75 d8                	jne    140001780 <mark_section_writable+0x30>
   1400017a8:	48 8b 0d 61 1b 00 00 	mov    0x1b61(%rip),%rcx        # 140003310 <.refptr.__ImageBase>
   1400017af:	48 63 41 3c          	movslq 0x3c(%rcx),%rax
   1400017b3:	48 01 c8             	add    %rcx,%rax
   1400017b6:	0f b7 50 06          	movzwl 0x6(%rax),%edx
   1400017ba:	66 85 d2             	test   %dx,%dx
   1400017bd:	74 3d                	je     1400017fc <mark_section_writable+0xac>
   1400017bf:	44 0f b7 58 14       	movzwl 0x14(%rax),%r11d
   1400017c4:	4d 89 c2             	mov    %r8,%r10
   1400017c7:	49 29 ca             	sub    %rcx,%r10
   1400017ca:	4a 8d 5c 18 18       	lea    0x18(%rax,%r11,1),%rbx
   1400017cf:	8d 42 ff             	lea    -0x1(%rdx),%eax
   1400017d2:	48 8d 04 80          	lea    (%rax,%rax,4),%rax
   1400017d6:	4c 8d 5c c3 28       	lea    0x28(%rbx,%rax,8),%r11
   1400017db:	0f 1f 44 00 00       	nopl   0x0(%rax,%rax,1)
   1400017e0:	8b 53 0c             	mov    0xc(%rbx),%edx
   1400017e3:	48 89 d0             	mov    %rdx,%rax
   1400017e6:	49 39 d2             	cmp    %rdx,%r10
   1400017e9:	72 08                	jb     1400017f3 <mark_section_writable+0xa3>
   1400017eb:	03 43 08             	add    0x8(%rbx),%eax
   1400017ee:	49 39 c2             	cmp    %rax,%r10
   1400017f1:	72 1d                	jb     140001810 <mark_section_writable+0xc0>
   1400017f3:	48 83 c3 28          	add    $0x28,%rbx
   1400017f7:	4c 39 db             	cmp    %r11,%rbx
   1400017fa:	75 e4                	jne    1400017e0 <mark_section_writable+0x90>
   1400017fc:	4c 89 c2             	mov    %r8,%rdx
   1400017ff:	48 8d 0d 8a 19 00 00 	lea    0x198a(%rip),%rcx        # 140003190 <.rdata+0x20>
   140001806:	e8 e5 fe ff ff       	call   1400016f0 <__report_error>
   14000180b:	0f 1f 44 00 00       	nopl   0x0(%rax,%rax,1)
   140001810:	8b 53 0c             	mov    0xc(%rbx),%edx
   140001813:	4b 8d 34 89          	lea    (%r9,%r9,4),%rsi
   140001817:	48 8b 05 6a 48 00 00 	mov    0x486a(%rip),%rax        # 140006088 <the_secs>
   14000181e:	41 b8 30 00 00 00    	mov    $0x30,%r8d
   140001824:	48 c1 e6 03          	shl    $0x3,%rsi
   140001828:	48 01 f0             	add    %rsi,%rax
   14000182b:	48 01 d1             	add    %rdx,%rcx
   14000182e:	48 8d 54 24 20       	lea    0x20(%rsp),%rdx
   140001833:	48 89 58 20          	mov    %rbx,0x20(%rax)
   140001837:	c7 00 00 00 00 00    	movl   $0x0,(%rax)
   14000183d:	48 89 48 18          	mov    %rcx,0x18(%rax)
   140001841:	ff 15 f1 59 00 00    	call   *0x59f1(%rip)        # 140007238 <__imp_VirtualQuery>
   140001847:	48 85 c0             	test   %rax,%rax
   14000184a:	74 7c                	je     1400018c8 <mark_section_writable+0x178>
   14000184c:	8b 44 24 44          	mov    0x44(%rsp),%eax
   140001850:	8d 50 fc             	lea    -0x4(%rax),%edx
   140001853:	83 e2 fb             	and    $0xfffffffb,%edx
   140001856:	74 08                	je     140001860 <mark_section_writable+0x110>
   140001858:	8d 50 c0             	lea    -0x40(%rax),%edx
   14000185b:	83 e2 bf             	and    $0xffffffbf,%edx
   14000185e:	75 10                	jne    140001870 <mark_section_writable+0x120>
   140001860:	83 05 1d 48 00 00 01 	addl   $0x1,0x481d(%rip)        # 140006084 <maxSections>
   140001867:	48 83 c4 58          	add    $0x58,%rsp
   14000186b:	5b                   	pop    %rbx
   14000186c:	5e                   	pop    %rsi
   14000186d:	c3                   	ret
   14000186e:	66 90                	xchg   %ax,%ax
   140001870:	83 f8 02             	cmp    $0x2,%eax
   140001873:	48 8b 4c 24 20       	mov    0x20(%rsp),%rcx
   140001878:	48 8b 54 24 38       	mov    0x38(%rsp),%rdx
   14000187d:	41 b8 40 00 00 00    	mov    $0x40,%r8d
   140001883:	b8 04 00 00 00       	mov    $0x4,%eax
   140001888:	44 0f 44 c0          	cmove  %eax,%r8d
   14000188c:	48 03 35 f5 47 00 00 	add    0x47f5(%rip),%rsi        # 140006088 <the_secs>
   140001893:	48 89 4e 08          	mov    %rcx,0x8(%rsi)
   140001897:	49 89 f1             	mov    %rsi,%r9
   14000189a:	48 89 56 10          	mov    %rdx,0x10(%rsi)
   14000189e:	ff 15 8c 59 00 00    	call   *0x598c(%rip)        # 140007230 <__imp_VirtualProtect>
   1400018a4:	85 c0                	test   %eax,%eax
   1400018a6:	75 b8                	jne    140001860 <mark_section_writable+0x110>
   1400018a8:	ff 15 6a 59 00 00    	call   *0x596a(%rip)        # 140007218 <__IAT_start__>
   1400018ae:	48 8d 0d 33 19 00 00 	lea    0x1933(%rip),%rcx        # 1400031e8 <.rdata+0x78>
   1400018b5:	89 c2                	mov    %eax,%edx
   1400018b7:	e8 34 fe ff ff       	call   1400016f0 <__report_error>
   1400018bc:	0f 1f 40 00          	nopl   0x0(%rax)
   1400018c0:	45 31 c9             	xor    %r9d,%r9d
   1400018c3:	e9 e0 fe ff ff       	jmp    1400017a8 <mark_section_writable+0x58>
   1400018c8:	48 8b 05 b9 47 00 00 	mov    0x47b9(%rip),%rax        # 140006088 <the_secs>
   1400018cf:	8b 53 08             	mov    0x8(%rbx),%edx
   1400018d2:	48 8d 0d d7 18 00 00 	lea    0x18d7(%rip),%rcx        # 1400031b0 <.rdata+0x40>
   1400018d9:	4c 8b 44 30 18       	mov    0x18(%rax,%rsi,1),%r8
   1400018de:	e8 0d fe ff ff       	call   1400016f0 <__report_error>
   1400018e3:	90                   	nop
   1400018e4:	90                   	nop
   1400018e5:	66 66 2e 0f 1f 84 00 	data16 cs nopw 0x0(%rax,%rax,1)
   1400018ec:	00 00 00 00 

00000001400018f0 <_pei386_runtime_relocator>:
   1400018f0:	55                   	push   %rbp
   1400018f1:	41 56                	push   %r14
   1400018f3:	41 55                	push   %r13
   1400018f5:	41 54                	push   %r12
   1400018f7:	57                   	push   %rdi
   1400018f8:	56                   	push   %rsi
   1400018f9:	53                   	push   %rbx
   1400018fa:	48 83 ec 40          	sub    $0x40,%rsp
   1400018fe:	48 8d 6c 24 40       	lea    0x40(%rsp),%rbp
   140001903:	44 8b 25 76 47 00 00 	mov    0x4776(%rip),%r12d        # 140006080 <was_init.0>
   14000190a:	45 85 e4             	test   %r12d,%r12d
   14000190d:	75 19                	jne    140001928 <_pei386_runtime_relocator+0x38>
   14000190f:	4c 8b 35 fa 19 00 00 	mov    0x19fa(%rip),%r14        # 140003310 <.refptr.__ImageBase>
   140001916:	c7 05 60 47 00 00 01 	movl   $0x1,0x4760(%rip)        # 140006080 <was_init.0>
   14000191d:	00 00 00 
   140001920:	66 41 81 3e 4d 5a    	cmpw   $0x5a4d,(%r14)
   140001926:	74 18                	je     140001940 <_pei386_runtime_relocator+0x50>
   140001928:	48 89 ec             	mov    %rbp,%rsp
   14000192b:	5b                   	pop    %rbx
   14000192c:	5e                   	pop    %rsi
   14000192d:	5f                   	pop    %rdi
   14000192e:	41 5c                	pop    %r12
   140001930:	41 5d                	pop    %r13
   140001932:	41 5e                	pop    %r14
   140001934:	5d                   	pop    %rbp
   140001935:	c3                   	ret
   140001936:	66 2e 0f 1f 84 00 00 	cs nopw 0x0(%rax,%rax,1)
   14000193d:	00 00 00 
   140001940:	49 63 46 3c          	movslq 0x3c(%r14),%rax
   140001944:	4c 01 f0             	add    %r14,%rax
   140001947:	81 38 50 45 00 00    	cmpl   $0x4550,(%rax)
   14000194d:	75 d9                	jne    140001928 <_pei386_runtime_relocator+0x38>
   14000194f:	66 81 78 18 0b 02    	cmpw   $0x20b,0x18(%rax)
   140001955:	75 d1                	jne    140001928 <_pei386_runtime_relocator+0x38>
   140001957:	0f b7 40 06          	movzwl 0x6(%rax),%eax
   14000195b:	66 85 c0             	test   %ax,%ax
   14000195e:	74 c8                	je     140001928 <_pei386_runtime_relocator+0x38>
   140001960:	48 8d 04 80          	lea    (%rax,%rax,4),%rax
   140001964:	48 8d 04 c5 0f 00 00 	lea    0xf(,%rax,8),%rax
   14000196b:	00 
   14000196c:	25 f0 ff 7f 00       	and    $0x7ffff0,%eax
   140001971:	e8 7a 03 00 00       	call   140001cf0 <___chkstk_ms>
   140001976:	48 8b 3d a3 19 00 00 	mov    0x19a3(%rip),%rdi        # 140003320 <.refptr.__RUNTIME_PSEUDO_RELOC_LIST_END__>
   14000197d:	48 8b 1d ac 19 00 00 	mov    0x19ac(%rip),%rbx        # 140003330 <.refptr.__RUNTIME_PSEUDO_RELOC_LIST__>
   140001984:	48 29 c4             	sub    %rax,%rsp
   140001987:	c7 05 f3 46 00 00 00 	movl   $0x0,0x46f3(%rip)        # 140006084 <maxSections>
   14000198e:	00 00 00 
   140001991:	48 8d 44 24 30       	lea    0x30(%rsp),%rax
   140001996:	48 89 05 eb 46 00 00 	mov    %rax,0x46eb(%rip)        # 140006088 <the_secs>
   14000199d:	48 89 f8             	mov    %rdi,%rax
   1400019a0:	48 29 d8             	sub    %rbx,%rax
   1400019a3:	48 83 f8 07          	cmp    $0x7,%rax
   1400019a7:	0f 8e 7b ff ff ff    	jle    140001928 <_pei386_runtime_relocator+0x38>
   1400019ad:	48 83 f8 0b          	cmp    $0xb,%rax
   1400019b1:	7e 2d                	jle    1400019e0 <_pei386_runtime_relocator+0xf0>
   1400019b3:	44 8b 13             	mov    (%rbx),%r10d
   1400019b6:	45 85 d2             	test   %r10d,%r10d
   1400019b9:	0f 85 21 01 00 00    	jne    140001ae0 <_pei386_runtime_relocator+0x1f0>
   1400019bf:	44 8b 4b 04          	mov    0x4(%rbx),%r9d
   1400019c3:	45 85 c9             	test   %r9d,%r9d
   1400019c6:	0f 85 14 01 00 00    	jne    140001ae0 <_pei386_runtime_relocator+0x1f0>
   1400019cc:	44 8b 43 08          	mov    0x8(%rbx),%r8d
   1400019d0:	45 85 c0             	test   %r8d,%r8d
   1400019d3:	75 20                	jne    1400019f5 <_pei386_runtime_relocator+0x105>
   1400019d5:	48 83 c3 0c          	add    $0xc,%rbx
   1400019d9:	0f 1f 80 00 00 00 00 	nopl   0x0(%rax)
   1400019e0:	8b 0b                	mov    (%rbx),%ecx
   1400019e2:	85 c9                	test   %ecx,%ecx
   1400019e4:	0f 85 f6 00 00 00    	jne    140001ae0 <_pei386_runtime_relocator+0x1f0>
   1400019ea:	8b 53 04             	mov    0x4(%rbx),%edx
   1400019ed:	85 d2                	test   %edx,%edx
   1400019ef:	0f 85 eb 00 00 00    	jne    140001ae0 <_pei386_runtime_relocator+0x1f0>
   1400019f5:	8b 53 08             	mov    0x8(%rbx),%edx
   1400019f8:	83 fa 01             	cmp    $0x1,%edx
   1400019fb:	0f 85 7d 02 00 00    	jne    140001c7e <_pei386_runtime_relocator+0x38e>
   140001a01:	48 83 c3 0c          	add    $0xc,%rbx
   140001a05:	48 39 fb             	cmp    %rdi,%rbx
   140001a08:	72 76                	jb     140001a80 <_pei386_runtime_relocator+0x190>
   140001a0a:	e9 19 ff ff ff       	jmp    140001928 <_pei386_runtime_relocator+0x38>
   140001a0f:	90                   	nop
   140001a10:	83 fa 08             	cmp    $0x8,%edx
   140001a13:	0f 84 c7 01 00 00    	je     140001be0 <_pei386_runtime_relocator+0x2f0>
   140001a19:	83 fa 10             	cmp    $0x10,%edx
   140001a1c:	0f 85 36 02 00 00    	jne    140001c58 <_pei386_runtime_relocator+0x368>
   140001a22:	0f b7 06             	movzwl (%rsi),%eax
   140001a25:	66 85 c0             	test   %ax,%ax
   140001a28:	0f 89 02 02 00 00    	jns    140001c30 <_pei386_runtime_relocator+0x340>
   140001a2e:	48 0d 00 00 ff ff    	or     $0xffffffffffff0000,%rax
   140001a34:	48 29 c8             	sub    %rcx,%rax
   140001a37:	4c 01 c8             	add    %r9,%rax
   140001a3a:	48 89 45 f8          	mov    %rax,-0x8(%rbp)
   140001a3e:	48 3d 00 80 ff ff    	cmp    $0xffffffffffff8000,%rax
   140001a44:	0f 8c 20 02 00 00    	jl     140001c6a <_pei386_runtime_relocator+0x37a>
   140001a4a:	48 3d ff ff 00 00    	cmp    $0xffff,%rax
   140001a50:	0f 8f 14 02 00 00    	jg     140001c6a <_pei386_runtime_relocator+0x37a>
   140001a56:	48 89 f1             	mov    %rsi,%rcx
   140001a59:	4c 8d 6d f8          	lea    -0x8(%rbp),%r13
   140001a5d:	e8 ee fc ff ff       	call   140001750 <mark_section_writable>
   140001a62:	41 b8 02 00 00 00    	mov    $0x2,%r8d
   140001a68:	4c 89 ea             	mov    %r13,%rdx
   140001a6b:	48 89 f1             	mov    %rsi,%rcx
   140001a6e:	e8 bd 04 00 00       	call   140001f30 <memcpy>
   140001a73:	48 83 c3 0c          	add    $0xc,%rbx
   140001a77:	48 39 fb             	cmp    %rdi,%rbx
   140001a7a:	0f 83 a0 00 00 00    	jae    140001b20 <_pei386_runtime_relocator+0x230>
   140001a80:	8b 0b                	mov    (%rbx),%ecx
   140001a82:	8b 73 04             	mov    0x4(%rbx),%esi
   140001a85:	0f b6 53 08          	movzbl 0x8(%rbx),%edx
   140001a89:	4c 01 f1             	add    %r14,%rcx
   140001a8c:	4c 01 f6             	add    %r14,%rsi
   140001a8f:	4c 8b 09             	mov    (%rcx),%r9
   140001a92:	83 fa 20             	cmp    $0x20,%edx
   140001a95:	0f 84 e5 00 00 00    	je     140001b80 <_pei386_runtime_relocator+0x290>
   140001a9b:	0f 86 6f ff ff ff    	jbe    140001a10 <_pei386_runtime_relocator+0x120>
   140001aa1:	83 fa 40             	cmp    $0x40,%edx
   140001aa4:	0f 85 ae 01 00 00    	jne    140001c58 <_pei386_runtime_relocator+0x368>
   140001aaa:	48 8b 06             	mov    (%rsi),%rax
   140001aad:	4c 8d 6d f8          	lea    -0x8(%rbp),%r13
   140001ab1:	48 29 c8             	sub    %rcx,%rax
   140001ab4:	48 89 f1             	mov    %rsi,%rcx
   140001ab7:	4c 01 c8             	add    %r9,%rax
   140001aba:	48 89 45 f8          	mov    %rax,-0x8(%rbp)
   140001abe:	e8 8d fc ff ff       	call   140001750 <mark_section_writable>
   140001ac3:	41 b8 08 00 00 00    	mov    $0x8,%r8d
   140001ac9:	4c 89 ea             	mov    %r13,%rdx
   140001acc:	48 89 f1             	mov    %rsi,%rcx
   140001acf:	e8 5c 04 00 00       	call   140001f30 <memcpy>
   140001ad4:	eb 9d                	jmp    140001a73 <_pei386_runtime_relocator+0x183>
   140001ad6:	66 2e 0f 1f 84 00 00 	cs nopw 0x0(%rax,%rax,1)
   140001add:	00 00 00 
   140001ae0:	4c 8d 6d f8          	lea    -0x8(%rbp),%r13
   140001ae4:	48 39 fb             	cmp    %rdi,%rbx
   140001ae7:	0f 83 3b fe ff ff    	jae    140001928 <_pei386_runtime_relocator+0x38>
   140001aed:	0f 1f 00             	nopl   (%rax)
   140001af0:	8b 73 04             	mov    0x4(%rbx),%esi
   140001af3:	8b 03                	mov    (%rbx),%eax
   140001af5:	48 83 c3 08          	add    $0x8,%rbx
   140001af9:	41 03 04 36          	add    (%r14,%rsi,1),%eax
   140001afd:	4a 8d 0c 36          	lea    (%rsi,%r14,1),%rcx
   140001b01:	89 45 f8             	mov    %eax,-0x8(%rbp)
   140001b04:	e8 47 fc ff ff       	call   140001750 <mark_section_writable>
   140001b09:	41 b8 04 00 00 00    	mov    $0x4,%r8d
   140001b0f:	4c 89 ea             	mov    %r13,%rdx
   140001b12:	4a 8d 0c 36          	lea    (%rsi,%r14,1),%rcx
   140001b16:	e8 15 04 00 00       	call   140001f30 <memcpy>
   140001b1b:	48 39 fb             	cmp    %rdi,%rbx
   140001b1e:	72 d0                	jb     140001af0 <_pei386_runtime_relocator+0x200>
   140001b20:	8b 05 5e 45 00 00    	mov    0x455e(%rip),%eax        # 140006084 <maxSections>
   140001b26:	85 c0                	test   %eax,%eax
   140001b28:	0f 8e fa fd ff ff    	jle    140001928 <_pei386_runtime_relocator+0x38>
   140001b2e:	48 8b 35 fb 56 00 00 	mov    0x56fb(%rip),%rsi        # 140007230 <__imp_VirtualProtect>
   140001b35:	31 db                	xor    %ebx,%ebx
   140001b37:	66 0f 1f 84 00 00 00 	nopw   0x0(%rax,%rax,1)
   140001b3e:	00 00 
   140001b40:	48 8b 05 41 45 00 00 	mov    0x4541(%rip),%rax        # 140006088 <the_secs>
   140001b47:	48 01 d8             	add    %rbx,%rax
   140001b4a:	44 8b 00             	mov    (%rax),%r8d
   140001b4d:	45 85 c0             	test   %r8d,%r8d
   140001b50:	74 0d                	je     140001b5f <_pei386_runtime_relocator+0x26f>
   140001b52:	48 8b 50 10          	mov    0x10(%rax),%rdx
   140001b56:	48 8b 48 08          	mov    0x8(%rax),%rcx
   140001b5a:	4d 89 e9             	mov    %r13,%r9
   140001b5d:	ff d6                	call   *%rsi
   140001b5f:	41 83 c4 01          	add    $0x1,%r12d
   140001b63:	48 83 c3 28          	add    $0x28,%rbx
   140001b67:	44 3b 25 16 45 00 00 	cmp    0x4516(%rip),%r12d        # 140006084 <maxSections>
   140001b6e:	7c d0                	jl     140001b40 <_pei386_runtime_relocator+0x250>
   140001b70:	48 89 ec             	mov    %rbp,%rsp
   140001b73:	5b                   	pop    %rbx
   140001b74:	5e                   	pop    %rsi
   140001b75:	5f                   	pop    %rdi
   140001b76:	41 5c                	pop    %r12
   140001b78:	41 5d                	pop    %r13
   140001b7a:	41 5e                	pop    %r14
   140001b7c:	5d                   	pop    %rbp
   140001b7d:	c3                   	ret
   140001b7e:	66 90                	xchg   %ax,%ax
   140001b80:	8b 06                	mov    (%rsi),%eax
   140001b82:	85 c0                	test   %eax,%eax
   140001b84:	0f 89 b6 00 00 00    	jns    140001c40 <_pei386_runtime_relocator+0x350>
   140001b8a:	49 ba 00 00 00 00 ff 	movabs $0xffffffff00000000,%r10
   140001b91:	ff ff ff 
   140001b94:	4c 09 d0             	or     %r10,%rax
   140001b97:	48 29 c8             	sub    %rcx,%rax
   140001b9a:	4c 01 c8             	add    %r9,%rax
   140001b9d:	48 89 45 f8          	mov    %rax,-0x8(%rbp)
   140001ba1:	48 3d 00 00 00 80    	cmp    $0xffffffff80000000,%rax
   140001ba7:	0f 8c bd 00 00 00    	jl     140001c6a <_pei386_runtime_relocator+0x37a>
   140001bad:	b9 ff ff ff ff       	mov    $0xffffffff,%ecx
   140001bb2:	48 39 c8             	cmp    %rcx,%rax
   140001bb5:	0f 8f af 00 00 00    	jg     140001c6a <_pei386_runtime_relocator+0x37a>
   140001bbb:	48 89 f1             	mov    %rsi,%rcx
   140001bbe:	4c 8d 6d f8          	lea    -0x8(%rbp),%r13
   140001bc2:	e8 89 fb ff ff       	call   140001750 <mark_section_writable>
   140001bc7:	41 b8 04 00 00 00    	mov    $0x4,%r8d
   140001bcd:	4c 89 ea             	mov    %r13,%rdx
   140001bd0:	48 89 f1             	mov    %rsi,%rcx
   140001bd3:	e8 58 03 00 00       	call   140001f30 <memcpy>
   140001bd8:	e9 96 fe ff ff       	jmp    140001a73 <_pei386_runtime_relocator+0x183>
   140001bdd:	0f 1f 00             	nopl   (%rax)
   140001be0:	0f b6 06             	movzbl (%rsi),%eax
   140001be3:	84 c0                	test   %al,%al
   140001be5:	79 69                	jns    140001c50 <_pei386_runtime_relocator+0x360>
   140001be7:	48 0d 00 ff ff ff    	or     $0xffffffffffffff00,%rax
   140001bed:	48 29 c8             	sub    %rcx,%rax
   140001bf0:	4c 01 c8             	add    %r9,%rax
   140001bf3:	48 89 45 f8          	mov    %rax,-0x8(%rbp)
   140001bf7:	48 83 f8 80          	cmp    $0xffffffffffffff80,%rax
   140001bfb:	7c 6d                	jl     140001c6a <_pei386_runtime_relocator+0x37a>
   140001bfd:	48 3d ff 00 00 00    	cmp    $0xff,%rax
   140001c03:	7f 65                	jg     140001c6a <_pei386_runtime_relocator+0x37a>
   140001c05:	48 89 f1             	mov    %rsi,%rcx
   140001c08:	4c 8d 6d f8          	lea    -0x8(%rbp),%r13
   140001c0c:	e8 3f fb ff ff       	call   140001750 <mark_section_writable>
   140001c11:	41 b8 01 00 00 00    	mov    $0x1,%r8d
   140001c17:	4c 89 ea             	mov    %r13,%rdx
   140001c1a:	48 89 f1             	mov    %rsi,%rcx
   140001c1d:	e8 0e 03 00 00       	call   140001f30 <memcpy>
   140001c22:	e9 4c fe ff ff       	jmp    140001a73 <_pei386_runtime_relocator+0x183>
   140001c27:	66 0f 1f 84 00 00 00 	nopw   0x0(%rax,%rax,1)
   140001c2e:	00 00 
   140001c30:	48 29 c8             	sub    %rcx,%rax
   140001c33:	4c 01 c8             	add    %r9,%rax
   140001c36:	e9 ff fd ff ff       	jmp    140001a3a <_pei386_runtime_relocator+0x14a>
   140001c3b:	0f 1f 44 00 00       	nopl   0x0(%rax,%rax,1)
   140001c40:	48 29 c8             	sub    %rcx,%rax
   140001c43:	4c 01 c8             	add    %r9,%rax
   140001c46:	e9 52 ff ff ff       	jmp    140001b9d <_pei386_runtime_relocator+0x2ad>
   140001c4b:	0f 1f 44 00 00       	nopl   0x0(%rax,%rax,1)
   140001c50:	48 29 c8             	sub    %rcx,%rax
   140001c53:	4c 01 c8             	add    %r9,%rax
   140001c56:	eb 9b                	jmp    140001bf3 <_pei386_runtime_relocator+0x303>
   140001c58:	31 c0                	xor    %eax,%eax
   140001c5a:	48 8d 0d e7 15 00 00 	lea    0x15e7(%rip),%rcx        # 140003248 <.rdata+0xd8>
   140001c61:	48 89 45 f8          	mov    %rax,-0x8(%rbp)
   140001c65:	e8 86 fa ff ff       	call   1400016f0 <__report_error>
   140001c6a:	48 89 44 24 20       	mov    %rax,0x20(%rsp)
   140001c6f:	49 89 f0             	mov    %rsi,%r8
   140001c72:	48 8d 0d ff 15 00 00 	lea    0x15ff(%rip),%rcx        # 140003278 <.rdata+0x108>
   140001c79:	e8 72 fa ff ff       	call   1400016f0 <__report_error>
   140001c7e:	48 8d 0d 8b 15 00 00 	lea    0x158b(%rip),%rcx        # 140003210 <.rdata+0xa0>
   140001c85:	e8 66 fa ff ff       	call   1400016f0 <__report_error>
   140001c8a:	90                   	nop
   140001c8b:	90                   	nop
   140001c8c:	90                   	nop
   140001c8d:	90                   	nop
   140001c8e:	90                   	nop
   140001c8f:	90                   	nop

0000000140001c90 <__mingw_raise_matherr>:
   140001c90:	48 83 ec 58          	sub    $0x58,%rsp
   140001c94:	48 8b 05 f5 43 00 00 	mov    0x43f5(%rip),%rax        # 140006090 <stUserMathErr>
   140001c9b:	48 85 c0             	test   %rax,%rax
   140001c9e:	74 29                	je     140001cc9 <__mingw_raise_matherr+0x39>
   140001ca0:	f2 0f 10 84 24 80 00 	movsd  0x80(%rsp),%xmm0
   140001ca7:	00 00 
   140001ca9:	66 0f 14 d3          	unpcklpd %xmm3,%xmm2
   140001cad:	89 4c 24 20          	mov    %ecx,0x20(%rsp)
   140001cb1:	48 8d 4c 24 20       	lea    0x20(%rsp),%rcx
   140001cb6:	48 89 54 24 28       	mov    %rdx,0x28(%rsp)
   140001cbb:	0f 11 54 24 30       	movups %xmm2,0x30(%rsp)
   140001cc0:	f2 0f 11 44 24 40    	movsd  %xmm0,0x40(%rsp)
   140001cc6:	ff d0                	call   *%rax
   140001cc8:	90                   	nop
   140001cc9:	48 83 c4 58          	add    $0x58,%rsp
   140001ccd:	c3                   	ret
   140001cce:	66 90                	xchg   %ax,%ax

0000000140001cd0 <__mingw_setusermatherr>:
   140001cd0:	48 89 0d b9 43 00 00 	mov    %rcx,0x43b9(%rip)        # 140006090 <stUserMathErr>
   140001cd7:	e9 64 02 00 00       	jmp    140001f40 <__setusermatherr>
   140001cdc:	90                   	nop
   140001cdd:	90                   	nop
   140001cde:	90                   	nop
   140001cdf:	90                   	nop

0000000140001ce0 <_fpreset>:
   140001ce0:	db e3                	fninit
   140001ce2:	c3                   	ret
   140001ce3:	90                   	nop
   140001ce4:	90                   	nop
   140001ce5:	90                   	nop
   140001ce6:	90                   	nop
   140001ce7:	90                   	nop
   140001ce8:	90                   	nop
   140001ce9:	90                   	nop
   140001cea:	90                   	nop
   140001ceb:	90                   	nop
   140001cec:	90                   	nop
   140001ced:	90                   	nop
   140001cee:	90                   	nop
   140001cef:	90                   	nop

0000000140001cf0 <___chkstk_ms>:
   140001cf0:	51                   	push   %rcx
   140001cf1:	50                   	push   %rax
   140001cf2:	48 3d 00 10 00 00    	cmp    $0x1000,%rax
   140001cf8:	48 8d 4c 24 18       	lea    0x18(%rsp),%rcx
   140001cfd:	72 19                	jb     140001d18 <___chkstk_ms+0x28>
   140001cff:	48 81 e9 00 10 00 00 	sub    $0x1000,%rcx
   140001d06:	48 83 09 00          	orq    $0x0,(%rcx)
   140001d0a:	48 2d 00 10 00 00    	sub    $0x1000,%rax
   140001d10:	48 3d 00 10 00 00    	cmp    $0x1000,%rax
   140001d16:	77 e7                	ja     140001cff <___chkstk_ms+0xf>
   140001d18:	48 29 c1             	sub    %rax,%rcx
   140001d1b:	48 83 09 00          	orq    $0x0,(%rcx)
   140001d1f:	58                   	pop    %rax
   140001d20:	59                   	pop    %rcx
   140001d21:	c3                   	ret
   140001d22:	90                   	nop
   140001d23:	90                   	nop
   140001d24:	90                   	nop
   140001d25:	90                   	nop
   140001d26:	90                   	nop
   140001d27:	90                   	nop
   140001d28:	90                   	nop
   140001d29:	90                   	nop
   140001d2a:	90                   	nop
   140001d2b:	90                   	nop
   140001d2c:	90                   	nop
   140001d2d:	90                   	nop
   140001d2e:	90                   	nop
   140001d2f:	90                   	nop

0000000140001d30 <vfprintf>:
   140001d30:	57                   	push   %rdi
   140001d31:	56                   	push   %rsi
   140001d32:	53                   	push   %rbx
   140001d33:	48 83 ec 30          	sub    $0x30,%rsp
   140001d37:	48 89 cb             	mov    %rcx,%rbx
   140001d3a:	48 89 d6             	mov    %rdx,%rsi
   140001d3d:	4c 89 c7             	mov    %r8,%rdi
   140001d40:	e8 7b 00 00 00       	call   140001dc0 <__local_stdio_printf_options>
   140001d45:	45 31 c9             	xor    %r9d,%r9d
   140001d48:	49 89 f0             	mov    %rsi,%r8
   140001d4b:	48 89 da             	mov    %rbx,%rdx
   140001d4e:	48 8b 08             	mov    (%rax),%rcx
   140001d51:	48 89 7c 24 20       	mov    %rdi,0x20(%rsp)
   140001d56:	e8 3d 01 00 00       	call   140001e98 <__stdio_common_vfprintf>
   140001d5b:	48 83 c4 30          	add    $0x30,%rsp
   140001d5f:	5b                   	pop    %rbx
   140001d60:	5e                   	pop    %rsi
   140001d61:	5f                   	pop    %rdi
   140001d62:	c3                   	ret
   140001d63:	90                   	nop
   140001d64:	90                   	nop
   140001d65:	90                   	nop
   140001d66:	90                   	nop
   140001d67:	90                   	nop
   140001d68:	90                   	nop
   140001d69:	90                   	nop
   140001d6a:	90                   	nop
   140001d6b:	90                   	nop
   140001d6c:	90                   	nop
   140001d6d:	90                   	nop
   140001d6e:	90                   	nop
   140001d6f:	90                   	nop

0000000140001d70 <fprintf>:
   140001d70:	57                   	push   %rdi
   140001d71:	56                   	push   %rsi
   140001d72:	53                   	push   %rbx
   140001d73:	48 83 ec 40          	sub    $0x40,%rsp
   140001d77:	48 8d 7c 24 70       	lea    0x70(%rsp),%rdi
   140001d7c:	48 89 cb             	mov    %rcx,%rbx
   140001d7f:	48 89 d6             	mov    %rdx,%rsi
   140001d82:	4c 89 44 24 70       	mov    %r8,0x70(%rsp)
   140001d87:	4c 89 4c 24 78       	mov    %r9,0x78(%rsp)
   140001d8c:	48 89 7c 24 38       	mov    %rdi,0x38(%rsp)
   140001d91:	e8 2a 00 00 00       	call   140001dc0 <__local_stdio_printf_options>
   140001d96:	45 31 c9             	xor    %r9d,%r9d
   140001d99:	49 89 f0             	mov    %rsi,%r8
   140001d9c:	48 89 da             	mov    %rbx,%rdx
   140001d9f:	48 8b 08             	mov    (%rax),%rcx
   140001da2:	48 89 7c 24 20       	mov    %rdi,0x20(%rsp)
   140001da7:	e8 ec 00 00 00       	call   140001e98 <__stdio_common_vfprintf>
   140001dac:	48 83 c4 40          	add    $0x40,%rsp
   140001db0:	5b                   	pop    %rbx
   140001db1:	5e                   	pop    %rsi
   140001db2:	5f                   	pop    %rdi
   140001db3:	c3                   	ret
   140001db4:	90                   	nop
   140001db5:	90                   	nop
   140001db6:	90                   	nop
   140001db7:	90                   	nop
   140001db8:	90                   	nop
   140001db9:	90                   	nop
   140001dba:	90                   	nop
   140001dbb:	90                   	nop
   140001dbc:	90                   	nop
   140001dbd:	90                   	nop
   140001dbe:	90                   	nop
   140001dbf:	90                   	nop

0000000140001dc0 <__local_stdio_printf_options>:
   140001dc0:	48 8d 05 79 02 00 00 	lea    0x279(%rip),%rax        # 140002040 <options>
   140001dc7:	c3                   	ret
   140001dc8:	90                   	nop
   140001dc9:	90                   	nop
   140001dca:	90                   	nop
   140001dcb:	90                   	nop
   140001dcc:	90                   	nop
   140001dcd:	90                   	nop
   140001dce:	90                   	nop
   140001dcf:	90                   	nop

0000000140001dd0 <__p___initenv>:
   140001dd0:	48 8b 05 79 15 00 00 	mov    0x1579(%rip),%rax        # 140003350 <.refptr.__imp___initenv>
   140001dd7:	48 8b 00             	mov    (%rax),%rax
   140001dda:	c3                   	ret
   140001ddb:	90                   	nop
   140001ddc:	90                   	nop
   140001ddd:	90                   	nop
   140001dde:	90                   	nop
   140001ddf:	90                   	nop

0000000140001de0 <_amsg_exit>:
   140001de0:	53                   	push   %rbx
   140001de1:	48 83 ec 20          	sub    $0x20,%rsp
   140001de5:	89 cb                	mov    %ecx,%ebx
   140001de7:	b9 02 00 00 00       	mov    $0x2,%ecx
   140001dec:	e8 8f 00 00 00       	call   140001e80 <__acrt_iob_func>
   140001df1:	41 89 d8             	mov    %ebx,%r8d
   140001df4:	48 8d 15 d5 14 00 00 	lea    0x14d5(%rip),%rdx        # 1400032d0 <.rdata>
   140001dfb:	48 89 c1             	mov    %rax,%rcx
   140001dfe:	e8 6d ff ff ff       	call   140001d70 <fprintf>
   140001e03:	b9 ff 00 00 00       	mov    $0xff,%ecx
   140001e08:	e8 db 00 00 00       	call   140001ee8 <_exit>
   140001e0d:	90                   	nop
   140001e0e:	90                   	nop
   140001e0f:	90                   	nop

0000000140001e10 <__getmainargs>:
   140001e10:	55                   	push   %rbp
   140001e11:	57                   	push   %rdi
   140001e12:	56                   	push   %rsi
   140001e13:	53                   	push   %rbx
   140001e14:	48 83 ec 28          	sub    $0x28,%rsp
   140001e18:	44 89 cd             	mov    %r9d,%ebp
   140001e1b:	48 89 d6             	mov    %rdx,%rsi
   140001e1e:	4c 89 c3             	mov    %r8,%rbx
   140001e21:	48 89 cf             	mov    %rcx,%rdi
   140001e24:	e8 c7 00 00 00       	call   140001ef0 <_initialize_narrow_environment>
   140001e29:	83 fd 01             	cmp    $0x1,%ebp
   140001e2c:	b9 01 00 00 00       	mov    $0x1,%ecx
   140001e31:	83 d9 ff             	sbb    $0xffffffff,%ecx
   140001e34:	e8 9f 00 00 00       	call   140001ed8 <_configure_narrow_argv>
   140001e39:	e8 82 00 00 00       	call   140001ec0 <__p___argc>
   140001e3e:	8b 00                	mov    (%rax),%eax
   140001e40:	89 07                	mov    %eax,(%rdi)
   140001e42:	e8 81 00 00 00       	call   140001ec8 <__p___argv>
   140001e47:	48 8b 00             	mov    (%rax),%rax
   140001e4a:	48 89 06             	mov    %rax,(%rsi)
   140001e4d:	e8 1e 01 00 00       	call   140001f70 <__p__environ>
   140001e52:	48 8b 00             	mov    (%rax),%rax
   140001e55:	48 89 03             	mov    %rax,(%rbx)
   140001e58:	48 8b 44 24 70       	mov    0x70(%rsp),%rax
   140001e5d:	8b 08                	mov    (%rax),%ecx
   140001e5f:	e8 fc 00 00 00       	call   140001f60 <_set_new_mode>
   140001e64:	31 c0                	xor    %eax,%eax
   140001e66:	48 83 c4 28          	add    $0x28,%rsp
   140001e6a:	5b                   	pop    %rbx
   140001e6b:	5e                   	pop    %rsi
   140001e6c:	5f                   	pop    %rdi
   140001e6d:	5d                   	pop    %rbp
   140001e6e:	c3                   	ret
   140001e6f:	90                   	nop

0000000140001e70 <strlen>:
   140001e70:	ff 25 e2 54 00 00    	jmp    *0x54e2(%rip)        # 140007358 <__imp_strlen>
   140001e76:	90                   	nop
   140001e77:	90                   	nop
   140001e78:	0f 1f 84 00 00 00 00 	nopl   0x0(%rax,%rax,1)
   140001e7f:	00 

0000000140001e80 <__acrt_iob_func>:
   140001e80:	ff 25 92 54 00 00    	jmp    *0x5492(%rip)        # 140007318 <__imp___acrt_iob_func>
   140001e86:	90                   	nop
   140001e87:	90                   	nop

0000000140001e88 <__p__commode>:
   140001e88:	ff 25 92 54 00 00    	jmp    *0x5492(%rip)        # 140007320 <__imp___p__commode>
   140001e8e:	90                   	nop
   140001e8f:	90                   	nop

0000000140001e90 <__p__fmode>:
   140001e90:	ff 25 92 54 00 00    	jmp    *0x5492(%rip)        # 140007328 <__imp___p__fmode>
   140001e96:	90                   	nop
   140001e97:	90                   	nop

0000000140001e98 <__stdio_common_vfprintf>:
   140001e98:	ff 25 92 54 00 00    	jmp    *0x5492(%rip)        # 140007330 <__imp___stdio_common_vfprintf>
   140001e9e:	90                   	nop
   140001e9f:	90                   	nop

0000000140001ea0 <fflush>:
   140001ea0:	ff 25 92 54 00 00    	jmp    *0x5492(%rip)        # 140007338 <__imp_fflush>
   140001ea6:	90                   	nop
   140001ea7:	90                   	nop

0000000140001ea8 <puts>:
   140001ea8:	ff 25 92 54 00 00    	jmp    *0x5492(%rip)        # 140007340 <__imp_puts>
   140001eae:	90                   	nop
   140001eaf:	90                   	nop

0000000140001eb0 <setvbuf>:
   140001eb0:	ff 25 92 54 00 00    	jmp    *0x5492(%rip)        # 140007348 <__imp_setvbuf>
   140001eb6:	90                   	nop
   140001eb7:	90                   	nop
   140001eb8:	0f 1f 84 00 00 00 00 	nopl   0x0(%rax,%rax,1)
   140001ebf:	00 

0000000140001ec0 <__p___argc>:
   140001ec0:	ff 25 da 53 00 00    	jmp    *0x53da(%rip)        # 1400072a0 <__imp___p___argc>
   140001ec6:	90                   	nop
   140001ec7:	90                   	nop

0000000140001ec8 <__p___argv>:
   140001ec8:	ff 25 da 53 00 00    	jmp    *0x53da(%rip)        # 1400072a8 <__imp___p___argv>
   140001ece:	90                   	nop
   140001ecf:	90                   	nop

0000000140001ed0 <_cexit>:
   140001ed0:	ff 25 da 53 00 00    	jmp    *0x53da(%rip)        # 1400072b0 <__imp__cexit>
   140001ed6:	90                   	nop
   140001ed7:	90                   	nop

0000000140001ed8 <_configure_narrow_argv>:
   140001ed8:	ff 25 da 53 00 00    	jmp    *0x53da(%rip)        # 1400072b8 <__imp__configure_narrow_argv>
   140001ede:	90                   	nop
   140001edf:	90                   	nop

0000000140001ee0 <_crt_atexit>:
   140001ee0:	ff 25 da 53 00 00    	jmp    *0x53da(%rip)        # 1400072c0 <__imp__crt_atexit>
   140001ee6:	90                   	nop
   140001ee7:	90                   	nop

0000000140001ee8 <_exit>:
   140001ee8:	ff 25 da 53 00 00    	jmp    *0x53da(%rip)        # 1400072c8 <__imp__exit>
   140001eee:	90                   	nop
   140001eef:	90                   	nop

0000000140001ef0 <_initialize_narrow_environment>:
   140001ef0:	ff 25 da 53 00 00    	jmp    *0x53da(%rip)        # 1400072d0 <__imp__initialize_narrow_environment>
   140001ef6:	90                   	nop
   140001ef7:	90                   	nop

0000000140001ef8 <_XcptFilter>:
   140001ef8:	ff 25 da 53 00 00    	jmp    *0x53da(%rip)        # 1400072d8 <__imp__XcptFilter>
   140001efe:	90                   	nop
   140001eff:	90                   	nop

0000000140001f00 <_initterm>:
   140001f00:	ff 25 da 53 00 00    	jmp    *0x53da(%rip)        # 1400072e0 <__imp__initterm>
   140001f06:	90                   	nop
   140001f07:	90                   	nop

0000000140001f08 <_initterm_e>:
   140001f08:	ff 25 da 53 00 00    	jmp    *0x53da(%rip)        # 1400072e8 <__imp__initterm_e>
   140001f0e:	90                   	nop
   140001f0f:	90                   	nop

0000000140001f10 <__set_app_type>:
   140001f10:	ff 25 da 53 00 00    	jmp    *0x53da(%rip)        # 1400072f0 <__imp___set_app_type>
   140001f16:	90                   	nop
   140001f17:	90                   	nop

0000000140001f18 <_set_invalid_parameter_handler>:
   140001f18:	ff 25 da 53 00 00    	jmp    *0x53da(%rip)        # 1400072f8 <__imp__set_invalid_parameter_handler>
   140001f1e:	90                   	nop
   140001f1f:	90                   	nop

0000000140001f20 <abort>:
   140001f20:	ff 25 da 53 00 00    	jmp    *0x53da(%rip)        # 140007300 <__imp_abort>
   140001f26:	90                   	nop
   140001f27:	90                   	nop

0000000140001f28 <exit>:
   140001f28:	ff 25 da 53 00 00    	jmp    *0x53da(%rip)        # 140007308 <__imp_exit>
   140001f2e:	90                   	nop
   140001f2f:	90                   	nop

0000000140001f30 <memcpy>:
   140001f30:	ff 25 5a 53 00 00    	jmp    *0x535a(%rip)        # 140007290 <__imp_memcpy>
   140001f36:	90                   	nop
   140001f37:	90                   	nop
   140001f38:	0f 1f 84 00 00 00 00 	nopl   0x0(%rax,%rax,1)
   140001f3f:	00 

0000000140001f40 <__setusermatherr>:
   140001f40:	ff 25 3a 53 00 00    	jmp    *0x533a(%rip)        # 140007280 <__imp___setusermatherr>
   140001f46:	90                   	nop
   140001f47:	90                   	nop
   140001f48:	0f 1f 84 00 00 00 00 	nopl   0x0(%rax,%rax,1)
   140001f4f:	00 

0000000140001f50 <_configthreadlocale>:
   140001f50:	ff 25 1a 53 00 00    	jmp    *0x531a(%rip)        # 140007270 <__imp__configthreadlocale>
   140001f56:	90                   	nop
   140001f57:	90                   	nop
   140001f58:	0f 1f 84 00 00 00 00 	nopl   0x0(%rax,%rax,1)
   140001f5f:	00 

0000000140001f60 <_set_new_mode>:
   140001f60:	ff 25 f2 52 00 00    	jmp    *0x52f2(%rip)        # 140007258 <__imp__set_new_mode>
   140001f66:	90                   	nop
   140001f67:	90                   	nop

0000000140001f68 <malloc>:
   140001f68:	ff 25 f2 52 00 00    	jmp    *0x52f2(%rip)        # 140007260 <__imp_malloc>
   140001f6e:	90                   	nop
   140001f6f:	90                   	nop

0000000140001f70 <__p__environ>:
   140001f70:	ff 25 d2 52 00 00    	jmp    *0x52d2(%rip)        # 140007248 <__imp___p__environ>
   140001f76:	90                   	nop
   140001f77:	90                   	nop
   140001f78:	0f 1f 84 00 00 00 00 	nopl   0x0(%rax,%rax,1)
   140001f7f:	00 

0000000140001f80 <VirtualQuery>:
   140001f80:	ff 25 b2 52 00 00    	jmp    *0x52b2(%rip)        # 140007238 <__imp_VirtualQuery>
   140001f86:	90                   	nop
   140001f87:	90                   	nop

0000000140001f88 <VirtualProtect>:
   140001f88:	ff 25 a2 52 00 00    	jmp    *0x52a2(%rip)        # 140007230 <__imp_VirtualProtect>
   140001f8e:	90                   	nop
   140001f8f:	90                   	nop

0000000140001f90 <Sleep>:
   140001f90:	ff 25 92 52 00 00    	jmp    *0x5292(%rip)        # 140007228 <__imp_Sleep>
   140001f96:	90                   	nop
   140001f97:	90                   	nop

0000000140001f98 <SetUnhandledExceptionFilter>:
   140001f98:	ff 25 82 52 00 00    	jmp    *0x5282(%rip)        # 140007220 <__imp_SetUnhandledExceptionFilter>
   140001f9e:	90                   	nop
   140001f9f:	90                   	nop

0000000140001fa0 <GetLastError>:
   140001fa0:	ff 25 72 52 00 00    	jmp    *0x5272(%rip)        # 140007218 <__IAT_start__>
   140001fa6:	90                   	nop
   140001fa7:	90                   	nop
   140001fa8:	0f 1f 84 00 00 00 00 	nopl   0x0(%rax,%rax,1)
   140001faf:	00 

0000000140001fb0 <register_frame_ctor>:
   140001fb0:	e9 0b f5 ff ff       	jmp    1400014c0 <__gcc_register_frame>
   140001fb5:	90                   	nop
   140001fb6:	90                   	nop
   140001fb7:	90                   	nop
   140001fb8:	90                   	nop
   140001fb9:	90                   	nop
   140001fba:	90                   	nop
   140001fbb:	90                   	nop
   140001fbc:	90                   	nop
   140001fbd:	90                   	nop
   140001fbe:	90                   	nop
   140001fbf:	90                   	nop
