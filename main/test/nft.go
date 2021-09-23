// const: fold/script/assetID
// contract Params: [fold, script]  passing in cli
const (
	MarginFold = 10

	// []byte("platformScript")
	PlatformScript hex = "706c6174666f726d536372697074"

	// ownerScirpt = []byte("ownerScirpt")
	OwnerScirpt hex = "63726561746572536372697074"

	// testutil.MustDecodeAsset("bda946b3110fa46fd94346ce3f05f0760f1b9de72e238835bc4d19f9d64f1742")
	BTC asset = "bda946b3110fa46fd94346ce3f05f0760f1b9de72e238835bc4d19f9d64f1742"

	// testutil.MustDecodeAsset("78de44ffa1bce37b757c9eae8925b5f199dc4621b412ef0f3f46168865284a93")
	ETH asset = "78de44ffa1bce37b757c9eae8925b5f199dc4621b412ef0f3f46168865284a93"
)

// stateData:  [creater, taxRate, nftAsset, owner, marginAsset, marginAmount]
var (
	// []byte("createrScript")
	CreaterScript hex = "63726561746572536372697074"

	// reverse(uint256.NewInt(10).Bytes())
	TaxRate int64 = 10

	// testutil.MustDecodeAsset("a0a71c215764e342d10d003be6369baf4145d9c7977f7b8f6bf446e628d8b8b8")
	NftAsset asset = "a0a71c215764e342d10d003be6369baf4145d9c7977f7b8f6bf446e628d8b8b8"

	// []byte("ownerScirpt")
	OwnerScirpt hex = "6f776e6572536369727074"

	MarginAsset asset

	// vm.Uint64Bytes(200000000)
	MarginAmount amount = 200000000
)

// alt stack	[creater, taxRate, nftAsset, owner, marginAsset, marginAmount]
// data statck	[newMarginAsset, newMarginAmount, selecter]
func editMargin(marginAsset asset, marginAmount amount) {
	MarginAmount = marginAmount
	MarginAsset = marginAsset
	lock(1, NftAsset, "", "")
	lock(MarginAmount, MarginAsset, "", "")
}

// alt stack	[creater, taxRate, nftAsset, owner, marginAsset, marginAmount]
// data statck  [buyer, payAmount, marginAmount, selecter]
func buy(buyer hex, payAmount int64, marginAmount int64) {
	platformFee := payAmount / 100
	createrTax := payAmount * TaxRate / 100
	ownerGot := payAmount - platformFee - createrTax
	verify(ownerGot < MarginAmount*MarginFold)

	lock(platformFee, MarginAsset, PlatformScript, "")
	lock(createrTax, MarginAsset, CreaterScript, "")
	lock(ownerGot, MarginAsset, ownerScirpt, "")

	Owner = buyer
	MarginAmount = marginAmount
	lock(1, NftAsset, "", "")
	lock(MarginAmount, MarginAsset, "", "")
}
