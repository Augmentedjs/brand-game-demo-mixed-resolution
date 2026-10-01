#include "mixed_resolution_demo.h"

#include "brand_engine/display_band.h"
#include "brand_engine/raster.h"
#include "brand_engine/runtime.h"

enum {
  TOP_BAND_LINES = 32,
  BODY_BAND_LINES = 136,
  BOTTOM_BAND_LINES = 32,
  LORES_COLUMNS = 40,
  HIRES_COLUMNS = 80,
  CHROME_ROWS = 4,
  BODY_ROWS = 17
};

static const BE_PaletteColor topChromePalette[16] = {
  BE_PALETTE_COLOR_RGB4(1, 0, 0),
  BE_PALETTE_COLOR_RGB4(6, 1, 0),
  BE_PALETTE_COLOR_RGB4(11, 4, 0),
  BE_PALETTE_COLOR_RGB4(15, 12, 2),
  BE_PALETTE_COLOR_RGB4(1, 1, 2),
  BE_PALETTE_COLOR_RGB4(3, 3, 5),
  BE_PALETTE_COLOR_RGB4(5, 5, 8),
  BE_PALETTE_COLOR_RGB4(9, 9, 12),
  BE_PALETTE_COLOR_RGB4(2, 1, 0),
  BE_PALETTE_COLOR_RGB4(6, 3, 0),
  BE_PALETTE_COLOR_RGB4(11, 7, 1),
  BE_PALETTE_COLOR_RGB4(15, 13, 6),
  BE_PALETTE_COLOR_RGB4(5, 1, 2),
  BE_PALETTE_COLOR_RGB4(10, 2, 3),
  BE_PALETTE_COLOR_RGB4(14, 5, 4),
  BE_PALETTE_COLOR_RGB4(15, 15, 15)
};

static const BE_PaletteColor bottomChromePalette[16] = {
  BE_PALETTE_COLOR_RGB4(0, 1, 0),
  BE_PALETTE_COLOR_RGB4(0, 5, 2),
  BE_PALETTE_COLOR_RGB4(2, 11, 5),
  BE_PALETTE_COLOR_RGB4(9, 15, 10),
  BE_PALETTE_COLOR_RGB4(1, 1, 2),
  BE_PALETTE_COLOR_RGB4(3, 3, 5),
  BE_PALETTE_COLOR_RGB4(5, 5, 8),
  BE_PALETTE_COLOR_RGB4(9, 9, 12),
  BE_PALETTE_COLOR_RGB4(2, 1, 0),
  BE_PALETTE_COLOR_RGB4(6, 3, 0),
  BE_PALETTE_COLOR_RGB4(11, 7, 1),
  BE_PALETTE_COLOR_RGB4(15, 13, 6),
  BE_PALETTE_COLOR_RGB4(5, 1, 2),
  BE_PALETTE_COLOR_RGB4(10, 2, 3),
  BE_PALETTE_COLOR_RGB4(14, 5, 4),
  BE_PALETTE_COLOR_RGB4(15, 15, 15)
};

static const BE_PaletteColor terminalPalette[4] = {
  BE_PALETTE_COLOR_RGB4(0, 0, 1),
  BE_PALETTE_COLOR_RGB4(1, 4, 5),
  BE_PALETTE_COLOR_RGB4(3, 10, 11),
  BE_PALETTE_COLOR_RGB4(10, 15, 15)
};

static void makePaletteBarTile(
    BE_U8 *tile,
    const BE_U16 tileX,
    const BE_U16 logicalWidth,
    const BE_U8 colorCount) {
  BE_U16 y;

  for (y = 0; y < 8; y++) {
    BE_U16 x;

    for (x = 0; x < 8; x += 2) {
      const BE_U16 leftX = (BE_U16)(tileX * 8u + x);
      const BE_U16 rightX = (BE_U16)(leftX + 1u);
      const BE_U8 left = (BE_U8)(((BE_U32)leftX * colorCount) / logicalWidth);
      const BE_U8 right = (BE_U8)(((BE_U32)rightX * colorCount) / logicalWidth);

      tile[y * 4u + x / 2u] = (BE_U8)((left << 4) | right);
    }
  }
}

static BE_Bool drawPaletteBars(
    const BE_U16 band,
    const BE_U16 columns,
    const BE_U16 firstRow,
    const BE_U16 rowCount,
    const BE_U8 colorCount) {
  const BE_U16 logicalWidth = (BE_U16)(columns * 8u);
  BE_U8 tile[32];
  BE_U16 row;

  for (row = 0; row < rowCount; row++) {
    BE_U16 x;

    for (x = 0; x < columns; x++) {
      makePaletteBarTile(tile, x, logicalWidth, colorCount);
      if (BE_displayBandWriteTile(
              band,
              x,
              (BE_U16)(firstRow + row),
              tile) != BE_DISPLAY_BAND_RESULT_OK) {
        return BE_FALSE;
      }
    }
  }
  return BE_TRUE;
}

