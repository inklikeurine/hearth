#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-or-later
"""
Claude Code guard for shell commands (PreToolUse hook on Bash).

Reads the tool call as JSON on stdin and answers with a permission decision:
  deny: force pushes, deleting branches on GitHub, pushing many branches at
        once, pushing or deleting the local-only "spikes" branch, skipping git
        hooks, changing where git looks for hooks, deleting .git
  ask:  any other push; git commands that throw away work (reset --hard, clean,
        checking out or restoring files over changes, stash drop, branch -D);
        GitHub commands that publish or change things; shell commands that
        write to .githooks/, .claude/settings*.json, .claude/hooks/ or .git/config
Anything else gets no answer, so the normal permission rules apply.

It splits the command into words the way a shell does, so it finds git after
"git -C <folder>", inside "bash -c '...'", "$(...)", backquotes and heredocs fed
to a shell, and it doesn't read text inside a commit message as flags.
It catches mistakes, not someone set on getting round it ("G=git; $G push"
gets past). The git hooks in .githooks/ are the backstop.

If the guard itself fails, it asks rather than lets the command through.
Why: the lead approves every push, spikes stay on this machine, and the guards
can't be loosened without the lead (AGENTS.md, "Code and repo safety").
Tests: .claude/hooks/git-guard-cases.txt, run by .claude/hooks/test-git-guard.py
(part of tools/dev/check).
"""

import json
import os
import re
import shlex
import subprocess
import sys

PUNCT = ";&|()<>\n`"
SHELLS = {"sh", "bash", "zsh", "dash", "ksh", "fish"}
C_RUNNERS = SHELLS | {"su", "runuser", "script"}  # take a command string after -c
INTERPRETERS = {"python", "python3", "perl", "node", "ruby"}
# Words that can stand before a command without being the command
PREFIX_WORDS = {"{", "}", "!", "if", "then", "else", "elif", "do", "while", "until",
                "time", "exec", "command", "builtin", "nohup", "sudo", "doas"}
# Commands that run another command given in their arguments
WRAPPERS = {"env", "nice", "ionice", "timeout", "xargs", "stdbuf", "chronic", "unbuffer",
            "distrobox", "distrobox-enter", "flatpak-spawn", "toolbox", "podman", "docker",
            "find", "watch", "parallel", "setsid", "flock", "sudo", "doas", "su", "runuser"}
# Commands that write, move or delete the files named in their arguments
WRITERS = {"rm", "rmdir", "unlink", "shred", "mv", "cp", "install", "ln", "chmod", "chown",
           "touch", "truncate", "tee", "dd", "rsync", "patch"}
IN_PLACE = {"sed", "perl"}  # write only with -i

ASSIGN = re.compile(r"^[A-Za-z_][A-Za-z0-9_]*=")
PROTECTED = re.compile(r"(^|/)(\.githooks(/|$)|\.claude/?$|\.claude/(settings[^/]*\.json|hooks)(/|$)"
                       r"|\.git/(config|hooks)(/|$))")
PROTECTED_IN_CODE = re.compile(r"\.githooks|\.claude/(settings|hooks)|\.git/(config|hooks)")
DOT_GIT = re.compile(r"(^|/)\.git/?$")
HEREDOC = re.compile(r"(?<!<)<<(?!<)-?\s*(?:'([^']*)'|\"([^\"]*)\"|\\?([A-Za-z_][A-Za-z0-9_]*))")
SPIKES_REF = re.compile(r"(^|[:/+])spikes($|[:~^])")

PUSH_DENIED = ["--force", "--force-with-lease", "--force-if-includes", "--mirror", "--all",
               "--branches", "--delete", "--prune"]

