#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-or-later
"""
Runs every case in git-guard-cases.txt through git-guard.py and checks its answer.
Prints each case that gave the wrong answer, and exits 1 if there were any.
The guard only reads the commands; nothing in them runs. Branch checks run
against two throwaway repos (one on main, one on spikes) in a temporary folder.
Usage: python3 -I .claude/hooks/test-git-guard.py
"""

import importlib.util
import json
import os
import subprocess
import sys
import tempfile

sys.dont_write_bytecode = True  # loading the guard mustn't leave __pycache__ in .claude/hooks
here = os.path.dirname(os.path.abspath(__file__))
guard = os.path.join(here, "git-guard.py")
cases = os.path.join(here, "git-guard-cases.txt")


def make_repo(path, branch):
    def git(*args):
        subprocess.run(["git", "-C", path, *args], check=True, capture_output=True)
    os.makedirs(path)
    git("init", "-q", "-b", "main")
    git("-c", "user.name=test", "-c", "user.email=test@example.invalid",
        "commit", "-q", "--allow-empty", "-m", "start")
    git("branch", "spikes")
    if branch != "main":
        git("switch", "-q", branch)


def load_guard():
    spec = importlib.util.spec_from_file_location("git_guard", guard)
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


def run_guard(command, cwd):
    """Run the guard the way Claude Code does, as a program fed JSON."""
    data = json.dumps({"tool_name": "Bash", "tool_input": {"command": command}, "cwd": cwd})
    out = subprocess.run([sys.executable, "-I", guard], input=data, capture_output=True, text=True)
    if out.returncode != 0 or not out.stdout.strip():
        return f"no answer (exit {out.returncode}) {out.stderr.strip()}"
    return json.loads(out.stdout)["hookSpecificOutput"]["permissionDecision"]


def answer_for(module, command, cwd):
    try:
        v = module.judge(command, cwd)
    except Exception as error:
        return f"crash ({type(error).__name__}: {error})"
    return "deny" if v.deny else "ask" if v.ask else "none"


def main():
    module = load_guard()
    failures = total = 0
    with tempfile.TemporaryDirectory() as tmp:
        repos = {b: os.path.join(tmp, b) for b in ("main", "spikes")}
        for branch, path in repos.items():
            make_repo(path, branch)
        with open(cases, encoding="utf-8") as f:
            for number, line in enumerate(f, 1):
                line = line.rstrip("\n")
                if not line.strip() or line.startswith("#"):
                    continue
                expect, _, command = line.partition("\t")
                expect, _, branch = expect.partition("@")
                command = command.replace("\\n", "\n")
                got = answer_for(module, command, repos[branch or "main"])
                total += 1
                if got != expect:
                    failures += 1
                    print(f"git-guard-cases.txt:{number}: expected {expect}, got {got}: {command!r}")
        # Once end to end, as a program: the JSON Claude Code reads
        got = run_guard("git push origin main", repos["main"])
        total += 1
        if got != "ask":
            failures += 1
            print(f"run as a program: expected ask, got {got}")
    print(f"{total - failures} of {total} guard cases passed")
    return 1 if failures else 0


if __name__ == "__main__":
    sys.exit(main())
