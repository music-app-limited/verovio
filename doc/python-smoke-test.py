"""Minimal installed-wheel smoke test for the Python bindings.

This validates that the extension imports, packaged resource files are
discoverable, and a trivial render returns SVG on the active interpreter.
It also checks that a tempo change within a measure starts at its own time
in the timemap and in the MIDI output.
"""

import base64
import importlib.resources as resources

import verovio


def run_smoke_test() -> None:
    toolkit = verovio.toolkit()
    print(toolkit.getVersion())
    toolkit.setResourcePath(str(resources.files("verovio") / "data"))
    toolkit.loadData(
        """<?xml version="1.0" encoding="UTF-8"?>
<mei xmlns="http://www.music-encoding.org/ns/mei" meiversion="5.1">
  <meiHead>
    <fileDesc>
      <titleStmt>
        <title>Verovio Python smoke test</title>
      </titleStmt>
      <pubStmt/>
    </fileDesc>
  </meiHead>
  <music>
    <body>
      <mdiv>
        <score>
          <scoreDef meter.count="4" meter.unit="4">
            <staffGrp>
              <staffDef n="1" lines="5" clef.shape="G" clef.line="2"/>
            </staffGrp>
          </scoreDef>
          <section>
            <measure n="1">
              <staff n="1">
                <layer n="1">
                  <note pname="c" oct="4" dur="4"/>
                </layer>
              </staff>
            </measure>
          </section>
        </score>
      </mdiv>
    </body>
  </music>
</mei>
"""
    )
    svg = toolkit.renderToSVG(1)
    assert "<svg" in svg, "renderToSVG did not return SVG output"


# One 4/4 measure with "Adagio" at beat 1 and "Presto" at beat 4.75, then one
# measure at the Presto tempo. Note onsets are at quarters 0, 2, 3, 3.75 and 4.
TEMPO_CHANGE_MEI = """<?xml version="1.0" encoding="UTF-8"?>
<mei xmlns="http://www.music-encoding.org/ns/mei" meiversion="5.1">
  <meiHead>
    <fileDesc>
      <titleStmt>
        <title>Tempo change within a measure</title>
      </titleStmt>
      <pubStmt/>
    </fileDesc>
  </meiHead>
  <music>
    <body>
      <mdiv>
        <score>
          <scoreDef meter.count="4" meter.unit="4">
            <staffGrp>
              <staffDef n="1" lines="5" clef.shape="G" clef.line="2"/>
            </staffGrp>
          </scoreDef>
          <section>
            <measure n="1">
              <staff n="1">
                <layer n="1">
                  <note pname="c" oct="4" dur="2"/>
                  <note pname="d" oct="4" dur="4"/>
                  <note pname="e" oct="4" dur="8" dots="1"/>
                  <note pname="f" oct="4" dur="16"/>
                </layer>
              </staff>
              <tempo staff="1" tstamp="1" mm="60" mm.unit="4" midi.bpm="60">Adagio</tempo>
              <tempo staff="1" tstamp="4.75" mm="160" mm.unit="4" midi.bpm="160">Presto</tempo>
            </measure>
            <measure n="2">
              <staff n="1">
                <layer n="1">
                  <note pname="g" oct="4" dur="1"/>
                </layer>
              </staff>
            </measure>
          </section>
        </score>
      </mdiv>
    </body>
  </music>
</mei>
"""


def read_varlen(data: bytes, pos: int) -> tuple[int, int]:
    value = 0
    while True:
        byte = data[pos]
        pos += 1
        value = (value << 7) | (byte & 0x7F)
        if not byte & 0x80:
            return value, pos


def read_midi_tempos(data: bytes) -> list[tuple[float, float]]:
    """Return (quarter note, BPM) for each set-tempo event of a MIDI file."""
    tpq = int.from_bytes(data[12:14], "big")
    tempos = []
    pos = 14
    while pos < len(data):
        length = int.from_bytes(data[pos + 4 : pos + 8], "big")
        track = data[pos + 8 : pos + 8 + length]
        pos += 8 + length
        tick, i, status = 0, 0, 0
        while i < len(track):
            delta, i = read_varlen(track, i)
            tick += delta
            if track[i] & 0x80:
                status = track[i]
                i += 1
            if status == 0xFF:
                meta = track[i]
                size, i = read_varlen(track, i + 1)
                if meta == 0x51:
                    microseconds = int.from_bytes(track[i : i + 3], "big")
                    tempos.append((tick / tpq, round(60_000_000 / microseconds, 3)))
                i += size
            elif status in (0xF0, 0xF7):
                size, i = read_varlen(track, i)
                i += size
            else:
                i += 1 if (status & 0xF0) in (0xC0, 0xD0) else 2
    return sorted(tempos)


def run_tempo_change_test() -> None:
    toolkit = verovio.toolkit()
    toolkit.setResourcePath(str(resources.files("verovio") / "data"))
    toolkit.loadData(TEMPO_CHANGE_MEI)

    # Quarters 0 to 3.75 at 60 BPM, quarters 3.75 to 8 at 160 BPM
    onsets = {entry["qstamp"]: entry["tstamp"] for entry in toolkit.renderToTimemap()}
    expected = {0.0: 0, 2.0: 2000, 3.0: 3000, 3.75: 3750, 4.0: 3844, 8.0: 5344}
    assert onsets == expected, f"Timemap onsets {onsets} != {expected}"

    tempos = read_midi_tempos(base64.b64decode(toolkit.renderToMIDI()))
    assert tempos == [(0.0, 60.0), (3.75, 160.0)], f"MIDI tempos {tempos} != [(0.0, 60.0), (3.75, 160.0)]"


def main() -> None:
    try:
        run_smoke_test()
        run_tempo_change_test()
    except Exception as exc:
        print(f"SMOKE TEST FAIL: {exc}")
        raise

    print("SMOKE TEST PASS")


if __name__ == "__main__":
    main()
