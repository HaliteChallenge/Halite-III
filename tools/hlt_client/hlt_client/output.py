"""
Utilities to switch between console and JSON output.
"""

import json
import sys
import textwrap

_mode = 'text'

JSON = 'json'

def set_mode(mode):
    global _mode
    _mode = mode


def mode():
    global _mode
    return _mode


def format_json(obj):
    return json.dumps(obj, indent=None)


def jsonify(obj):
    if hasattr(obj, "to_json"):
        return obj.to_json()

    return obj


def error(message, **fields):
    if mode() == JSON:
        message = jsonify(message)
        print(format_json({ "status": "error", "message": message }))
    else:
        print(message, file=sys.stderr)


def output(message, **fields):
    if mode() == JSON:
        result = { "status": "success" }
        result.update(fields)
        message = jsonify(message)
        if isinstance(message, dict):
            result.update(message)
        else:
            result["message"] = message
        print(format_json(result))
        sys.stdout.flush()
    else:
        print(message)


def warning(message, **fields):
    output(message, status="warning", **fields)


def print_list(title, items, formatter=lambda x: str(x)):
    if mode() == JSON:
        output(title, items=items)
    else:
        print(title)
        for item in items:
            print(' *', textwrap.indent(formatter(item), prefix='   ').lstrip())
