#include "mixed_resolution_demo.h"

#include "brand_engine/display.h"
#include "brand_engine/internal/display_atari_ste_backend.h"
#include "brand_engine/internal/display_band_atari_ste_backend.h"
#include "brand_engine/internal/raster_atari_ste_backend.h"
#include "brand_engine/internal/runtime_atari_ste_backend.h"
#include "brand_engine/machine.h"
#include "brand_engine/runtime.h"
#include "brand_engine/targets/atari_ste.h"

int main(void) {
  static const BE_RuntimeIdentity runtimeIdentity = {
    .id = "brand-game-demo-mixed-resolution",
    .displayName = "Brand 2 Mixed Resolution Demo",
    .kind = BE_RUNTIME_TARGET_APPLICATION,
    .artifactKind = BE_ARTIFACT_TARGET_EXECUTABLE
  };
  static const BE_BackendIdentity backendIdentity = {
    .id = "atari-ste",
    .displayName = "Atari STe"
  };
  const BE_RuntimeBinding binding = {
    .machine = BE_machineAtariSteProfile(),
    .runtime = &runtimeIdentity,
    .backend = &backendIdentity
  };
  const BE_RuntimeConfig runtimeConfig = {
    .title = "Brand 2 Mixed Resolution Demo",
    .frameSync = BE_RUNTIME_FRAME_SYNC_VBLANK,
    .targetFramesPerSecond = 0
  };
  const BE_DisplayMode displayMode = { 640, 200 };
  int result = 1;

  if (BE_atariSteDisplaySelectMode(
          BE_ATARI_STE_DISPLAY_MODE_MEDIUM_RASTER) !=
          BE_ATARI_STE_RESULT_OK ||
      BE_runtimeAtariSteInstall() != BE_RUNTIME_RESULT_OK ||
      BE_runtimeInitialize(&binding, &runtimeConfig) !=
          BE_RUNTIME_RESULT_OK) {
    return 1;
  }
  if (BE_displayAtariSteInstall() != BE_DISPLAY_RESULT_OK ||
      BE_displaySetMode(&displayMode) != BE_DISPLAY_RESULT_OK ||
      BE_displayReset() != BE_DISPLAY_RESULT_OK ||
      BE_displayBandAtariSteInstall() != BE_DISPLAY_BAND_RESULT_OK ||
      BE_rasterAtariSteInstall() != BE_RASTER_RESULT_OK) {
    goto shutdownRuntime;
  }

  result = MixedResolutionDemo_run();

shutdownRuntime:
  BE_runtimeShutdown();
  return result;
}