static void makeResolutionProofTile(
    BE_U8 *tile,
    const BE_U16 tileX,
    const BE_U16 tileY,
    const BE_U8 darkColor,
    const BE_U8 brightColor) {
  BE_U16 y;

  for (y = 0; y < 8; y++) {
    BE_U16 x;

    for (x = 0; x < 8; x += 2) {
      const BE_U16 pixelX = (BE_U16)(tileX * 8u + x);
      const BE_U16 pixelY = (BE_U16)(tileY * 8u + y);
      const BE_U8 left = ((pixelX + pixelY) & 1u) != 0u
          ? brightColor
          : darkColor;
      const BE_U8 right = ((pixelX + pixelY + 1u) & 1u) != 0u
          ? brightColor
          : darkColor;

      tile[y * 4u + x / 2u] = (BE_U8)((left << 4) | right);
    }
  }
}

static BE_Bool drawResolutionProof(
    const BE_U16 band,
    const BE_U16 columns,
    const BE_U16 firstRow,
    const BE_U16 rowCount,
    const BE_U8 darkColor,
    const BE_U8 brightColor) {
  BE_U8 tile[32];
  BE_U16 row;

  for (row = 0; row < rowCount; row++) {
    BE_U16 x;

    for (x = 0; x < columns; x++) {
      makeResolutionProofTile(
          tile,
          x,
          (BE_U16)(firstRow + row),
          darkColor,
          brightColor);
      if (BE_displayBandWriteTile(
              band,
              x,
              (BE_U16)(firstRow + row),
              tile) != BE_DISPLAY_BAND_RESULT_OK) {
        return BE_FALSE;
      }
    }
  }
  return BE_TRUE;
}

static BE_Bool submitTerminalRasters(void) {
  BE_RasterProgram program;
  BE_U16 row;

  if (BE_rasterProgramReset(&program) != BE_RASTER_RESULT_OK)
    return BE_FALSE;
  for (row = 1; row < BODY_ROWS; row++) {
    const BE_U16 line = (BE_U16)(TOP_BAND_LINES + row * 8u);
    const BE_U16 phase = (BE_U16)(row & 3u);

    if (BE_rasterSetPaletteColor(
            &program,
            line,
            0,
            1,
            BE_PALETTE_COLOR_RGB4(1, 3u + phase, 5u + phase)) !=
            BE_RASTER_RESULT_OK ||
        BE_rasterSetPaletteColor(
            &program,
            line,
            0,
            2,
            BE_PALETTE_COLOR_RGB4(2u + phase, 8u + phase, 10u + phase)) !=
            BE_RASTER_RESULT_OK ||
        BE_rasterSetPaletteColor(
            &program,
            line,
            0,
            3,
            BE_PALETTE_COLOR_RGB4(8u + phase, 12u + phase, 15)) !=
            BE_RASTER_RESULT_OK) {
      return BE_FALSE;
    }
  }
  return BE_rasterSubmit(&program) == BE_RASTER_RESULT_OK;
}

static BE_Bool configureDisplay(void) {
  BE_DisplayBandProgram bands;

  return BE_displayBandProgramReset(&bands) == BE_DISPLAY_BAND_RESULT_OK &&
      BE_displayBandAppend(
          &bands,
          TOP_BAND_LINES,
          BE_DISPLAY_BAND_MODE_LORES_INDEXED_16,
          topChromePalette,
          16) == BE_DISPLAY_BAND_RESULT_OK &&
      BE_displayBandAppend(
          &bands,
          BODY_BAND_LINES,
          BE_DISPLAY_BAND_MODE_HIRES_INDEXED_4,
          terminalPalette,
          4) == BE_DISPLAY_BAND_RESULT_OK &&
      BE_displayBandAppend(
          &bands,
          BOTTOM_BAND_LINES,
          BE_DISPLAY_BAND_MODE_LORES_INDEXED_16,
          bottomChromePalette,
          16) == BE_DISPLAY_BAND_RESULT_OK &&
      BE_displayBandSubmit(&bands) == BE_DISPLAY_BAND_RESULT_OK &&
      drawPaletteBars(0, LORES_COLUMNS, 0, 2, 16) &&
      drawPaletteBars(1, HIRES_COLUMNS, 0, 2, 4) &&
      drawPaletteBars(2, LORES_COLUMNS, 0, 2, 16) &&
      drawResolutionProof(0, LORES_COLUMNS, 2, 2, 0, 15) &&
      drawResolutionProof(1, HIRES_COLUMNS, 2, BODY_ROWS - 2, 0, 3) &&
      drawResolutionProof(2, LORES_COLUMNS, 2, 2, 0, 15) &&
      submitTerminalRasters();
}

int MixedResolutionDemo_run(void) {
  int result = 1;

  if (!configureDisplay())
    goto shutdownDisplayBands;

  while (BE_runtimeIsRunning()) {
    if (BE_runtimeEndFrame() != BE_RUNTIME_RESULT_OK)
      goto shutdownDisplayBands;
  }
  result = 0;

shutdownDisplayBands:
  BE_rasterShutdown();
  BE_displayBandShutdown();
  return result;
}

