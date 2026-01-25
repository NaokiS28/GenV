import subprocess
from collections import defaultdict
import re
from datetime import datetime
import argparse

DEFAULT_OUTPUT_FILE = "TODOs.md"
PATTERN = "// TODO"  # Keep this so git grep matches only comments

# Text placed at top of markdown file
HEADER_TEXT = """# TODO List

This file is auto-generated.
Do not edit manually.

"""

# Clean regex:
# - Matches leading whitespace
# - Matches // TODO (any capitalization)
# - Matches optional colon or space after TODO
CLEAN_RE = re.compile(r'//\s*TODO\b[:\s]*', re.IGNORECASE)

def main():
    parser = argparse.ArgumentParser(description="Generate TODO list from git grep")

    # Optional positional output path
    parser.add_argument(
        "output",
        nargs="?",
        default=DEFAULT_OUTPUT_FILE,
        help=f"Path to output markdown file (default: {DEFAULT_OUTPUT_FILE})"
    )

    parser.add_argument(
        "--ignore",
        action="append",
        default=[],
        help='Ignore path (quote patterns like "lib/*"). Can be used multiple times.'
    )

    args = parser.parse_args()
    output_file = args.output

    # Build git grep command
    cmd = ["git", "grep", "-n", PATTERN]

    # Always start pathspec section
    cmd.append("--")

    # User ignores
    for path in args.ignore:
        cmd.append(f":!{path}")

    # Ignore output file
    cmd.append(f":!{output_file}")

    # Ignore this script itself
    cmd.append(":!tools/generate_todos.py")

    result = subprocess.run(
        cmd,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True
    )

    if result.returncode not in (0, 1):
        print(result.stderr)
        return

    todos = defaultdict(list)

    for line in result.stdout.splitlines():
        try:
            file, lineno, text = line.split(":", 2)
            # Remove comment marker and TODO prefix
            cleaned = CLEAN_RE.sub("", text).strip()
            todos[file].append((lineno, cleaned))
        except ValueError:
            continue

    with open(output_file, "w", encoding="utf-8") as f:
        f.write(HEADER_TEXT)
        f.write(f"_Generated on {datetime.now()}_\n\n")

        for file in sorted(todos):
            f.write(f"#### [{file}]({file})\n\n")
            for lineno, text in todos[file]:
                f.write(f"- [{lineno}]({file}#{lineno}): {text}\n")
            f.write("\n")

    print(f"Wrote {output_file}")

if __name__ == "__main__":
    main()
