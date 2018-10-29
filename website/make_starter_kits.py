#!/usr/bin/env python3
"""
Generate starter kit downloads and a download page.
"""

import argparse
import glob
import hashlib
import itertools
import json
import os
import shutil
import zipfile


ENVIRONMENT_DIR_HELP = "Directory containing precompiled Halite environment " \
                       "executables, each named after their platform. "
BOX_DIR_HELP = "Directory containing precompiled Halite-in-a-Box builds, " \
               "each named after their platform."
VERSION_HELP = "The version string to embed in the downloads page."
INCLUDED_EXTENSIONS = {
    ".bat",
    ".clj",
    ".coffee",
    ".cpp",
    ".cs",
    ".csproj",
    ".d",
    ".dart",
    ".erl",
    ".ex",
    ".exs",
    ".fs",
    ".fsproj",
    ".go",
    ".groovy",
    ".h",
    ".hpp",
    ".hs",
    ".java",
    ".jl",
    ".js",
    ".kt",
    ".lisp",
    ".lock",
    ".lua",
    ".m",
    ".md",
    ".ml",
    ".pas",
    ".php",
    ".pl",
    ".properties",
    ".pxd",
    ".py",
    ".pyx",
    ".rb",
    ".rkt",
    ".rs",
    ".sbt",
    ".scala",
    ".sh",
    ".sln",
    ".svc",
    ".swift",
    ".toml",
    ".txt",
}
INCLUDED_FILES = {
    "Makefile",
    "README",
    "REQUIRE",
    "LANGUAGE",
    "build.gradle",
}
STARTER_KIT_DIR = "../starter_kits"
DOWNLOAD_DATA = "_data/downloads.json"
PLATFORM_AGNOSTIC = "None"

# Kits that we support
OFFICIAL_KITS = {
    "Python3",
    "JavaScript",
    "Java",
    "C++",
    "Rust",
}

# Names of generated downloads
# Standard language + platform
OUTPUT_FILE_FORMAT = "assets/downloads/Halite3_{language}_{platform}.zip"

# Platform only
ENVIRONMENT_OUTPUT_FILE_FORMAT = "assets/downloads/Halite3_{platform}.zip"
BOX_OUTPUT_FILE_FORMAT = "assets/downloads/Halite3_Offline_{platform}{extension}"

# All languages + platform
ALL_LANGUAGES_OUTPUT_FILE_FORMAT = "assets/downloads/Halite3_all_{platform}.zip"
SOURCE_FILE = "assets/downloads/Halite3Source.zip"
BENCHMARK_FILE = "assets/downloads/Halite3Benchmark.zip"
BENCHMARK_MANIFEST = "assets/downloads/Halite3Benchmark.json"
TOOLS_FILE = "assets/downloads/Halite3Tools.zip"

REPLAY_README = """Replays and error logs will appear here if you use the run_game.sh or run_game.bat scripts.
"""

def detect_environments(directory):
    """Detect which platform binaries we have."""
    environments = [(PLATFORM_AGNOSTIC, None, None)]
    for filename in os.listdir(directory):
        platform, platform_ext = os.path.splitext(filename)

        if platform.startswith("."):
            # .DS_Store, .gitignore, etc.
            continue

        print("Detected platform", platform)
        environments.append((platform,
                             os.path.join(directory, filename),
                             "halite" + platform_ext))

    return environments


def scan_directory(full_path):
    """Figure out what the starter kit files in a directory are."""

    included_files = []
    for containing_dir, _, possible_files in os.walk(full_path):
        for filename in possible_files:
            _, ext = os.path.splitext(filename)
            if ext.lower() in INCLUDED_EXTENSIONS or filename in INCLUDED_FILES:
                included_files.append(os.path.join(containing_dir, filename))

    included_files.append(("README.md", open(os.path.join(STARTER_KIT_DIR, "README.md")).read()))
    included_files.append((".gitignore", open(os.path.join(STARTER_KIT_DIR, ".gitignore")).read()))
    included_files.append(("./docs/api-docs.md", open("./learn-programming-challenge/api-docs.md").read()))
    included_files.append(("./docs/game-overview.md", open("./learn-programming-challenge/game-overview.md").read()))
    return included_files


