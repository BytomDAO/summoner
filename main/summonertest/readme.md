# LockWithPublicKey
## 合约语法
```
var publicKey pubkey = "e9108d3ca8049800727f6a3505b3a2710dc579405dde03c250f16d9a7e1e6e78"

func unlockWithSig(signnature sig) {
	verify(check_tx_sig(publicKey, signnature))
}
```
## 指令码
```
  0 FROMALTSTACK 
   1 DUP 
   2 SWAP 
   3 TOALTSTACK 
   4 OVER 
   5 FROMALTSTACK 
   6 DUP 
   7 SWAP 
   8 TOALTSTACK 
   9 TXSIGHASH 
  10 SWAP 
  11 CHECKSIG 
  12 FROMALTSTACK 
  13 DUP 
  14 SWAP 
  15 TOALTSTACK 
  16 4 
  17 PICK 
  18 FROMALTSTACK 
  19 DUP 
  20 SWAP 
  21 TOALTSTACK 
  22 TXSIGHASH 
  23 SWAP 
  24 CHECKSIG 
  25 VERIFY 
  26 DROP
```
### 字节码
```
6c767c6b786c767c6bae7cac6c767c6b54796c767c6bae7cac6975
```
# LockToOutput
## 合约语法
```
var amtGlobal amount = 123

func LockToOutput() {
	var amt amount = 1
	var ast asset = "ffffffff"
	var p hex = "1234"
	lock(amtGlobal, ast, "")
}
```
## 指令码
```
 0 1 
   1 DATA_8 
   2 ffffffff
  10 DATA_4 
  11 1234
  15 FALSE 
  16 FROMALTSTACK 
  17 DUP 
  18 SWAP 
  19 TOALTSTACK 
  20 3 
  21 PICK 
  22 1 
  23 PROGRAM 
  24 FROMALTSTACK 
  25 DUP 
  26 SWAP 
  27 TOALTSTACK 
  28 CHECKOUTPUT 
  29 VERIFY 
  30 DROP 
```
### 字节码
```
510866666666666666660434333231006c767c6b537951c46c767c6bc16975
```

# FuncIfLocal
## 合约语法
```
func IfLocal(b int64, c int64) {
	var a int64 = 12345678
	if b > c {
		a = b
	} else {
		a = c
	}
	a = a + 6
}
```

## 指令码
```
0 DATA_4 
   1 12345678
   5 2 
   6 PICK 
   7 2 
   8 PICK 
   9 GREATERTHAN 
  10 NOT 
  11 NOP 
  12 JUMPIF 
  13 24
  17 DROP 
  18 DUP 
  19 JUMP 
  20 24
  24 DROP 
  25 OVER 
  26 DUP 
  27 6 
  28 ADD 
```
### 字节码
```
044e61bc0052795279a091616400000018757663000000187578765693
```

# globalParaAdd
## 合约语法
```
var a int64 = 12345678

func globalParaAdd(b int64) {
	a = a + b
}
```

## 指令码
```
 0 FROMALTSTACK 
   1 DUP 
   2 SWAP 
   3 TOALTSTACK 
   4 OVER 
   5 ADD 
```
### 字节码
```
6c767c6b7893
```

