// stateData:  [creater, taxRate, nftAsset, owner, marginAsset, marginAmount]
//var (
// []byte("createrScript")
var CreaterScript hex = "63726561746572536372697074"

// reverse(uint256.NewInt(10).Bytes())
var TaxRate int64 = 12345678

// testutil.MustDecodeAsset("a0a71c215764e342d10d003be6369baf4145d9c7977f7b8f6bf446e628d8b8b8")
var NftAsset asset = "a0a71c215764e342d10d003be6369baf4145d9c7977f7b8f6bf446e628d8b8b8"

// []byte("ownerScirpt")
var OwnerScirpt hex = "6f776e6572536369727074"

var MarginAsset asset = "a0a71c215764e342d10d003be6369baf4145d9c7977f7b8f6bf446e628d8b8b8"

// vm.Uint64Bytes(200000000)
var MarginAmount amount = 200000000

//)

// alt stack	[creater, taxRate, nftAsset, owner, marginAsset, marginAmount]
// data statck	[newMarginAsset, newMarginAmount, selecter]
//func editMargin(marginAsset asset, marginAmount amount) {
//	MarginAmount = marginAmount
//	MarginAsset = marginAsset
//	lock(1, NftAsset, "")
//	lock(MarginAmount, MarginAsset, "")
//}

// alt stack	[creater, taxRate, nftAsset, owner, marginAsset, marginAmount]
// data statck  [buyer, payAmount, marginAmount, selecter]
func buy(buyer hex, payAmount amount, marginAmount amount) {
	var MarginFold int64 = 10
	// []byte("platformScript")
	var PlatformScript hex = "706c6174666f726d536372697074"

	platformFee := int64(payAmount) / 100
	createrTax := int64(payAmount) * TaxRate / 100
	ownerGot := int64(payAmount) - platformFee - createrTax
	verify(ownerGot < int64(MarginAmount)*MarginFold)
	lock(amount(platformFee), MarginAsset, PlatformScript)
	lock(amount(createrTax), MarginAsset, CreaterScript)
	lock(amount(ownerGot), MarginAsset, OwnerScirpt)

	OwnerScirpt = buyer
	MarginAmount = marginAmount
	lock(1, NftAsset, "")
	lock(MarginAmount, MarginAsset, "")
}