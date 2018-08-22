from . import output

def confirm(prompt, json_confirm=False):
    if output.mode() == output.JSON:
        return json_confirm

    while True:
        print(prompt + " [yn]:", end=' ')
        result = input().lower().strip()
        if result and result in "yn":
            return True if result == "y" else False
        print("Please enter y/n.")