def make_archive(output, environment, base_path, included_files):
    """Create the output ZIP archive."""
    platform, source, target = environment
    # Get rid of duplicates
    included_files = list(set(included_files))
    with zipfile.ZipFile(output, "w", zipfile.ZIP_DEFLATED) as archive:
        if source is not None:
            # source is None <=> platform-agnostic archive
            zinfo = zipfile.ZipInfo.from_file(source, target)
            zinfo.compress_type = zipfile.ZIP_DEFLATED
            zinfo.external_attr = 0o0100755 << 16
            with open(source, 'rb') as source_file:
                archive.writestr(zinfo, source_file.read())

        for file in included_files:
            if isinstance(file, tuple):
                archive.writestr(file[0], file[1])
            else:
                target_path = os.path.relpath(file, base_path)
                archive.write(file, target_path)


def make_source_download():
    included_files = []

    for directory, _, file_list in os.walk("../game_engine"):
        target_dir = os.path.relpath(directory, "../game_engine")
        for filename in file_list:
            # Just include all files in directory, since we should be
            # deploying from a clean repo.
            source_path = os.path.join(directory, filename)
            target_path = os.path.normpath(
                os.path.join("Halite/", target_dir, filename))

            included_files.append((source_path, target_path))

    with zipfile.ZipFile(SOURCE_FILE, "w", zipfile.ZIP_DEFLATED) as archive:
        for source_path, target_path in included_files:
            archive.write(source_path, target_path)


def make_benchmark_download():
    included_files = []
    manifest = []

    def add_directory(root):
        for directory, _, file_list in os.walk(root):
            for filename in file_list:
                _, ext = os.path.splitext(filename)
                if ext.lower() in {".py"}:
                    source_path = os.path.join(directory, filename)
                    target_path = os.path.normpath(
                        os.path.join("benchmark/", os.path.relpath(source_path, start=root)))
                    if filename == 'MyBot.py':
                        target_path = os.path.normpath(
                            os.path.join("benchmark/", os.path.relpath(os.path.join(directory, 'RandomBot.py'), start=root)))

                    included_files.append((source_path, target_path))
                    digest = hashlib.sha256()
                    with open(source_path, "rb") as source_file:
                        digest.update(source_file.read())
                    manifest.append((target_path, digest.hexdigest()))

    add_directory("../starter_kits/benchmark")
    add_directory("../starter_kits/Python3")

    with zipfile.ZipFile(BENCHMARK_FILE, "w", zipfile.ZIP_DEFLATED) as archive:
        for source_path, target_path in included_files:
            archive.write(source_path, target_path)

    with open(BENCHMARK_MANIFEST, "w") as manifest_file:
        json.dump({
            "digest_type": "SHA-256",
            "manifest": manifest,
        }, manifest_file)


def make_tools_download():
    included_files = []
    for directory, _, file_list in os.walk("../tools/hlt_client/hlt_client"):
        for filename in file_list:
            _, ext = os.path.splitext(filename)
            if ext.lower() in {".py"}:
                source_path = os.path.join(directory, filename)
                target_path = os.path.normpath(
                    os.path.join("hlt_client/", filename))
                included_files.append((source_path, target_path))

    with zipfile.ZipFile(TOOLS_FILE, "w", zipfile.ZIP_DEFLATED) as archive:
        for source_path, target_path in included_files:
            archive.write(source_path, target_path)


