from . import output

def confirm(prompt):
    if output.mode() == output.JSON:
        return False

    while True:
        print(prompt + " [yn]:", end=' ')
        result = input().lower().strip()
        if result and result in "yn":
            return True if result == "y" else False
        print("Please enter y/n.")
