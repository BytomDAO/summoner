var publicKey pubkey = "e9108d3ca8049800727f6a3505b3a2710dc579405dde03c250f16d9a7e1e6e78"

func unlockWithSig(signnature sig) {
	verify(check_tx_sig(publicKey, signnature))
}
