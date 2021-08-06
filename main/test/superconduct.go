// 三个资产类型全局常量，可以先用全局变量实现
// asset资产类型，32个字节长度的资产
var (
	assetX = asset(1) // int类型强转
	assetY = asset(2)
	assetM = asset(3)
)

// struct结构描述合约结构：field为三资产池状态数据，function为存取兑解锁函数
// 暂通过全局函数实现

// 三资产池状态数据[poolAmtX,poolAmtY,poolAmtM]构造交易时传入，存入辅栈

// 全局函数实现存取兑

// 构造交易时传入参数:兑换资产idassetX, 数量amtX, 选择swap兑换功能，checkoutput缠绕合约的program
//     如：[][]byte{assetX.Bytes(),amtX,swap,program}
// 状态数据传入参数，三资产池资产数量state [poolAmtX,poolAmtY,poolAmtM]
// 注意这里传参summoner的处理过程
//     如：[][]byte{vm.Int64Bytes(10), vm.Int64Bytes(100), vm.Int64Bytes(1000)}
func Swap(amtX amount) { // 资产数量amount无符号整型，可为uint64类型别名
	verify(amtX > 0) // 暂builtin实现
	verify(poolAmtX*poolAmtY == poolAmtM)

	newPoolAmtX := poolAmtX + amtX // 暂不计算手续费
	newPoolAmtY := poolAmtM / newPoolAmtX

	lock(newPoolAmtX, assetX, program, state) // 注：state经过summoner取算存的过程
	lock(newPoolAmtY, assetY, program, state)
	lock(poolAmtM, assetM, program, state)
}

// TOOD
func AddLiquidity(amtX amount, amtY amount) {
}

// TOOD
func RemoveLiquidity(amtX amount, amtY amount) {
}


