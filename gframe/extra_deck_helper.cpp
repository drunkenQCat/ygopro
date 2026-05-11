#include "extra_deck_helper.h"
#include "data_manager.h"

namespace ygo {

void ExtraDeckHelper::PreProcessSideCards(uint32_t dbuf[], int* mainc, int* sidec) {
	if (!mainc || !sidec || *sidec <= 0)
		return;

	auto& _datas = dataManager.GetDataTable();
	int original_mainc = *mainc;
	int original_sidec = *sidec;

	uint32_t extra_cards[15];
	uint32_t non_extra_cards[15];
	int extra_count = 0;
	int non_extra_count = 0;

	// 收集 side 中的卡，分成额外卡和非额外卡
	for (int i = 0; i < original_sidec; i++) {
		auto code = dbuf[original_mainc + i];
		auto it = _datas.find(code);
		if (it != _datas.end()) {
			auto& cd = it->second;
			if (cd.type & TYPES_EXTRA_DECK) {
				extra_cards[extra_count++] = code;
			} else {
				non_extra_cards[non_extra_count++] = code;
			}
		}
	}

	// 将额外卡移动到 main+extra 部分的末尾
	for (int i = 0; i < extra_count; i++) {
		dbuf[original_mainc + i] = extra_cards[i];
	}

	// 将非额外卡放回 dbuf 的 side 部分（从 extra_cards 之后开始）
	for (int i = 0; i < non_extra_count; i++) {
		dbuf[original_mainc + extra_count + i] = non_extra_cards[i];
	}

	// 更新计数：mainc 增加（因为额外卡现在算在 main+extra 部分了）
	*mainc += extra_count;
	// sidec 只剩下非额外卡
	*sidec = non_extra_count;
}

} // namespace ygo
