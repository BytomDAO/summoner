func IfLocal(b int64, c int64) {
	var a int64 = 12345678
	if b > c {
		a = b
	} else {
		a = c
	}
	a = a + 6
}