GH_WRITES = {
    "pr": {"create", "merge", "close", "reopen", "edit", "ready", "review", "comment",
           "lock", "unlock", "update-branch", "revert"},
    "issue": {"create", "close", "reopen", "edit", "comment", "delete", "transfer",
              "pin", "unpin", "lock", "unlock", "develop"},
    "repo": {"create", "delete", "edit", "rename", "archive", "unarchive", "fork", "sync",
             "deploy-key"},
    "release": {"create", "delete", "edit", "upload", "delete-asset"},
    "secret": {"set", "delete"},
    "variable": {"set", "delete"},
    "workflow": {"run", "enable", "disable"},
    "run": {"cancel", "rerun", "delete"},
    "label": {"create", "edit", "delete", "clone"},
    "gist": {"create", "edit", "delete", "rename"},
    "ssh-key": {"add", "delete"},
    "gpg-key": {"add", "delete"},
    "cache": {"delete"},
    "extension": {"install", "upgrade", "remove"},
    "project": {"create", "edit", "delete", "close", "copy", "link", "unlink", "mark-template",
                "item-add", "item-create", "item-edit", "item-delete", "item-archive",
                "field-create", "field-delete"},
}


class Verdict:
    def __init__(self):
        self.deny, self.ask = [], []

    def add(self, kind, reason):
        target = self.deny if kind == "deny" else self.ask
        if reason not in target:
            target.append(reason)


def base(word):
    return os.path.basename(word)


def short_flags(word, value_letters=""):
    """Letters in a short-option cluster like -fd, stopping at a letter that takes a value."""
    if not re.fullmatch(r"-[A-Za-z0-9]+", word):
        return ""
    letters = ""
    for ch in word[1:]:
        letters += ch
        if ch in value_letters:
            break
    return letters


def long_matches(word, options, minimum=4):
    """True if word is one of options or an abbreviation git would accept for it."""
    name = word.split("=", 1)[0]
    return name.startswith("--") and len(name) >= minimum and any(o.startswith(name) for o in options)


# ---------------------------------------------------------------- splitting

def strip_heredocs(text):
    """Drop heredoc bodies (they're data), but keep a body fed to a shell as commands."""
    lines = text.split("\n")
    kept, scripts = [], []
    i = 0
    while i < len(lines):
        line = lines[i]
        kept.append(line)
        i += 1
        for m in HEREDOC.finditer(line):
            delim = m.group(1) or m.group(2) or m.group(3) or ""
            body = []
            while i < len(lines) and lines[i].strip() != delim:
                body.append(lines[i])
                i += 1
            i += 1
            if re.search(r"(^|[\s;&|(/])(%s)(\s|$)" % "|".join(SHELLS), line[:m.start()]):
                scripts.append("\n".join(body))
    return "\n".join(kept), scripts


def words_of(text):
    lex = shlex.shlex(text, posix=True, punctuation_chars=PUNCT)
    lex.whitespace = " \t\r"
    lex.whitespace_split = True
    lex.commenters = ""
    return list(lex)


def segments(text, depth=0):
    """Split command text into simple commands: [{"words": [...], "redirects": [...]}]."""
    if depth > 6:
        return []
    text, scripts = strip_heredocs(text)
    found = []
    current = {"words": [], "redirects": []}
    redirect_next = False
    for word in words_of(text):
        if word and all(c in PUNCT for c in word):
            if any(c in "()`" for c in word) or not any(c in "<>" for c in word):
                if current["words"] or current["redirects"]:
                    found.append(current)
                current = {"words": [], "redirects": []}
                redirect_next = False
            else:
                redirect_next = True
            continue
        if redirect_next:
            current["redirects"].append(word)
            redirect_next = False
            continue
        current["words"].append(word)
        if "$(" in word or "`" in word:
            found.extend(segments(word, depth + 1))
    if current["words"] or current["redirects"]:
        found.append(current)
    for script in scripts:
        found.extend(segments(script, depth + 1))
    # Command strings handed to a shell: bash -c '...', eval ...
    nested = []
    for seg in found:
        words = seg["words"]
        if any(base(w) in C_RUNNERS for w in words):
            for k, w in enumerate(words[:-1]):
                if re.fullmatch(r"-[a-z]*c", w):
                    nested.extend(segments(words[k + 1], depth + 1))
        start = command_index(words)
        if start < len(words) and words[start] == "eval":
            nested.extend(segments(" ".join(words[start + 1:]), depth + 1))
    return found + nested


def command_index(words):
    i = 0
    while i < len(words) and (ASSIGN.match(words[i]) or words[i] in PREFIX_WORDS):
        i += 1
    return i


