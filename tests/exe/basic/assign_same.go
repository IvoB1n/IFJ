// potom no ne fakt
package main

func alpha () (int,  float64, int, float64) {
	return 1,  4.0,2, 8.0
}

func beta (param float64) (float64, string, float64) {
	param = param + 3.7
	return param, "help" ,2.9
}

func main () {
	a := 0
	b := 5.0
	a,b, a,  b = alpha()
	print(a, "\n", b, "\n")

	f := 0.0
	f, _, f = beta(f)
	print(f, "\n")
}
