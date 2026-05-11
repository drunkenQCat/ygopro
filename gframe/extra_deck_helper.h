#ifndef EXTRA_DECK_HELPER_H
#define EXTRA_DECK_HELPER_H

#include "deck.h"
#include <cstdint>

namespace ygo {

class ExtraDeckHelper {
public:
	// 预处理卡组数据：将 side 中的额外卡移动到 extra 部分
	// 参数：
	//   dbuf[] - 卡组数据数组
	//   mainc - 输入：主卡+额外卡数量，输出：处理后的主卡数量
	//   sidec - 输入：side卡数量，输出：处理后的side数量
	static void PreProcessSideCards(uint32_t dbuf[], int* mainc, int* sidec);
};

} // namespace ygo

#endif // EXTRA_DECK_HELPER_H
