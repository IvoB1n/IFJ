//compiler 0
//
package main

func main () {

	if (5 > 6) {
		print("if\n")
	} else if (5 < 6) {
		print("else if\n")  //---
		if (20 < 7) {
			print("20 > 7\n")
		} else if (8>7) {
			if (5 > 6) {
				print("if\n")
			} else if (5 < 6) {
				print("else if2\n") //---
				if (20 < 7) {
					print("20 < 7\n")
				} else if (6>7) {
					print("else if3\n")
				} else {
					print("20 < 7\n") //---
					if (59 > 8) {
						print("positive\n") //---
					}
				}
			}
		} else if (5==5) {
			print("else if1 lvl1\n")
		} else {
			print("20 == 7\n")
			if (59 > 8) {
				print("positive2\n")
			}
		}
	} else {
		print("else\n")
	}
	
	if (14 > 100) {
		print("146%\n")
	} else {
		print("No body\n")
	}

	if (5 > 6) {
		print("if\n")
	} else if (5 < 6) {
		print("else if\n")  //---
		if (20 < 7) {
			print("20 > 7\n")
		} else if (6>7) {
			if (5 > 6) {
				print("if\n")
			} else if (5 < 6) {
				print("else if2\n") //---
				if (20 < 7) {
					print("20 < 7\n")
				} else if (6>7) {
					print("else if3\n")
				} else {
					print("20 < 7\n") //---
					if (59 > 8) {
						print("positive\n") //---
					}
				}
			}
		} else {
			print("20 == 7\n")
			if (59 > 8) {
				print("positive2\n")
			}
		}
	}
}
