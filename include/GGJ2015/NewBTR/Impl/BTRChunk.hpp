#ifndef GGJ2015_NEWBTR_IMPL_BTRCHUNK
#define GGJ2015_NEWBTR_IMPL_BTRCHUNK

#include "../../../GGJ2015/Common.hpp"

namespace Exp
{
	namespace Impl
	{
		class BTRChunk
		{
			template<typename> friend struct Impl::EffectHelper;
			friend class BTRRoot;

			private:
				BTRRoot& root;
				BTRChunk* parent{nullptr};
				std::string str;
				std::vector<BTREffect*> childrenEffects;
				std::vector<BTRChunk*> children;
				float trackingModifier{0.f}, leadingModifier{0.f}, hChunkSpacingModifier{0.f};
				SizeT idxHierarchyBegin, idxHierarchyEnd;
				bool enabled{true};

				template<typename... TArgs> BTRChunk& mkChild(TArgs&&... mArgs);
				template<typename T, typename... TArgs> T& mkEffect(TArgs&&... mArgs);

				void refreshGeometry() noexcept;
				void refreshEffects() noexcept;

				template<bool TSelf = false, typename TF> inline void recurseParents(const TF& mFn) const
				{
					if(TSelf) mFn(*this);
					if(parent != nullptr && parent->enabled) parent->recurseParents<true>(mFn);
				}

				template<bool TSelf = false, typename TF> inline void recurseChildren(const TF& mFn)
				{
					if(TSelf) mFn(*this);
					for(auto& c : children) if(c->enabled) c->recurseChildren<true>(mFn);
				}

				inline void update(FT mFT) noexcept
				{
					for(auto& e : childrenEffects) e->update(mFT);
					for(auto& c : children) if(c->enabled) c->update(mFT);
				}

			public:
				inline BTRChunk(BTRRoot& mRoot) noexcept : root{mRoot} { }

				inline auto getTracking() const noexcept
				{
					float result{0.f};
					recurseParents([&result](const auto& mP){ result += mP.trackingModifier; });
					return result;
				}
				inline auto getLeading() const noexcept
				{
					float result{0.f};
					recurseParents([&result](const auto& mP){ result += mP.leadingModifier; });
					return result;
				}
				inline auto getHChunkSpacing() const noexcept
				{
					float result{0.f};
					recurseParents([&result](const auto& mP){ result += mP.hChunkSpacingModifier; });
					return result;
				}

				template<typename TF> void forVertices(TF mFn) noexcept;

				template<typename T> void setStr(T&& mX);
				inline const auto& getStr() const noexcept { return str; }

				inline void setEnabled(bool mX) noexcept { enabled = mX; }
				inline bool isEnabled() const noexcept { return enabled; }

				// Make children and go one level deeper
				auto& in();
				auto& in(BTRChunk*&);
				template<typename T> auto& in(T&& mStr);
				template<typename T> auto& in(BTRChunk*&, T&& mStr);

				// Make children and stay on the same level
				auto& mk(BTRChunk*&);
				template<typename T> auto& mk(T&& mStr);
				template<typename T> auto& mk(BTRChunk*&, T&& mStr);

				// Add effects
				auto& eff(const sf::Color&);
				template<typename T, typename... TArgs> auto& eff(TArgs&&...);
				template<typename T, typename... TArgs> auto& eff(T*&, TArgs&&...);

				// Go back one level
				inline auto& out() const noexcept { SSVU_ASSERT(parent != nullptr); return *parent; }
		};

		using BTRChunkRecVector = ssvu::MonoRecVector<BTRChunk>;
		using BTREffectRecVector = ssvu::PolyRecVector<BTREffect>;
	}
}

#endif
