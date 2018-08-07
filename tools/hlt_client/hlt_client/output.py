"""
Utilities to switch between console and JSON output.
"""

import json
import sys

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
        message = jsonify(message)
        if isinstance(message, dict):
            result = { "status": "success" }
            result.update(message)
            print(format_json(result))
        else:
            print(format_json({ "status": "success", "message": message }))
    else:
        print(message)
