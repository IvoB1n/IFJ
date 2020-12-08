//compiler 7?
//
package main

func alpha () (int) {
	return 1
}

func main () {
	a := 0
	b := 0
	a, b = 4 , alpha()
}