def make_box_halite_download(box_dir):
    # Result is [platform independent, Mac, Linux, Windows] path links
    result = [None, None, None, None]
    # Halite-in-a-Box
    for filename in os.listdir(box_dir):
        if filename.startswith('.'):
            continue

        platform, extension = os.path.splitext(os.path.basename(filename))
        destination = BOX_OUTPUT_FILE_FORMAT.format(platform=platform, extension=extension)
        shutil.copy(os.path.join(box_dir, filename), destination)

        if platform == 'MacOS':
            result[1] = destination
        elif platform == 'Linux':
            result[2] = destination
        elif platform == 'Windows':
            result[3] = destination

    return result


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("version", help=VERSION_HELP)
    parser.add_argument("environment_dir", help=ENVIRONMENT_DIR_HELP)
    parser.add_argument("box_dir", help=BOX_DIR_HELP)

    args = parser.parse_args()

    environments = detect_environments(args.environment_dir)
    generated_languages = []

    # Ensure output directories exists
    for location in [SOURCE_FILE, DOWNLOAD_DATA]:
        try:
            os.makedirs(os.path.dirname(location))
        except FileExistsError:
            pass

    make_source_download()
    make_benchmark_download()
    make_tools_download()

    # Keep paths of all source files around so we can make a single combined
    # download at the end
    all_files = []

    for directory in os.listdir(STARTER_KIT_DIR):
        full_path = os.path.join(STARTER_KIT_DIR, directory)
        if not os.path.isdir(full_path):
            continue

        if directory in ("starterkitdocs", "benchmark"):
            continue

        language = directory
        generated_languages.append(language)
        print("Language:", language)

        included_files = scan_directory(full_path)
        for file in included_files:
            print("\tIncluding:", file[0] if isinstance(file, tuple) else file)

        print()

        all_files.extend(included_files)

        for (platform, source, target) in environments:
            output = "./" + OUTPUT_FILE_FORMAT.format(
                language=language, platform=platform)
            print("\tMaking:", output)
            make_archive(output, (platform, source, target),
                         full_path, included_files + [("replays/README.md", REPLAY_README)])

    panlanguage_kits = []
    for (platform, source, target) in environments:
        # Make downloads including all languages
        filename = ALL_LANGUAGES_OUTPUT_FILE_FORMAT.format(platform=platform)
        all_output = "./" + filename
        print("\tMaking:", all_output)
        make_archive(all_output, (platform, source, target), "../starter_kits", all_files)
        panlanguage_kits.append(filename)

        # Make downloads including no languages
        if source is None:
            continue
        output = "./" + ENVIRONMENT_OUTPUT_FILE_FORMAT.format(platform=platform)
        print("\tMaking:", output)
        make_archive(output, (platform, source, target), "", [])

    output = {
        "platforms": [environment[0] for environment in environments],
        "languages": [],
        "environments": [],
        "tools": [
            # Don't allow downloading benchmark bots
            # {
            #     "name": "Benchmark Bots",
            #     "files": [BENCHMARK_FILE, None, None, None],
            # },
            {
                "name": "Halite Visualizer & Gym",
                "files": make_box_halite_download(args.box_dir),
            },
            {
                "name": "CLI Client Tools",
                "files": [TOOLS_FILE, None, None, None],
            },
        ],
        "source": SOURCE_FILE,
        "version": args.version,
    }
    generated_languages.sort()
    for language in generated_languages:
        language_kits = []
        for (platform, _, _) in environments:
            language_kits.append(
                OUTPUT_FILE_FORMAT.format(
                    language=language, platform=platform))

        output["languages"].append({
            "language": language,
            "files": language_kits,
            "version": args.version if language in OFFICIAL_KITS else "Community-contributed",
        })

    output["languages"].append({
        "language": "All Languages",
        "files": panlanguage_kits,
    })

    for (platform, source, _) in environments:
        if source is None:
            continue
        output["environments"].append(
            ENVIRONMENT_OUTPUT_FILE_FORMAT.format(platform=platform))

    with open(DOWNLOAD_DATA, "w") as output_file:
        json.dump(output, output_file, indent=2)


if __name__ == "__main__":
    main()
