//input brainfuck_A.in
//input brainfuck_B.in
//input brainfuck_C.in
// go run tests/exe/complex/brainfuck.go  tests/exe/complex/ifj20.go
package main

func Instruction(instr string, pc int, ptr int, memory string, program string, progLen int) (int,int,int,string) {
	err := 0

	print("instr = ", instr, "  pc= ", pc, "  ptr= ", ptr, "  memory= ", memory, "\n") //"  program= ", program, "  progLen= ", progLen, "\n")

	

	if instr == "+" {
		memory,err = Change(1, memory, ptr)
		print("memory = ", memory, "\n" )
		return err , pc , ptr , memory
	} else {
	}

	if instr == "-" {
		memory,err = Change(0, memory, ptr)
		return err , pc , ptr , memory
	} else {
	}

	if instr == ">" {
		ptr = Move(1, ptr)
		return 0 , pc , ptr , memory
	} else {
	}

	if instr == "<" {
		ptr = Move(0, ptr)
		return 0 , pc , ptr , memory
	} else {
	}

	if instr == "." {
		Output(memory, ptr)
		return 0 , pc , ptr , memory
	} else {
	}

	if instr == "," {
		memory,err = Input(memory, ptr)
		return err , pc , ptr , memory
	} else {
	}

	if instr == "[" {
		pc,err = CycleBegin(memory, ptr, program, pc, progLen)
		return err , pc , ptr , memory
	} else {
	}

	if instr == "]" {
		pc,err = CycleEnd(memory, ptr, program, pc)
		return err , pc , ptr , memory
	} else {
	}

	print("Unsupported instruction\n")
	return 1 , pc , ptr , memory
}

func Move (right int, ptr int) (int) {
	if right == 1 {
		ptr = ptr + 1
	} else {
		ptr = ptr - 1
	}
	return ptr
}

func Change (increment int, memory string, ptr int) (string, int) {
	print("\n0 -  memory= ", memory, "   ptr= ", ptr, "\n")

	first := ""
	first , _ = substr(memory, 0, ptr)
	//print("\n1 -  memory= ", memory, "   ptr= ", ptr, "\n")

	lastBegin := ptr + 1
	lastLen := 0
	lastLen = len(memory)
	print("lastLen = ", lastLen, "\n")
	//print("2 -  memory= ", memory, "   ptr= ", ptr, "\n")

	lastLen = lastLen - ptr + 1
	print("mem = ", memory, "lastbegin = ", lastBegin, "lastLen = ", lastLen, "\n")
	last := ""
	last,_ = substr(memory, lastBegin, lastLen)
	//print("3 -  memory= ", memory, "   ptr= ", ptr, "\n")
	char := 0
	char,_ = ord(memory, ptr)
	print("char= ", char, "  memory= ", memory, "   ptr= ", ptr, "\n")
	if increment==1 {
		char = char + 1
	} else {
		char = char - 1
	}
	print("char= ", char, "  memory= ", memory, "   ptr= ", ptr, "\n")
	err := 0
	result := ""
	result,err = chr(char)
	if err==1 {
		print("Memory value out of range\n")
		return memory,1
	} else {
		//print("first= ", first, "  result= ", result, "   last= ", last, "\n")
		return first + result + last , 0	//  result, 0
	}
}

func Input (memory string, ptr int) (string,int) {
	first := ""
	first,_ = substr(memory, 0, ptr)
	lastBegin := ptr + 1
	lastLen := 0
	lastLen = len(memory)
	lastLen = lastLen - ptr + 1
	last := ""
	last,_ = substr(memory, lastBegin, lastLen)
	char := 0
	err := 0
	char,err = inputi()
	if err!=0 {
		print("Input error\n")
		return memory,1
	} else {
		result := ""
		result,err = chr(char)
		if err!=0 {
			print("Input value is too large\n")
			return memory,1
		} else {
			return first + result + last , 0
		}
	}
}

func Output (memory string, ptr int) {
	char := ""
	char,_ = substr(memory, ptr, 1)
	print(char)
}

func CycleBegin (memory string, ptr int, program string, pc int, progLen int) (int,int) {
	current := 0
	current,_ = ord(memory, ptr)
	if (current == 0) {
		counter := pc
		deep := 0
		for ;0==0; {
			counter = counter + 1
			if counter >= progLen {
				print("Incomplete cycle (begin)\n")
				return pc , 1
			} else {
			}
			instr := ""
			instr,_ = substr(program, counter, 1)
			if instr == "]" {
				if deep == 0 {
					return counter , 0
				} else {
					deep = deep - 1
				}
			} else {
			}
			if instr == "[" {
				deep = deep + 1
			} else {
			}
		}
	} else {
	}
	return pc , 0
}

func CycleEnd (memory string, ptr int, program string, pc int) (int,int) {
	current := 0
	current,_ = ord(memory, ptr)
	if (current != 0) {
		counter := pc
		deep := 0
		for ;1!=0; {
			counter = counter - 1
			if counter < 0 {
				print("Incomplete cycle (end)\n")
				return pc , 1
			} else {
			}
			instr := ""
			instr,_ = substr(program, counter, 1)
			if instr == "]" {
				deep = deep - 1
			} else {
			}
			if instr == "[" {
				if deep == 0 {
					return counter-1 , 0
				} else {
				}
				deep = deep + 1
			} else {
			}
		}
	} else {
	}
	return pc , 0
}

func LoadProgram () (string,int) {
	err := 0
	program := ""
	progLen := 0

	program,err = inputs()
	print("load_program= ", program, "  err= ", err, "\n")
	if err == 1 {
		print("Error loading program\n")
		return "", 0
	} else {
		progLen = len(program)
		print("proglen = ", progLen, "\n")
		return program, progLen
	}
}

func InitMemory () (string) {
	char := ""
	char,_ = chr(0)
	memory := ""
	for counter := 0 ; (counter < 256) ; counter = counter + 1 {
		memory = memory + char
	}
	s_len:= len(memory)
	print("s_len= ", s_len, "\n")
	return memory
}

func SanityCheck(ptr int, pc int, progLen int) (int) {
	ptrErrorMsg := "Value of 'ptr' is out of range\n"
	pcErrorMsg := "Value of 'pc' is out of range\n"

	if (ptr < 0) {
		print(ptrErrorMsg)
		return 1
	} else {
	}

	if (pc < 0) {
		print(pcErrorMsg)
		return 1
	} else {
	}

	if (ptr > 256) {
		print(ptrErrorMsg)
		return 1
	} else {
	}

	if (pc >= progLen) {
		print("Program completed\n")
		return 1
	} else {
	}

	return 0
}

func main () {
	pc := 0
	ptr := 0
	memory := ""
	program := ""
	progLen := 0

	memory = InitMemory()
	program , progLen = LoadProgram()
	print("loaded_program= ", program, "  proglen= " , progLen, "\n")
	if (progLen == 0) {
		return
	} else {
		for ;pc < progLen; {
			instr := ""
			result := 0
			instr,_ = substr(program, pc, 1)
			print("instr= ", instr, "\n")
			result,pc,ptr,memory = Instruction(instr,pc,ptr,memory,program,progLen)
			//print("result = ", result, "  pc= ", pc, "  ptr= ", ptr, "  memory= ", memory, "\n")
			if (result == 1) {
				return
			} else {
			}
			pc = pc + 1
			result = SanityCheck(ptr, pc, progLen)
			if (result == 1) {
				return
			} else {
			}
		}
	}
}
