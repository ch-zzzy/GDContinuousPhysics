#include <Geode/Geode.hpp>
#include <Geode/modify/CCEGLView.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include <chizz.continuous-physics-api/include/ContinuousPhysics.hpp>

using namespace geode::prelude;
using namespace continuousphysics::prelude;

auto mod = Mod::get();

class $modify(CCEGLView) {
	void pollEvents() {
		PlayLayer* playLayer = PlayLayer::get();
		CCNode* parent;

		// clang-format off
		if (!GetFocus() || !playLayer
			|| !(parent = playLayer->getParent())
			|| parent->getChildByType<PauseLayer>(0)
			|| playLayer->getChildByType<EndLevelLayer>(0)
			|| playLayer->m_playerDied)
		{
			g_physicsState.m_firstFrame = true;
		}
		// clang-format on

		CCEGLView::pollEvents();
	}
};

class $modify(PlayerObject) {
	void update(float dt) {
		if (useVanillaTick(this)) {
			PlayerObject::update(dt);
			return;
		}
		preTick(this);
		PlayerObject::update(dt);
		postTick(this);
	}
};

$on_mod(Loaded) {
	// TPS
	float tps = mod->getSettingValue<float>("tps");
	updateTPS(tps);
	listenForSettingChanges<float>("tps", +[](float val) { updateTPS(val); });

	// Input Hz
	g_inputHz = mod->getSettingValue<float>("input-hz");
	listenForSettingChanges<float>(
		"input-hz", +[](float val) { g_inputHz = val; });

	// Velocity unrounding
	g_velocityUnroundingEnabled =
		mod->getSettingValue<bool>("velocity-unrounding");
	toggleVelocityUnroundingPatches(g_velocityUnroundingEnabled);
	listenForSettingChanges<bool>(
		"velocity-unrounding", +[](bool val) {
			g_velocityUnroundingEnabled = val;
			toggleVelocityUnroundingPatches(val);
		});

	// Subframes
	g_subframesEnabled = mod->getSettingValue<bool>("subframes-enabled");
	listenForSettingChanges<bool>(
		"subframes-enabled", +[](bool val) {
			g_subframesEnabled = val;
			updateTPS(mod->getSettingValue<float>("tps"));
		});

	// Mod active
	g_modActive = !mod->getSettingValue<bool>("mod-disabled");
	listenForSettingChanges<bool>(
		"mod-disabled", +[](bool val) { g_modActive = !val; });
}
