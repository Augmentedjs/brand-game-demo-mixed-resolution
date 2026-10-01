# Brand 2 Mixed Resolution Demo

This compatibility demo exercises Brand Engine 2 display bands on Atari STe.
It presents a reusable 200-line terminal layout:

- 32 lines of 320-pixel, 16-color top chrome
- 136 lines of 640-pixel, 4-color terminal content
- 32 lines of 320-pixel, 16-color bottom chrome

The middle band also applies three deterministic palette changes at 8-line
boundaries. The generated bars and checker patterns make mode and palette
boundaries visible without external assets.

The portable demo code uses public Brand Engine headers only. The Atari STe
launcher is intentionally thin and owns target adapter installation. This
first version advertises only the `atari-ste` Builder target because no other
target currently provides the public Display Band contract.

## Build locally

```sh
make atari \
  ENGINE_ROOT=/path/to/brand-engine \
  BUILDER_ROOT=/path/to/brand-builder
```

The resulting program is `dist/mixedres.prg`.

## Build through Brand Builder

Register or select this repository as a game source and build the `atari-ste`
platform. The `brand-build` entrypoint rejects unsupported build coordinates
explicitly.

## Controls

Close the application or emulator to exit. The scene is a visual compatibility
probe and has no interactive controls.

