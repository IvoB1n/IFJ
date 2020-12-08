// 3 consider extension 0
package main

func main () {
	// i1:= 5
	// i2:= 4
	// res_int := i1 + i2 
	// print("res_int= ", res_int, "\n")
	a := true
	b := false
	print("a(", a,")\n","b(", b,")\n\n")
	
	res_and := a && b
	res_or := a || b
	print("a(", a,") && ","b(", b,")= ", res_and, "\n")
	print("a(", a,") || ","b(", b,")= ", res_or, "\n\n")
	
	res_advanced1 := (a && b) || a // true
	res_advanced2 := a && (b || a) // true
	print("(a(", a,") && ","b(", b,")) || ","a(", a,")= ", res_advanced1, "\n")
	print("a(", a,") && ","(b(", b,")  || ","a(", a,"))= ", res_advanced2, "\n\n")
	
	res_advanced3 := (b && a) || b // false
	res_advanced4 := b && (a || b) // false
	print("(b(", b,") && ","a(", a,")) || ","b(", b,")= ", res_advanced3, "\n")
	print("b(", b,") && ","(a(", a,")  || ","b(", b,"))= ", res_advanced4, "\n\n")
	
	not_a := !a
	not_b := !b
	print("!a(", a,")= ", not_a, "\n","!b(", b,")= ", not_b, "\n\n")
	
	not_a_and_b:= !(a && b)
	not_a_or_b:= !(a || b)
	print("!(a(", a,") && ","b(", b,"))= ", not_a_and_b, "\n")
	print("!(a(", a,") || ","b(", b,"))= ", not_a_or_b, "\n\n")

	res_a_eq_b := a == b
	res_a_neq_b := a != b
	print("a(", a,") == ","b(", b,")= ", res_a_eq_b, "\n")
	print("a(", a,") != ","b(", b,")= ", res_a_neq_b, "\n")

	res_ass:= (a==b) && (a != b)
	// print (res_ass, "\n")	
}