def find_program(words, name):
    """Index of the word that runs program `name` in this simple command, or None."""
    start = command_index(words)
    for k, w in enumerate(words):
        if base(w) != name or k < start:
            continue
        if k == start or any(base(x) in WRAPPERS or x == "--" for x in words[:k]):
            return k
    return None


# ---------------------------------------------------------------- git

def split_git(args):
    """Global options, -C folders, subcommand, and the subcommand's arguments."""
    takes_value = {"-C", "-c", "--git-dir", "--work-tree", "--namespace", "--config-env",
                   "--super-prefix", "--attr-source"}
    options, folders = [], []
    i = 0
    while i < len(args) and args[i].startswith("-"):
        if args[i] in takes_value and i + 1 < len(args):
            options.append((args[i], args[i + 1]))
            if args[i] == "-C":
                folders.append(args[i + 1])
            i += 2
        else:
            options.append((args[i], None))
            i += 1
    sub = args[i] if i < len(args) else ""
    return options, folders, sub, args[i + 1:]


def current_branch(cwd, folders):
    path = cwd or "."
    for folder in folders:
        path = os.path.join(path, os.path.expanduser(folder))
    try:
        out = subprocess.run(["git", "-C", path, "rev-parse", "--abbrev-ref", "HEAD"],
                             capture_output=True, text=True, timeout=3)
        return out.stdout.strip()
    except (OSError, subprocess.SubprocessError):
        return ""


def check_git(args, cwd, v):
    options, folders, sub, rest = split_git(args)

    # Where git looks for hooks
    for opt, value in options:
        if opt == "-c" and value and value.lower().startswith("core.hookspath"):
            if value.split("=", 1)[-1].rstrip("/") != ".githooks":
                v.add("deny", "Changing core.hooksPath is not allowed: the repo's hooks live in .githooks/.")
        if opt == "--config-env" and value and value.lower().startswith("core.hookspath"):
            v.add("deny", "Changing core.hooksPath is not allowed: the repo's hooks live in .githooks/.")
    if sub == "config":
        keys = [k for k, w in enumerate(rest) if w.lower() == "core.hookspath"]
        removing = any(w in ("--unset", "--unset-all", "unset", "--remove-section", "remove-section",
                             "--rename-section", "rename-section") for w in rest)
        if keys:
            values = [w for w in rest[keys[0] + 1:] if not w.startswith("-")]
            if removing or (values and values[0].rstrip("/") != ".githooks"):
                v.add("deny", "Changing core.hooksPath is not allowed: the repo's hooks live in .githooks/.")
        elif removing and "core" in rest:
            v.add("deny", "Removing git's core settings would turn off the repo's hooks.")

    # Skipping hooks
    if any(w.startswith("--no-veri") for w in rest):
        v.add("deny", "Skipping git hooks (--no-verify) is not allowed in this repo.")
    if sub == "commit":
        skip = False
        for w in rest:
            if skip:
                skip = False
                continue
            letters = short_flags(w, value_letters="mFCctSu")
            if "n" in letters:
                v.add("deny", "Skipping git hooks (commit -n) is not allowed in this repo.")
            if letters and letters[-1] in "mFCct" and letters == w[1:]:
                skip = True  # the next word is that option's value, e.g. the message

    if sub in ("rm", "mv", "apply", "update-index") and any(PROTECTED.search(w) for w in rest):
        v.add("ask", "This changes the repo's guards (.githooks/, .claude/ or .git/config); the lead approves it.")

    if sub == "push":
        check_push(rest, cwd, folders, v)
    elif sub == "branch":
        check_branch(rest, cwd, folders, v)
    elif sub in ("checkout", "switch"):
        names = [w for w in rest if not w.startswith("-")]
        forced_create = [w for w in rest if w in ("-B", "-C", "--force-create")]
        if forced_create:
            if any(n == "spikes" for n in names):
                v.add("deny", "This would reset the local-only spikes branch, which has no other copy.")
            else:
                v.add("ask", "-B/-C resets an existing branch to a new place; the lead checks it.")
        if sub == "checkout" and ("--" in rest or "." in rest):
            v.add("ask", "Checking out files overwrites their uncommitted changes.")
        if any(w in ("-f", "--force", "--discard-changes") for w in rest):
            v.add("ask", "Switching with --force throws away uncommitted changes.")
    elif sub == "restore":
        letters = "".join(short_flags(w, "s") for w in rest)
        staged_only = ("--staged" in rest or "S" in letters) and not ("--worktree" in rest or "W" in letters)
        if not staged_only:
            v.add("ask", "git restore overwrites uncommitted changes in the working files.")
    elif sub == "reset":
        if any(long_matches(w, ["--hard"]) for w in rest):
            v.add("ask", "git reset --hard throws away uncommitted changes.")
    elif sub == "clean":
        letters = "".join(short_flags(w, "e") for w in rest)
        dry = "n" in letters or any(long_matches(w, ["--dry-run"]) for w in rest)
        if not dry and ("f" in letters or "--force" in rest):
            v.add("ask", "git clean deletes files git doesn't track; they can't be got back.")
    elif sub == "stash":
        if rest and rest[0] in ("drop", "clear"):
            v.add("ask", "Dropping a stash loses the changes kept in it.")
    elif sub == "reflog":
        if rest and rest[0] in ("expire", "delete"):
            v.add("ask", "Expiring the reflog removes the way back to lost commits.")
    elif sub == "gc":
        if any(w.startswith("--prune") for w in rest):
            v.add("ask", "git gc --prune removes lost commits for good.")
    elif sub == "update-ref":
        if any(SPIKES_REF.search(w) for w in rest):
            v.add("deny", "This would change the local-only spikes branch directly.")
        elif "-d" in rest or "--stdin" in rest:
            v.add("ask", "update-ref changes branches directly; the lead checks it.")
    elif sub in ("filter-branch", "filter-repo"):
        v.add("ask", "Rewriting history across the repo; the lead checks it.")
    elif sub == "worktree":
        if rest and rest[0] == "remove" and any(w in ("-f", "--force") for w in rest):
            v.add("ask", "Removing a worktree with --force throws away its uncommitted changes.")


