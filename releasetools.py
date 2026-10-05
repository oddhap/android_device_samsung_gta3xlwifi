# SPDX-License-Identifier: Apache-2.0
"""Keep the tested SM-T510 TWRP header aligned with each signed Android 14 OTA."""
import re
import struct
import common

_BIN = "gta3xlwifi_recovery_header_sync"
_TMP = "/tmp/" + _BIN
_STATUS = "/tmp/gta3xlwifi-recovery-header.status"


def _target(info):
    archive = getattr(info, "target_zip", None) or info.input_zip
    boot = archive.read("IMAGES/boot.img")
    if (boot[:8] != b"ANDROID!" or len(boot) < 2048 or
            struct.unpack_from("<I", boot, 40)[0] != 1 or
            boot[48:64].rstrip(b"\0") != b"SRPSA25A005KU"):
        raise ValueError("Unexpected SM-T510 boot image")
    props = dict(line.split("=", 1) for line in archive.read("SYSTEM/build.prop").decode().splitlines()
                 if "=" in line and not line.startswith("#"))
    release = props["ro.build.version.release"]
    patch = props["ro.build.version.security_patch"]
    if release not in ("14", "14.0", "14.0.0") or not re.fullmatch(r"20\d\d-(0[1-9]|1[0-2])-\d\d", patch):
        raise ValueError("Recovery synchronization is validated only for Android 14")
    year, month, _ = map(int, patch.split("-"))
    if not 2020 <= year <= 2127:
        raise ValueError("Patch year is outside the supported boot-header range")
    expected = (14 << 25) | ((year - 2000) << 4) | month
    if struct.unpack_from("<I", boot, 44)[0] != expected:
        raise ValueError("ROM properties and boot-header Keymaster inputs differ")
    binary = archive.read("SYSTEM/bin/" + _BIN)
    if binary[:5] != b"\x7fELF\x01" or struct.unpack_from("<H", binary, 18)[0] != 40:
        raise ValueError("Recovery synchronization helper must be ARM32 ELF")
    return binary, "0x%08x" % expected


def _begin(info):
    binary, word = _target(info)
    common.ZipWriteStr(info.output_zip, "recovery-header-sync", binary)
    info.script.AppendExtra('package_extract_file("recovery-header-sync", "%s");' % _TMP)
    info.script.AppendExtra('set_metadata("%s", "uid", 0, "gid", 0, "mode", 0755);' % _TMP)
    info.script.AppendExtra('assert(run_program("%s", "--preflight", "%s") == "0" || '
                            'abort("Cannot validate TWRP encryption compatibility; ROM installation stopped."));' % (_TMP, word))
    info.script.AppendExtra('if file_getprop("%s", "result") == "unsupported" then '
                            'ui_print("Installed recovery is not a supported TWRP build; it will be kept unchanged."); endif;' % _STATUS)


def _end(info):
    _, word = _target(info)
    info.script.AppendExtra('assert(run_program("%s", "--apply", "%s") == "0" || '
                            'abort("Cannot synchronize TWRP encryption compatibility."));' % (_TMP, word))
    info.script.AppendExtra('if file_getprop("%s", "result") == "synchronized" then '
                            'ui_print("TWRP encryption compatibility synchronized."); endif;' % _STATUS)


FullOTA_InstallBegin = _begin
FullOTA_InstallEnd = _end
IncrementalOTA_InstallBegin = _begin
IncrementalOTA_InstallEnd = _end
