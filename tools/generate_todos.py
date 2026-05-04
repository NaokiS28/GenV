import subprocess
from collections import defaultdict
import re
from datetime import datetime
import argparse
from pathlib import Path

DEFAULT_OUTPUT_FILE = "TODOs.md"
DEFAULT_TAGS = ["TODO", "FIXME", "HACK", "NOTE", "BUG", "BUGS", "ERROR"]

HEADER_TEXT = """# TODO List

This file is auto-generated.
Do not edit manually.

"""

REGEX_TEMPLATE = r'//\s*{tag}\b[:\s]*'


def main():
    parser = argparse.ArgumentParser(description="Generate TODO list from git grep")

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
        help='Ignore path (quote patterns like "lib/"). Can be used multiple times.'
    )

    parser.add_argument(
        "--tags",
        nargs="+",
        default=DEFAULT_TAGS,
        help="List of tags to search for (default: TODO, FIXME, HACK, NOTE)"
    )

    parser.add_argument(
        "--nothing-msg",
        default=None,
        help="Optional message to write if no items found"
    )

    args = parser.parse_args()
    output_file = Path(args.output).resolve()
    script_file = Path(__file__).resolve()

    todos = defaultdict(list)

    for tag in args.tags:
        # Build git grep command
        pattern = f"// {tag}"  # comment-only match
        cmd = ["git", "grep", "-n", pattern, "--"]

        # User ignore paths
        for path in args.ignore:
            cmd.append(f":!{path}")

        # Always ignore output file and script file (relative to repo root)
        repo_root = subprocess.run(
            ["git", "rev-parse", "--show-toplevel"],
            capture_output=True,
            text=True,
            check=True
        ).stdout.strip()

        # Convert absolute paths to relative paths from repo root
        rel_output = output_file.relative_to(repo_root)
        rel_script = script_file.relative_to(repo_root)
        cmd.append(f":!{rel_output}")
        cmd.append(f":!{rel_script}")

        # Run git grep
        result = subprocess.run(
            cmd,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True
        )

        if result.returncode not in (0, 1):
            print(result.stderr)
            continue

        # Compile regex for stripping comment and tag
        CLEAN_RE = re.compile(REGEX_TEMPLATE.format(tag=tag), re.IGNORECASE)

        for line in result.stdout.splitlines():
            try:
                file, lineno, text = line.split(":", 2)
                cleaned = CLEAN_RE.sub("", text).strip()
                todos[file].append((lineno, cleaned))
            except ValueError:
                continue

    # Write Markdown output
    with open(output_file, "w", encoding="utf-8") as f:
        f.write(HEADER_TEXT)
        f.write(f"_Generated on {datetime.now()}_\n\n")

        if todos:
            for file in sorted(todos):
                f.write(f"#### [{file}]({file})\n\n")
                for lineno, text in todos[file]:
                    f.write(f"- [{lineno}]({file}?plain=1#L{lineno}): {text}\n")
                f.write("\n")
        else:
            if args.nothing_msg:
                f.write(args.nothing_msg + "\n")
            else:
                f.write("_No TODOs found_\n")

    print(f"Wrote {output_file}")


if __name__ == "__main__":
    main()