def check_push(rest, cwd, folders, v):
    plain = []
    skip = False
    for w in rest:
        if skip:
            skip = False
            continue
        if w.startswith("--"):
            if long_matches(w, PUSH_DENIED):
                v.add("deny", "Force pushes, deleting branches on GitHub, and --all/--mirror/--prune pushes are not allowed.")
            if w in ("--repo", "--push-option", "--receive-pack", "--exec") :
                skip = True
            continue
        letters = short_flags(w, value_letters="o")
        if letters:
            if "f" in letters or "d" in letters:
                v.add("deny", "Force pushes and deleting branches on GitHub are not allowed.")
            if letters.endswith("o") and letters == w[1:]:
                skip = True
            continue
        plain.append(w)
    for ref in plain[1:]:  # plain[0] is the remote
        if ref.startswith("+"):
            v.add("deny", "A '+' refspec is a force push; not allowed.")
        if ref.startswith(":"):
            v.add("deny", "Pushing ':branch' deletes that branch on GitHub; not allowed.")
        if "*" in ref:
            v.add("deny", "Pushing many branches at once is not allowed.")
        if SPIKES_REF.search(ref):
            v.add("deny", "The spikes branch is local only and is never pushed.")
    if current_branch(cwd, folders) == "spikes":
        v.add("deny", "You are on the local-only spikes branch; pushing from it is not allowed.")
    v.add("ask", "The lead approves every push.")


def check_branch(rest, cwd, folders, v):
    letters = "".join(short_flags(w) for w in rest)
    longs = [w for w in rest if w.startswith("--")]
    names = [w for w in rest if not w.startswith("-")]
    deleting = "d" in letters or "D" in letters or any(long_matches(w, ["--delete"]) for w in longs)
    moving = "m" in letters or "M" in letters or any(long_matches(w, ["--move"]) for w in longs)
    forced = any(c in letters for c in "fDMC") or any(long_matches(w, ["--force"]) for w in longs)
    if deleting or moving or forced:
        on_spikes = moving and len(names) < 2 and current_branch(cwd, folders) == "spikes"
        if on_spikes or any(n == "spikes" or n.endswith("/spikes") for n in names):
            v.add("deny", "The spikes branch has no other copy; deleting, renaming or resetting it is not allowed.")
        elif forced:
            v.add("ask", "A forced branch change can lose commits that aren't merged anywhere.")


# ---------------------------------------------------------------- gh

