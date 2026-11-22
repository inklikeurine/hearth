#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-3.0-or-later
#
# Claude Code guard for git and GitHub commands (PreToolUse hook on Bash).
# Reads the tool call as JSON on stdin and answers with a permission decision:
#   deny: force pushes, pushing the local-only "spikes" branch, pushing every
#         branch at once, skipping git hooks, changing where git looks for hooks
#   ask:  any other push, and GitHub commands that publish or change things
# Anything else gets no answer, so the normal permission rules apply.
# Why: the lead approves every push, and spikes stay on this machine
# (AGENTS.md, "Code and repo safety").

set -u

input="$(cat)"
cmd="$(printf '%s' "$input" | jq -r '.tool_input.command // empty')"
cwd="$(printf '%s' "$input" | jq -r '.cwd // empty')"
[ -z "$cmd" ] && exit 0

decide() {
  # $1 = deny or ask, $2 = reason shown to Claude and the lead
  jq -n --arg d "$1" --arg r "$2" '{
    hookSpecificOutput: {
      hookEventName: "PreToolUse",
      permissionDecision: $d,
      permissionDecisionReason: $r
    }
  }'
  exit 0
}

has() { printf '%s' "$cmd" | grep -Eq -- "$1"; }

# Skipping or redirecting git hooks
if has '--no-verify' || has '(^|[;&| ])git( +-[^ ]+)* +commit( +[^;&|]*)? -[a-zA-Z]*n'; then
  decide deny "Skipping git hooks (--no-verify) is not allowed in this repo."
fi
if has 'core\.hooksPath' && ! has 'core\.hooksPath[ =]+\.githooks/?( |;|&|$)'; then
  decide deny "Changing core.hooksPath is not allowed: the repo's hooks live in .githooks/."
fi

# Pushes
if has '(^|[;&| ])git( +-[^ ]+)* +push'; then
  if has ' (--force|--force-with-lease|--force-if-includes|--mirror|--all|--delete)' \
    || has ' -[a-zA-Z]*[fd]( |$)' || has ' \+[^ ]'; then
    decide deny "Force pushes, deleting remote branches, and --all/--mirror pushes are not allowed."
  fi
  if has '(^| |:|/)spikes( |:|$)'; then
    decide deny "The spikes branch is local only and is never pushed."
  fi
  branch=""
  [ -n "$cwd" ] && branch="$(git -C "$cwd" rev-parse --abbrev-ref HEAD 2>/dev/null || true)"
  if [ "$branch" = "spikes" ]; then
    decide deny "You are on the local-only spikes branch; pushing from it is not allowed."
  fi
  decide ask "The lead approves every push."
fi

# GitHub CLI commands that publish or change things on GitHub
if has '(^|[;&| ])gh +(pr +(create|merge|close|edit|ready|review|comment)|repo +(create|delete|edit|rename|archive|fork|sync)|release +(create|delete|edit|upload)|(secret|variable) +(set|delete)|workflow +(run|enable|disable)|issue +(create|close|edit|comment|delete))'; then
  decide ask "This changes or publishes something on GitHub; the lead approves it."
fi
if has '(^|[;&| ])gh +api' && has '(-X|--method)[ =]*(POST|PUT|PATCH|DELETE)|(^| )-(f|F)[ =]|--field|--raw-field|--input'; then
  decide ask "This GitHub API call writes; the lead approves it."
fi

exit 0
