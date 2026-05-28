# iPod Watch

A Pebble Time Steel (Basalt) watchface featuring a pixel art iPod Classic illustration with live time displayed on the iPod screen.

## Layout

- Full-screen pixel art iPod Classic (scanned and encoded as run-length arrays)
- 3-level grayscale: black outlines, light gray body, dark gray shading
- Time displayed in `BITHAM_34_MEDIUM_NUMBERS` centered in the iPod screen area
- AM/PM superscript in `GOTHIC_14_BOLD`
- White background

## CloudPebble Setup

1. New project → Platform: Basalt, Type: Watchface
2. Replace `main.c` with the one in this repo
3. Replace `package.json` with the one in this repo
4. Build & install — no font resources or additional files needed

## Structure

```
ipod-watch/
├── main.c        ← all code + pixel arrays inline
└── package.json
```
