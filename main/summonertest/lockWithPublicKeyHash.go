var publicKeyHash hex

func spend(publicKey hex, signnature sig) {
	verify(sha3(publicKey) == publicKeyHash)
	verify(check_tx_sig(publicKey, signnature))
}