def check_gh(args, v):
    i = 0
    while i < len(args) and args[i].startswith("-"):
        i += 2 if args[i] in ("-R", "--repo") else 1
    if i >= len(args):
        return
    sub, rest = args[i], args[i + 1:]
    verb = next((w for w in rest if not w.startswith("-")), "")
    if sub in GH_WRITES and verb in GH_WRITES[sub]:
        v.add("ask", "This changes or publishes something on GitHub; the lead approves it.")
    if sub == "api":
        writes = False
        for k, w in enumerate(rest):
            method = None
            if w in ("-X", "--method") and k + 1 < len(rest):
                method = rest[k + 1]
            elif w.startswith("-X") and len(w) > 2:
                method = w[2:]
            elif w.startswith("--method="):
                method = w.split("=", 1)[1]
            if method and method.upper() != "GET":
                writes = True
            if w in ("-f", "-F", "--field", "--raw-field", "--input") or re.match(r"-(f|F).|--(raw-)?field=|--input=", w):
                writes = True
        if writes:
            v.add("ask", "This GitHub API call writes; the lead approves it.")


# ---------------------------------------------------------------- files

def check_files(seg, v):
    words = seg["words"]
    for target in seg["redirects"]:
        if PROTECTED.search(target):
            v.add("ask", "This writes to the repo's guards (.githooks/, .claude/ or .git/config); the lead approves it.")
    start = command_index(words)
    if start >= len(words):
        return
    prog, args = base(words[start]), words[start + 1:]
    if prog in ("sudo", "doas", "env", "nice", "timeout", "xargs", "nohup"):
        rest = [w for w in args if not w.startswith("-") and not ASSIGN.match(w) and not w.isdigit()]
        if rest:
            prog, args = base(rest[0]), args[args.index(rest[0]) + 1:]
    writes = prog in WRITERS or (prog in IN_PLACE and any(w.startswith("-i") or w == "--in-place" for w in args))
    if prog == "find" and any(w in ("-delete", "-exec", "-execdir") for w in args):
        writes = True
    if prog in ("cp", "install", "rsync", "ln"):
        args = args[-1:]  # only the destination is written
    if writes and any(PROTECTED.search(w) for w in args):
        v.add("ask", "This writes to the repo's guards (.githooks/, .claude/ or .git/config); the lead approves it.")
    if prog in ("rm", "rmdir", "shred") and any(DOT_GIT.search(w) for w in args):
        v.add("deny", "Deleting .git loses the repo's history and the local-only spikes branch.")
    if prog in INTERPRETERS:
        for k, w in enumerate(args[:-1]):
            if w in ("-c", "-e", "-E") and PROTECTED_IN_CODE.search(args[k + 1]):
                v.add("ask", "This code touches the repo's guards (.githooks/, .claude/ or .git/config); the lead approves it.")


# ---------------------------------------------------------------- main

def judge(command, cwd):
    v = Verdict()
    try:
        segs = segments(command)
    except ValueError:
        if re.search(r"\b(git|gh)\b|\.githooks|\.claude/|\.git/config", command):
            v.add("ask", "The guard couldn't split this command into words (unclosed quote?), so the lead checks it.")
        return v
    for seg in segs:
        words = seg["words"]
        k = find_program(words, "git")
        if k is not None:
            check_git(words[k + 1:], cwd, v)
        k = find_program(words, "gh")
        if k is not None:
            check_gh(words[k + 1:], v)
        check_files(seg, v)
    return v


def answer(kind, reason):
    print(json.dumps({"hookSpecificOutput": {
        "hookEventName": "PreToolUse",
        "permissionDecision": kind,
        "permissionDecisionReason": reason,
    }}))


def main():
    try:
        data = json.load(sys.stdin)
        command = (data.get("tool_input") or {}).get("command") or ""
        if not command:
            return
        v = judge(command, data.get("cwd") or "")
    except Exception as error:  # a broken guard asks; it never waves a command through
        answer("ask", f"The guard hit an error ({type(error).__name__}: {error}); the lead checks this command.")
        return
    if v.deny:
        answer("deny", " ".join(v.deny))
    elif v.ask:
        answer("ask", " ".join(v.ask))


if __name__ == "__main__":
    main()
