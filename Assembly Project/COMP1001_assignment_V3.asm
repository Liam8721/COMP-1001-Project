; A simple template for assembly programs.
.386  ; Specify instruction set
.model flat, stdcall  ; Flat memory model, std. calling convention
.stack 4096 ; Reserve stack space
ExitProcess PROTO, dwExitCode: DWORD  ; Exit process prototype

.data ; data segment
	array_B db 10 dup(?)  ; define array called B of 10 bytes. all elements initialised to 0. size of array is 10
	array_A BYTE 3,2,3,1,7,5,0,8,9,2 ; define array A with 10 the values given in C code in question
	array_C BYTE 1,3,2,5,4,6,0,4,5,8 ; define array C with 10 the values given in C code in question
	term_1 BYTE 0 ; this is the variable that the value of first term in the formula is stored in (A[i]*3+1)
	term_2 BYTE 0 ; this is the variable that the value of the second term in the formula is stored in(C[i]*2+3)
	term_3 BYTE 0 ; this is the varibale that the value of the third term in the formula is stored in (A[i]+C[i])/3
	array_C_address DWORD ? ; this is the variable that stores the location of the arrayC
	array_A_address DWORD ? ; this is the variable that stores the location of the arrayA
	three BYTE 3 ; this stores the number 3 as a varibale so multipication and division of 3 can be perfromed
	C_multiply BYTE 2 ; this stores the number 2 as a variable in order to multiply the each element of the arrayC by 2

.code ; code segment

main PROC ; main procedure
	; write your assembly code here
	mov esi, 0 ; this is the variable i representing the count controlled loop in the C code 
	for_loop:                         
		cmp esi, 10 ; this acts as our conidtional counter that asseses if i is greater or equal to 10
		jge end_for_loop ; if the the esi (variable i) reaches 10 then this command is executed. this jumps to the end_for_loop which ends the program

		; creation of first term in formula
		lea ebx, array_A ; this loads the addresss of arrayA into a register from memory
		mov array_A_address, ebx ; this moves the address of arrayA into a variable so it can be accessed more efficiently in the third term of the formula
		mov al, [ebx + esi] ; this moves the value of the current element of the arrayA into al. al is used as it is 16 bits and this minimises space complexity as if you used AL or AH then it would be too small and overflow would occur 
		mul three ; this multiplies the number stored in al by 3 as said in the first term of the formula
		add al, 1 ; this adds one to the number in al as said in the first term of the formula
		mov term_1, al ; this stores the result of the first term in a variable so it can be easily accesssed later in the program

		; creation of second term in formula
		lea ebx, array_C ; this loads the addresss of arrayC into a register from memory
		mov array_C_address, ebx ; this moves the address of arrayC into a variable so it can be accessed more efficiently in the third term of the formula
		mov al, [ebx + esi]	; this moves the value of the current element of the arrayC into al. al is used as it is 16 bits and this minimises space complexity as if you used AL or AH then it would be too small and overflow would occur 
		shl al, 1 ; this multiplies the number stored in al by 2 as said in the second term of the formula
		add al, 3 ; this adds 3 to the number in al as said in the second term of the formula
		mov term_2, al ; this stores the result of the second term in a variable so it can be easily accesssed later in the program

		; creation of third term in formula
		mov ebx, array_C_address ; this moves the location of arrayC into ebx 
		mov al, [ebx + esi] ; this moves the value of the current element of the arrayC into al. 
		mov ebx, array_A_address ; this overwrites the data in ebx and moves the location of arrayA into ebx
		mov cl, [ebx + esi] ; this moves the value of current element of the arrayA into cl. 
		add al, cl ; this adds the values of the current elements from arrayA and arrayC together and stores the values in register AL
		mov ah, 0 ; this moves 0 into ah to remove garbage values
		div three ; this divides the value in AL by 3
		mov term_3, al ;this stores the 3rd term as a variable to be easily accessed later
		
		; creation of array B
		lea ebx, array_B ; this moves the location of arrayB into ebx
		mov eax, 0 ; this sets eax to 0 to remove any garbage values
		mov al, term_1 ; this moves term1 into al
		add al, term_2 ; this adds term2 to al
		mov dl, 0 ; this makes dl 0 to remove any garbage values
		mov dl, term_3 ; this moves the value of term3 into the dl register
		add al, dl ; this adds the value in dl to al and stores the result in al
		mov [ebx + esi], eax ; this overwrites the value of the element matched by index of esi to the value of eax

		add esi, 1 ;increments esi (i) by 1
		jmp	for_loop	; if i greater or equal to 10, exit the loop ; this jumps to the start of the loop
									 
	end_for_loop:
		INVOKE ExitProcess, 0 ; call exit function	
	
  
main ENDP ; exit main procedure
END main  ; stop assembling