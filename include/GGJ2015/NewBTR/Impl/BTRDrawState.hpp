#ifndef GGJ2015_NEWBTR_IMPL_BTRDRAWSTATE
#define GGJ2015_NEWBTR_IMPL_BTRDRAWSTATE

#include <SSVUtils/MemoryManager/MemoryManager.hpp>
#include "SSVStart/Global/Typedefs.hpp"
#include "SSVStart/BitmapText/Impl/BitmapFont.hpp"

#include "../../NewBTR/Impl/BTREffect.hpp"
#include "../../NewBTR/Impl/BTRChunk.hpp"
#include "../../NewBTR/Impl/BTREWave.hpp"
#include "../../NewBTR/Impl/BTREColor.hpp"

namespace ssvs
{
	namespace BTR
	{
		namespace Impl
		{
			struct BTRDrawState
			{
				struct RowData
				{
					float width;
					SizeT cells;

					inline RowData(float mWidth, SizeT mCells) noexcept : width{mWidth}, cells{mCells} { }
				};

				std::vector<RowData> rDatas;
				float xMin, xMax, yMin, yMax, nextHChunkSpacing;
				SizeT width, height, iX;
				int nl, htab, vtab;

				inline void reset(const BitmapFont& mBF) noexcept
				{
					rDatas.clear();

					xMin = xMax = yMin = yMax = nextHChunkSpacing = 0.f;

					width = mBF.getCellWidth();
					height = mBF.getCellHeight();
					iX = 0;

					nl = htab = vtab = 0;
				}
			};
		}
	}
}

#endif