# nftBuy
## 合约语法
```
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
```
## 指令码
```
 0 10 
   1 DATA_28 
   2 706c6174666f726d536372697074
  30 4 
  31 PICK 
  32 DATA_4 
  33 100
  37 DIV 
  38 OVER 
  39 FROMALTSTACK 
  40 FROMALTSTACK 
  41 FROMALTSTACK 
  42 FROMALTSTACK 
  43 FROMALTSTACK 
  44 DUP 
  45 SWAP 
  46 TOALTSTACK 
  47 SWAP 
  48 TOALTSTACK 
  49 SWAP 
  50 TOALTSTACK 
  51 SWAP 
  52 TOALTSTACK 
  53 SWAP 
  54 TOALTSTACK 
  55 MUL 
  56 DATA_4 
  57 100
  61 DIV 
  62 3 
  63 PICK 
  64 5 
  65 PICK 
  66 SUB 
  67 2 
  68 PICK 
  69 SUB 
  70 FROMALTSTACK 
  71 DUP 
  72 SWAP 
  73 TOALTSTACK 
  74 15 
  75 PICK 
  76 MUL 
  77 LESSTHAN 
  78 3 
  79 PICK 
  80 FROMALTSTACK 
  81 DUP 
  82 SWAP 
  83 TOALTSTACK 
  84 3 
  85 PICK 
  86 MUL 
  87 LESSTHAN 
  88 VERIFY 
  89 DROP 
  90 FALSE 
  91 14 
  92 PICK 
  93 FROMALTSTACK 
  94 FROMALTSTACK 
  95 DUP 
  96 SWAP 
  97 TOALTSTACK 
  98 SWAP 
  99 TOALTSTACK 
 100 1 
 101 DATA_4 
 102 27
 106 PICK 
 107 2 
 108 PICK 
 109 CHECKOUTPUT 
 110 VERIFY 
 111 DROP 
 112 FALSE 
 113 DATA_4 
 114 21
 118 PICK 
 119 FROMALTSTACK 
 120 FROMALTSTACK 
 121 DUP 
 122 SWAP 
 123 TOALTSTACK 
 124 SWAP 
 125 TOALTSTACK 
 126 1 
 127 FROMALTSTACK 
 128 FROMALTSTACK 
 129 FROMALTSTACK 
 130 FROMALTSTACK 
 131 FROMALTSTACK 
 132 FROMALTSTACK 
 133 DUP 
 134 SWAP 
 135 TOALTSTACK 
 136 SWAP 
 137 TOALTSTACK 
 138 SWAP 
 139 TOALTSTACK 
 140 SWAP 
 141 TOALTSTACK 
 142 SWAP 
 143 TOALTSTACK 
 144 SWAP 
 145 TOALTSTACK 
 146 2 
 147 PICK 
 148 CHECKOUTPUT 
 149 VERIFY 
 150 DROP 
 151 FALSE 
 152 DATA_4 
 153 24
 157 PICK 
 158 FROMALTSTACK 
 159 FROMALTSTACK 
 160 DUP 
 161 SWAP 
 162 TOALTSTACK 
 163 SWAP 
 164 TOALTSTACK 
 165 1 
 166 FROMALTSTACK 
 167 FROMALTSTACK 
 168 FROMALTSTACK 
 169 DUP 
 170 SWAP 
 171 TOALTSTACK 
 172 SWAP 
 173 TOALTSTACK 
 174 SWAP 
 175 TOALTSTACK 
 176 2 
 177 PICK 
 178 CHECKOUTPUT 
 179 VERIFY 
 180 DROP 
 181 DATA_4 
 182 56
 186 ROLL 
 187 DROP 
 188 DATA_4 
 189 58
 193 PICK 
 194 DATA_4 
 195 59
 199 ROLL 
 200 DROP 
 201 DATA_4 
 202 59
 206 PICK 
 207 FALSE 
 208 1 
 209 FROMALTSTACK 
 210 FROMALTSTACK 
 211 FROMALTSTACK 
 212 FROMALTSTACK 
 213 DUP 
 214 SWAP 
 215 TOALTSTACK 
 216 SWAP 
 217 TOALTSTACK 
 218 SWAP 
 219 TOALTSTACK 
 220 SWAP 
 221 TOALTSTACK 
 222 1 
 223 PROGRAM 
 224 1 
 225 CHECKOUTPUT 
 226 VERIFY 
 227 DROP 
 228 FALSE 
 229 FROMALTSTACK 
 230 DUP 
 231 SWAP 
 232 TOALTSTACK 
 233 FROMALTSTACK 
 234 FROMALTSTACK 
 235 DUP 
 236 SWAP 
 237 TOALTSTACK 
 238 SWAP 
 239 TOALTSTACK 
 240 1 
 241 PROGRAM 
 242 FROMALTSTACK 
 243 DUP 
 244 SWAP 
 245 TOALTSTACK 
 246 CHECKOUTPUT 
 247 VERIFY 
 248 DROP 
 ```
### 字节码
```
5a1c373036633631373436363666373236643533363337323639373037345479046400000096786c6c6c6c6c767c6b7c6b7c6b7c6b7c6b9504640000009653795579945279946c767c6b5f79959f53796c767c6b5379959f6975005e796c6c767c6b7c6b51041b000000795279c16975000415000000796c6c767c6b7c6b516c6c6c6c6c6c767c6b7c6b7c6b7c6b7c6b7c6b5279c16975000418000000796c6c767c6b7c6b516c6c6c767c6b7c6b7c6b5279c1697504380000007a75043a00000079043b0000007a75043b0000007900516c6c6c6c767c6b7c6b7c6b7c6b51c451c16975006c767c6b6c6c767c6b7c6b51c46c767c6bc16975
```


